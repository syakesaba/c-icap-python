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
#include <c_icap/request.h>
#include <c_icap/simple_api.h>

#include "pyci_service.h"
#include "pyci_script.h"
#include "pyci_debug.h"
#define POSSIBLE_PYINT_ASLONG_FAIL -1

int python_check_preview_handler(char *preview_data, int preview_data_len, ci_request_t *req)
{

    pyci_debug_printf(PYCI_INFO_LEVEL, "starts");
    PyObject *pInstance = (PyObject *)ci_service_data(req);

    if (!ci_req_hasbody(req))
    {
        pyci_debug_printf(PYCI_INFO_LEVEL, "No body! preview_data will be '' ");
    }

    // データのロックをするかどうかの確認
    PyObject *pBool = PyObject_GetAttrString(pInstance, PYCI_CLASS_BOOL_LOCK_DATA); // MUST DECREF pBool
    if (pBool != NULL && PyBool_Check(pBool))
    {
        if (!PyObject_IsTrue(pBool))
        {
            // 動的にロック、アンロックができるように、チェックします。
            pyci_debug_printf(PYCI_MESSAGE_LEVEL, PYCI_CLASS "." PYCI_CLASS_BOOL_LOCK_DATA " = False. unlocking data!");
            ci_req_unlock_data(req);
        }
        else
        {
            pyci_debug_printf(PYCI_MESSAGE_LEVEL, PYCI_CLASS "." PYCI_CLASS_BOOL_LOCK_DATA " = True. locking data.");
            ci_req_lock_data(req);
        }
        Py_DECREF(pBool);
    }
    else
    { // Error Occured!
        pyci_debug_printf(PYCI_ERROR_LEVEL, "No " PYCI_CLASS "." PYCI_CLASS_BOOL_LOCK_DATA " defined... unlocking data.");
        return CI_MOD_ERROR; // self.lock無かったYO！
    }

    // プレビューデータを使用するかどうかの確認及びプレビューデータの送信
    PyObject *pMethod = PyObject_GetAttrString(pInstance, PYCI_CLASS_METHOD_HANDLE_PREVIEW_DATA); // E+
    PyObject *pRet = NULL;
    if (pMethod != NULL && PyCallable_Check(pMethod))
    {
        Py_DECREF(pMethod);
        PyObject *pName = PyString_FromString(PYCI_CLASS_METHOD_HANDLE_PREVIEW_DATA);
        if (pName == NULL)
        {
            pyci_debug_printf(PYCI_ERROR_LEVEL,
                              "FAIL at PyString_FromString(" PYCI_CLASS_METHOD_HANDLE_PREVIEW_DATA ") returned NULL");
            goto preview_error;
        }
        PyObject *pPreview_data = PyString_FromStringAndSize(preview_data, preview_data_len);
        if (pPreview_data == NULL)
        {
            pyci_debug_printf(PYCI_ERROR_LEVEL,
                              "FAIL at PyString_FromStringAndSize(preview_data,preview_data_len) returned NULL");
            Py_DECREF(pName); // 用なし
            goto preview_error;
        }
        pRet = PyObject_CallMethodObjArgs(pInstance, pName, pPreview_data, NULL); // E+
        Py_DECREF(pPreview_data);                                                 // 用なし
        Py_DECREF(pName);                                                         // 用なし
        if (pRet != NULL)
        {                                         // pRetはエラーならNULL、
            int ret_preview = PyInt_AsLong(pRet); // キャストエラーなら-1を返すらしいぜ
            Py_DECREF(pRet);                      // 一応オブジェクト入ってたみたいだから、用なし宣言
            if (ret_preview == POSSIBLE_PYINT_ASLONG_FAIL && PyErr_Occurred())
            {
                pyci_debug_printf(PYCI_ERROR_LEVEL, "function " PYCI_CLASS_METHOD_HANDLE_PREVIEW_DATA " returned Invalid object.(not int)");
                goto preview_error;
            }
            return ret_preview;
        }
        else
        {
            pyci_debug_printf(PYCI_ERROR_LEVEL, "Error in calling function " PYCI_CLASS_METHOD_HANDLE_PREVIEW_DATA);
            goto preview_error;
        }
    }
    else
    { /* プレビューデータを使わないと判断 */
        pyci_debug_printf(PYCI_MESSAGE_LEVEL, "Can't call " PYCI_CLASS_METHOD_HANDLE_PREVIEW_DATA ". ignored preview!");
        python_service_io(NULL, NULL, preview_data, &preview_data_len, 0, req); /* プレビューデータをio関数に引き渡す。 */
        return CI_MOD_CONTINUE;
    }
preview_error:
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    Py_XDECREF(pMethod);
    Py_XDECREF(pRet);

    return CI_MOD_ERROR;
}
