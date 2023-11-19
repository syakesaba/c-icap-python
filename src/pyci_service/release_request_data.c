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

#include "pyci_service.h"
#include "pyci_debug.h"

void python_release_request_data(void *data)
{
    pyci_debug_printf(PYCI_INFO_LEVEL, "starts");
    PyObject *pInstance = (PyObject *)data;
    /*
    struct ruby_vars *rvars = (struct ruby_vars *)data;
    ci_debug_printf(9, "ruby_release_request_data()\n");
    free(rvars);
    */
    Py_DECREF(pInstance); // ガベコレ！
}
