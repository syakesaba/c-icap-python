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

/*
 * 必ず一度しか呼ばれない
 * ================================================================
 * This program called when the c-icap starts and loads .so
 * Next is -> load_module.c
 * ================================================================
 * #### No Error Messages can't seen.
 */

#include <Python.h>
#include "c_icap/service.h"

#include "pyci_debug.h"

int init_python_handler(struct ci_server_conf *server_conf) {
    pyci_debug_printf(PYCI_INFO_LEVEL,"starts");
    if (!Py_IsInitialized()) {
    	Py_Initialize();
    }
    /* pythonを初期化する。というか、インタプリタを立ち上げる */
    return CI_SERVICE_OK;
}
