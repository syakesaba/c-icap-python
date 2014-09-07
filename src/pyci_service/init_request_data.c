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
#include "c_icap/c-icap.h"
#include "c_icap/simple_api.h"

#include "../pyci_module.h"
#include "../pyci_service.h"
#include "../pyci_script.h"
#include "../pyci_module.h"
#include "../pyci_debug.h"

/**
 * make instance from pClass.
 *
 * @param req is pointer to ci_request_t holds request data.
 */
void *python_init_request_data(ci_request_t *req) {
    pyci_debug_printf(PYCI_INFO_LEVEL,"starts");

    const int REQ_TYPE = ci_req_type(req);
    const char * METHOD_TYPE = ci_method_string(REQ_TYPE);//Don't free!

    pyci_debug_printf(PYCI_MESSAGE_LEVEL,"request %s has come", METHOD_TYPE);

    ci_headers_list_t *hdrs;
    PyObject * pMethodName;
    PyObject * pTuple;
    PyObject * pList;
    PyObject * pDict;
    PyObject * pString;
    int i;

    if (REQ_TYPE == ICAP_REQMOD) {
        hdrs = ci_http_request_headers(req);
    } else if (REQ_TYPE == ICAP_RESPMOD) {
        hdrs = ci_http_response_headers(req);
    } else if (REQ_TYPE == ICAP_OPTIONS){
        // something? ;
        return NULL;
    } else {
        // UNKNOWN protocol
        pyci_debug_printf(PYCI_ERROR_LEVEL,"Error unsupported ICAP_METHOD_TYPE %d", REQ_TYPE);
        return NULL;
    }

    //get sub interp
    pyci_service_data_t * service_data = (pyci_service_data_t *)(req->service_data);
    // hold GIL
    PyThreadState_Swap(service_data->pThreadState);
    PyObject * pClass = service_data->pClass;
    pTuple = PyTuple_New(2);//*args to pass. refcount = 1;
    pMethodName = PyString_FromString(METHOD_TYPE);//pMethodName.refcount = 1
    PyTuple_SetItem(pTuple,0,pMethodName);//args[0] = "REQMOD" or "RESPMOD"
    //pMethodName.refcount => stolen! => no DECREF
    if (hdrs != NULL) {
        pList = PyList_New(hdrs->used);//pList.refcount = 1;
        for(i = 0; i < hdrs->used; i++) {
            pString = PyString_FromString(hdrs->headers[i]);//pString.refcount = 1
            PyList_SetItem(pList,i,pString);
            //pString.refcount => stolen! => no DECREF
        }
    }
    PyTuple_SetItem(pTuple,1,pList);//args[1] = list of http header.
    //pList.refcount => stolen! => no decref
    pDict = PyDict_New();//pDict.refcount = 1;
    PyObject * pInstance = PyInstance_New(pClass, pTuple, pDict);//pInstance.refcount = 1;
    Py_DECREF(pTuple);
    Py_DECREF(pDict);
    if (PyErr_Occurred()) {
        PyErr_Print();
        pyci_debug_printf(PYCI_ERROR_LEVEL, "Error at creating instance of class "PYCI_CLASS" \n");
        return NULL;
    }
    //release GIL
    PyThreadState_Swap(NULL);
    return (void *)pInstance;//pInstance.refcount = 1;
}
