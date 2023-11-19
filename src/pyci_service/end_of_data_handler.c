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
#include <c_icap/service.h>
#include <c_icap/request.h>
#include <c_icap/simple_api.h>

#include "pyci_service.h"
#include "pyci_script.h"
#include "pyci_debug.h"

int python_end_of_data_handler(ci_request_t *req) {
    pyci_debug_printf(PYCI_INFO_LEVEL, "");
    PyObject * pInstance = (PyObject *)ci_service_data(req);
    PyObject * pMethod = PyObject_GetAttrString(pInstance,PYCI_CLASS_METHOD_HANDLE_END_OF_DATA);//E+
    PyObject * pRet = NULL;
    if (pMethod != NULL && PyCallable_Check(pMethod)) {
        Py_DECREF(pMethod);//用なし
        PyObject * pName = PyString_FromString(PYCI_CLASS_METHOD_HANDLE_END_OF_DATA);
        pRet = PyObject_CallMethodObjArgs(pInstance, pName, NULL);//E+
        Py_DECREF(pName);//用なし
        if (pRet != NULL) { //pRetはエラーならNULL、
            int ret_end_of_data = PyInt_AsLong(pRet);//キャストエラーなら-1を返すらしいぜ
            Py_DECREF(pRet);//一応オブジェクト入ってたみたいだから、用なし宣言
#define POSSIBLE_PYINT_ASLONG_FAIL -1
            if (ret_end_of_data == POSSIBLE_PYINT_ASLONG_FAIL && PyErr_Occurred()) {
                pyci_debug_printf(PYCI_ERROR_LEVEL,
                        "function "PYCI_CLASS_METHOD_HANDLE_END_OF_DATA" returned Invalid object.(not int)");
                goto end_of_data_error;
            }
            if (!ci_req_sent_data(req)) {
                replace_headers(req);
            }
            return ret_end_of_data; /* #############################3成功ならばここが唯一の脱出口 */
        } else {
            pyci_debug_printf(PYCI_ERROR_LEVEL,"Error in calling function "PYCI_CLASS_METHOD_HANDLE_END_OF_DATA);
            goto end_of_data_error;
        }
    } else {
        pyci_debug_printf(PYCI_ERROR_LEVEL,"Error Can't call "PYCI_CLASS_METHOD_HANDLE_END_OF_DATA);
    }
end_of_data_error:
    if (PyErr_Occurred()) {
        PyErr_Print();
    }
    Py_XDECREF(pMethod);
    Py_XDECREF(pRet);

    return CI_MOD_ERROR;
}
