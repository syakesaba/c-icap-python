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

#include "pyci_module.h" //pClassアクセス
#include "pyci_debug.h"

/*
 * 一度だけ呼ばれる
 */
void release_python_handler() {
    pyci_debug_printf(PYCI_INFO_LEVEL,"starts");
    if (Py_IsInitialized()) {
        pyci_debug_printf(PYCI_MESSAGE_LEVEL,"Finalizing Python!");
        Py_XDECREF(pClass);
        Py_Finalize();
    }
    return;
}
