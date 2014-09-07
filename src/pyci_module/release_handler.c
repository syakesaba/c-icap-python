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

#include "../pyci_module.h" //pClassアクセス
#include "../pyci_service.h"
#include "../pyci_debug.h"

int end_interpreters(void * value,const char * name, const void *);

/**
 * called once when shutdown the daemon.
 */
void release_python_handler() {
    pyci_debug_printf(PYCI_INFO_LEVEL,"starts");
    ci_dyn_array_iterate(service_interp,NULL,end_interpreters);
    //hold GIL
    PyThreadState_Swap(pMainThreadState);
    if (Py_IsInitialized()) {
        pyci_debug_printf(PYCI_MESSAGE_LEVEL,"Finalizing Python!");
        Py_Finalize();
    }
    return;
}

/**
 * function for ci_dyn_array_iterate.<br>
 * for-each end interpreter.
 *
 * @param data NULL
 * @param name key
 * @param value value
 * @return 0 whenever
 */
int end_interpreters(void * value,const char * name, const void * ex) {
    pyci_service_data_t * service_data = (pyci_service_data_t *)value;
    //hold GIL
    PyThreadState_Swap(service_data->pThreadState);
    Py_DECREF(service_data->pClass);
    Py_DECREF(service_data->pModule);
    Py_EndInterpreter(service_data->pThreadState);
    //Release GIL
    PyThreadState_Swap(NULL);
    pyci_debug_printf(PYCI_MESSAGE_LEVEL,"closing interpreter for %s",name);
    return EXIT_SUCCESS;
}
