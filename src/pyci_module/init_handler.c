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
#include "c_icap/service.h"

#include "../pyci_debug.h"
#include "../pyci_module.h"

/**
 * This program called when the c-icap daemon start.<br>
 * Initialize [Python Global Interpreter](https://docs.python.org/2/c-api/init.html#initializing-and-finalizing-the-interpreter)<br>
 * <br>
 * Next hook: pyci_module/load_module.c
 *
 * @param server_conf is a pointer to the c-icap server config.
 * @return CI_SERVICE_OK always.
 * @see load_python_module(const char * service_file) next hook
 */
int init_python_handler(struct ci_server_conf *server_conf) {
    pyci_debug_printf(PYCI_INFO_LEVEL, "starts");
    if (!Py_IsInitialized()) {
        //http://www.linuxjournal.com/article/3641?page=0,2
        Py_Initialize();//initialize python and get GIL
        if (!PyEval_ThreadsInitialized()) {
            PyEval_InitThreads();//enable threading
            pMainThreadState = PyThreadState_Get();//save main thread
        }
    }
    service_interp = ci_dyn_array_new(MAX_INTERPRETER);
    PyThreadState_Swap(NULL);//release GIL
    return CI_SERVICE_OK;
}
