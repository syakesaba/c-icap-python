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
#include "service.h"
#include "cfg_param.h"

/*
 * 一度しかよばれない
 * Debug output will start after here the file.
 * ================================================================
 * Prev is <- ../pyci_service/init_service.c
 * Next is -> ../pyci_service/post_init_service.c
 * ================================================================
 */

#include "../pyci_debug.h"

int post_init_python_handler(struct ci_server_conf *server_conf) {
    pyci_debug_printf(PYCI_INFO_LEVEL,"starts");
    if (! Py_IsInitialized()) {
        pyci_debug_printf(PYCI_ERROR_LEVEL,"FAIL at Py_Initialize()");
        return CI_SERVICE_NOT_INITIALIZED;
    } else if (PyErr_Occurred()) {
        PyErr_Print();
        pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL after Py_Initialize()");
        return CI_SERVICE_ERROR;
    }
    return CI_SERVICE_OK;
}
