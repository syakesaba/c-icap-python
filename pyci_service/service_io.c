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
#include "request.h"

#include "../pyci_script.h"
#include "../pyci_debug.h"

//rlen,wlenもポインタになっていることに注意！
int python_service_io(char * wbuf, int * wlen, char * rbuf, int * rlen, int iseof, ci_request_t * req) {
    pyci_debug_printf(PYCI_INFO_LEVEL,"starts");

    PyObject * pInstance = (PyObject *)ci_service_data(req);

    int ret = CI_OK;
    PyObject * pRet = NULL;
    PyObject * pMethod = NULL;

    if (rlen && rbuf) { // io = IN MODE
        pyci_debug_printf(PYCI_INFO_LEVEL,"io = READ MODE!");
        if (*rlen < 0) {
            pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at (*rlen < 0) : rlen = %d", *rlen);
            ret = CI_ERROR;
            goto service_io_in_error;
        } else {
            PyObject * pRbuf = PyString_FromStringAndSize(rbuf, *rlen);//E+ N
            if (pRbuf == NULL) {
                pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at PyString_FromStringAndSize(%s,%d)",rbuf,*rlen);
                ret = CI_ERROR;
                goto service_io_in_error;
            }
            pMethod = PyString_FromString(PYCI_CLASS_METHOD_HANDLE_READ_DATA);//E N
            if (pMethod == NULL) {
                pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at PyString_FromString(%s)", PYCI_CLASS_METHOD_HANDLE_READ_DATA);
                //これはC側の問題。
                //ありえないが・・・
                ret = CI_ERROR;
                goto service_io_in_error;
            }
            //pRbufにはrlenの情報も入っているので、引数はrbufだけとなっている。
            //サイズを知りたいときはpython側でlen()関数でもつかってね。
            pRet = PyObject_CallMethodObjArgs(pInstance, pMethod, pRbuf, NULL);//E+
            Py_DECREF(pRbuf);
            if (pRet == NULL) {
                pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at PyObject_CallMethodObjArgs(...) : pRet is NULL!");
                ret = CI_ERROR;
                goto service_io_in_error;
            }
            ret = PyInt_AsLong(pRet);
#define PYCAST_ASLONG_FAIL -1
            if (ret == PYCAST_ASLONG_FAIL) {
                pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at PyInt_AsLong(pRet)");
                ret = CI_ERROR;
                goto service_io_in_error;
            }
        }
    }

    if (wlen && wbuf) { // io = OUT MODE
        pyci_debug_printf(PYCI_INFO_LEVEL,"io = WRITE MODE! wlen=%d", *wlen);
        pMethod = PyString_FromString(PYCI_CLASS_METHOD_HANDLE_WRITE_DATA);
        if (pMethod == NULL) {
            ret = CI_ERROR;
            pyci_debug_printf(PYCI_ERROR_LEVEL,
                    "FAIL : PyString_FromString(%s); returned NULL", PYCI_CLASS_METHOD_HANDLE_WRITE_DATA);
            goto service_io_out_error;
        }
        PyObject * pWlen = PyInt_FromSize_t(*wlen);
        if (pWlen == NULL) {
            ret = CI_ERROR;
            pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL : PyInt_FromSize_t(*wlen) returned NULL");
            goto service_io_out_error;
        }
        PyObject * pWbuf = PyObject_CallMethodObjArgs(pInstance, pMethod, pWlen, NULL);//E+
        Py_DECREF(pWlen);
        if (pWbuf == NULL) {
            ret = CI_ERROR;
            pyci_debug_printf(PYCI_ERROR_LEVEL,
                    "FAIL : PyObject_CallMethodObjArgs(...); returned pWbuf=NULL");
            goto service_io_out_error;
        }
        char * data = PyString_AsString(pWbuf);
        if (data == NULL) {
            ret = CI_ERROR;
            pyci_debug_printf(PYCI_ERROR_LEVEL,
                    "FAIL : PyString_AsString(pWbuf); returned NULL");
            goto service_io_out_error;
        }
        int datalen = strlen(data);
        if (datalen > *wlen) {
            ret = CI_ERROR;
            pyci_debug_printf(PYCI_ERROR_LEVEL, "Error: write_data() returned more than requested: %d for %d", datalen, *wlen);
        }
        if (datalen == 0) {
            *wlen = CI_EOF;
        } else {
            *wlen = datalen;
            memcpy(wbuf, data, *wlen * sizeof(char));
        }
    }

service_io_in_error:
service_io_out_error:
    if (PyErr_Occurred()) {
        PyErr_Print();
    }
    Py_XDECREF(pRet);
    Py_XDECREF(pMethod);
    return ret;
}
