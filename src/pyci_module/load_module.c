/*
    Embedded Python interpreter module for C-ICAP server
    Copyright (C) 2013  SYA-KE

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include <Python.h>
#include <libgen.h>
#include "c_icap/c-icap.h"
#include "c_icap/service.h"

#include "../pyci_service.h"
#include "../pyci_module.h"
#include "../pyci_script.h"
#include "../pyci_module.h"
#include "../pyci_debug.h"

int append_python_path(const char * service_file);//if ret != 0, ERROR.
PyObject * import_python_module(const char * service_file);//if NULL, ERROR.

/**
 *  PyEval_AcquireLock();                // get the GIL
    PyThreadState * myThreadState = Py_NewInterpreter();

    PyRun_SimpleString("A=1;");

    PyEval_ReleaseThread(myThreadState); // swap out thread state + release the GIL
*/

/**
 * Loads a service (a python script).<br>
 * This function sometimes called multiple times.<br>
 * <br>
 * Prev hook: pyci_module/init_handler.c<br>
 * Next hook: pyci_service/init_service.c<br>
 *
 * @param service_file is a full-file path of a service.
 * @return ci_service_module_t is a pointer of a service instance. NULL if fail to laod.
 * @see init_python_handler(struct ci_server_conf *server_conf)
 * @see python_init_service(ci_service_xdata_t *srv_xdata, struct ci_server_conf *server_conf)
 */
ci_service_module_t * load_python_module(const char * service_file){
    pyci_debug_printf(PYCI_INFO_LEVEL,"starts");
    pyci_debug_printf(PYCI_INFO_LEVEL, "loading %s", service_file);

    PyObject * pModule = NULL;
    PyObject * pClass = NULL;
    PyObject * pList = NULL;
    PyObject * pService_Name = NULL;

    ci_service_module_t * service; /* こいつはpythonスクリプト一個につき一個だけ生成される */

    // Allocate memory for service instance
    service = (ci_service_module_t *)malloc(sizeof(ci_service_module_t));
    if (service == NULL) {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at Allocating memory for ci_service_module_t of %s", service_file);
        return NULL;
    }

    // Create New Interpreter
    PyThreadState * pThreadState = Py_NewInterpreter();
    //hold GIL
    PyThreadState_Swap(pThreadState);
    //If NULL is returned, it fails and no exception is set.
    if (pThreadState == NULL) {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL creating new interpreter(s) of %s", service_file);
        free(service);
        return NULL;
    }

    // append python script's directory to sys.path
    if (append_python_path(service_file) != EXIT_SUCCESS){
        //ERROR
        pyci_debug_printf(PYCI_ERROR_LEVEL, "Could not append path for %s", service_file);
        goto could_not_setup_service;
    }

    // import python script file
    pModule = import_python_module(service_file);//pModule.refcount = 1
    if (pModule == NULL) {
        //ERROR
        pyci_debug_printf(PYCI_ERROR_LEVEL, "Could not import %s", service_file);
        goto could_not_setup_service;
    }

    // get pModule.ICAP class.
    pClass = PyObject_GetAttrString(pModule, PYCI_CLASS);//pObj.refcount = 1
    if (pClass == NULL) {
        //ERROR
        pyci_debug_printf(PYCI_ERROR_LEVEL, "Error at PyObject_GetAttrString(pModule, "PYCI_CLASS")" );
        goto could_not_setup_service;
    }
    pyci_service_data_t * service_data = NULL;
    // check is callable.(like PyType_IsSubType(pObj, PyClass_Type))
    if (PyCallable_Check(pClass)) {
        service_data = (pyci_service_data_t *)malloc(sizeof(pyci_service_data_t));
        if (service_data == NULL) {
            pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at Allocating memory for pyci_service_data_t of %s", service_file);
            goto could_not_setup_service;
        }
        service_data->pModule = pModule;
        service_data->pClass = pClass;
    } else {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at PyCallable_Check(pClass) : class '"PYCI_CLASS"' is not Callable");
        goto could_not_setup_service;
    }

    //END OF GET CLASS
    //REF:
    //    service_data->pClass => 1
    //    service_data->pModule => 1

    int mode = 0;
    //pick up supported METHODS
    pList = PyObject_GetAttrString(service_data->pClass, PYCI_CLASS_LIST_SUPPORTED_METHODS);//New Ref!
    if (pList == NULL || !PyList_Check(pList)) {
        pyci_debug_printf(PYCI_ERROR_LEVEL,
                "FAIL at PyObject_GetAttrString(service_data->pClass, "PYCI_CLASS_LIST_SUPPORTED_METHODS") is not List");
        Py_XDECREF(pList);
        goto could_not_setup_service;
    }
    Py_ssize_t sizeof_pList = -1;
    sizeof_pList = PyList_Size(pList);// No doc...
    if (sizeof_pList == -1) {
        pyci_debug_printf(PYCI_ERROR_LEVEL,
                "FAIL at PyObject_GetAttrString(service_data->pClass, "PYCI_CLASS_LIST_SUPPORTED_METHODS") is not List");
        Py_DECREF(pList);
        goto could_not_setup_service;
    }
    Py_ssize_t i;
    char * item;
    for(i=0;i<sizeof_pList;i++) {
        PyObject * pItem = PyList_GetItem(pList,i);//Borrowed Ref! DO NOT DECREF(pItem)
        if (pItem == NULL || !PyString_Check(pItem)) {
            Py_XDECREF(pItem);
            pyci_debug_printf(
                PYCI_ERROR_LEVEL,
                "(%s) invalid value in "PYCI_CLASS"."PYCI_CLASS_LIST_SUPPORTED_METHODS"[%d]",
                service_file,i
            );
            Py_DECREF(pList);
            goto could_not_setup_service;
        }
        //TODO: int error = PyString_AsStringAndSize(pItem, item, max);
        item = PyString_AsString(pItem);//DO NOT free(item)
        if (item == NULL){
            /* NOT_REACHED */
            pyci_debug_printf(
                PYCI_ERROR_LEVEL,
                "(%s) invalid value in "PYCI_CLASS"."PYCI_CLASS_LIST_SUPPORTED_METHODS"[%d]",
                service_file,i
            );
            Py_DECREF(pList);
            goto could_not_setup_service;
        }
        if (strstr(item, ci_method_string(ICAP_REQMOD))) {
            mode = mode | ICAP_REQMOD;
            pyci_debug_printf(PYCI_MESSAGE_LEVEL, "(%s) : REQMOD enabled", service_file);
        }
        if (strstr(item,ci_method_string(ICAP_RESPMOD))) {
            mode = mode | ICAP_RESPMOD;
            pyci_debug_printf(PYCI_MESSAGE_LEVEL, "(%s) : RESPMOD enabled", service_file);
        }
    }
    if (mode == 0) {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at mode selection for %s : see "PYCI_CLASS"."PYCI_CLASS_LIST_SUPPORTED_METHODS, service_file);
        Py_DECREF(pList);
        goto could_not_setup_service;
    }

    Py_DECREF(pList);

    //get service name from service file.
    char * service_name;
    pService_Name = PyObject_GetAttrString(pClass, PYCI_CLASS_STRING_SERVICE_NAME);//New ref!
    if (pService_Name == NULL || !PyString_Check(pService_Name)) {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "(%s) : "PYCI_CLASS"."PYCI_CLASS_STRING_SERVICE_NAME" is unknown", service_file);
        Py_XDECREF(pService_Name);
        goto could_not_setup_service;
    }
    //TODO: int error = PyString_AsStringAndSize
    service_name = PyString_AsString(pService_Name);//dont dealloc!
    if (service_name == NULL) {
        /* NOT_REACHED */
        pyci_debug_printf(PYCI_ERROR_LEVEL, "(%s) : "PYCI_CLASS"."PYCI_CLASS_STRING_SERVICE_NAME" is unknown", service_file);
        Py_DECREF(pService_Name);
        goto could_not_setup_service;
    }
    pyci_debug_printf(PYCI_MESSAGE_LEVEL, "(%s) : SERVICE_NAME is %s", service_file, service_name);

    Py_DECREF(pService_Name);

    //Save SubInterpreter(for IO)
    service_data->pThreadState = pThreadState;

    //Save SubInterpreter(for release_handler.c)
    ci_dyn_array_add(service_interp,service_name,(void *)service_data,sizeof(pyci_service_data_t));

    //setup service
    service->mod_data = (void *)service_data;
    service->mod_conf_table = NULL;
    service->mod_init_service = python_init_service;
    service->mod_post_init_service = python_post_init_service;
    service->mod_close_service = python_close_service;
    service->mod_init_request_data = python_init_request_data;
    service->mod_release_request_data = python_release_request_data;
    service->mod_check_preview_handler = python_check_preview_handler;
    service->mod_end_of_data_handler = python_end_of_data_handler;
    service->mod_service_io = python_service_io;
    service->mod_name = service_name;
    service->mod_short_descr = NULL;
    service->mod_type = mode;

    pyci_debug_printf(PYCI_MESSAGE_LEVEL,
            "(%s) : OK! service loaded", service_file);

    //release GIL
    PyThreadState_Swap(NULL);

    return service;

    could_not_setup_service:
    //show error to stdout
    if (PyErr_Occurred()) {
        PyErr_Print();
    }
    Py_XDECREF(pClass);
    Py_XDECREF(pModule);
    free(service);
    //shutdown sub-interpreter
    Py_EndInterpreter(pThreadState);
    //release GIL
    PyThreadState_Swap(NULL);
    return NULL;
}

#define PYLIST_APPEND_SUCCESS 0
#define PYSYS_SET_OBJECT_SUCCESS 0

/**
 * Append script's dirname to interpreter's sys.path
 *
 * @param service_file is a fullpath of python script.
 * @return EXIT_SUCCESS (0) if success.
 */
int append_python_path(const char * service_file) {
    int ret = EXIT_SUCCESS;
    const char * dir_name = NULL;
    dir_name = dirname(service_file);//returns pointer. not allocted one.
    if (dir_name == NULL) {
        pyci_debug_printf(PYCI_ERROR_LEVEL,"Error Invalid filename %s", service_file);
        return EXIT_FAILURE;
    }
    PyObject * pName = PyString_FromString(dir_name);//PLEASE DECREF pName
    if (pName == NULL) {
        pyci_debug_printf(PYCI_ERROR_LEVEL,"Error in PyString_FromString() of %s", service_file);
        return EXIT_FAILURE;
    }
    //pick up python sys.path
    PyObject * pPath = PySys_GetObject(PYTHONPATH_LOCATION_IN_SYS);//DO NOT DECREF pPath
    if (pPath == NULL) {
        pyci_debug_printf(PYCI_ERROR_LEVEL,"Error in PySys_GetObject("PYTHONPATH_LOCATION_IN_SYS") of %s", service_file);
        Py_DECREF(pName);
        return EXIT_FAILURE;
    }
    //append dirname of python script to sys.path
    ret = PyList_Append(pPath,pName);//append does not steal REF count.(clones pName)
    if (ret != PYLIST_APPEND_SUCCESS) {
        pyci_debug_printf(PYCI_ERROR_LEVEL,"Error in PyList_Append(pPath,pName) of %s", service_file);
        Py_DECREF(pName);
        return EXIT_FAILURE;
    }
    // set new python sys.path
    ret = PySys_SetObject(PYTHONPATH_LOCATION_IN_SYS,pPath);
    if (ret != PYSYS_SET_OBJECT_SUCCESS) {
        pyci_debug_printf(PYCI_ERROR_LEVEL,"Error in PySys_SetObject('"PYTHONPATH_LOCATION_IN_SYS"',pPath) of %s", service_file);
        Py_DECREF(pName);
        return EXIT_FAILURE;
    }
    Py_DECREF(pName);
    //DONT DECREF pPath
    return EXIT_SUCCESS;
}

/**
 * import python script as module
 *
 * @param service_file is a fullpath of a python script.
 * @return pModule(REF=1) if success. NULL if fail.
 */
PyObject * import_python_module(const char * service_file) {
    char * cp_service_file = strndup(service_file,strlen(service_file));
    if (cp_service_file == NULL) {
        pyci_debug_printf(PYCI_ERROR_LEVEL,"Could not allocating memory for %s", service_file);
        return NULL;
    }
    char * service_file_name = (char *)basename(cp_service_file);//don't free(service_file_name)
    //service_file always endswith .py (depends c-icap)
    service_file_name[strlen(service_file_name)-3] = '\0'; // -3 = strlen(".py") equal to basename $1 .py
    PyObject * pModule = PyImport_ImportModule(service_file_name);// new ref.
    free(cp_service_file);//also frees service_file_name.
    if (pModule == NULL) {
        //error importing module.
        pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at PyImport_Import(%s)", service_file_name);
        return NULL;
    }
    //DONT DECREF pModule!(b/c used in return value)
    return pModule;
}
