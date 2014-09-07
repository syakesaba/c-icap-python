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

#include "../pyci_script.h"
#include "../pyci_module.h"
#include "../pyci_debug.h"

/*
 * ICAPリクエストが到着する度に呼び出される。
 * ここでPythonクラスをインスタンスに変え、
 * 一つのICAPトランザクションで共有する。
 */
void *python_init_request_data(ci_request_t *req) {
    pyci_debug_printf(PYCI_INFO_LEVEL,"starts");

    const int REQ_TYPE = ci_req_type(req);
    const char * METHOD_TYPE = ci_method_string(REQ_TYPE);//Don't free!

    pyci_debug_printf(PYCI_MESSAGE_LEVEL,"request %s has come", METHOD_TYPE);

    ci_headers_list_t *hdrs;//HTTPヘッダが入る構造体
    PyObject * pMethodName;
    PyObject * pTuple;
    PyObject * pList;
    PyObject * pDict;
    PyObject * pString;//TODO: pMethodNameが節約できるなあ
    int i;

    if (REQ_TYPE == ICAP_REQMOD) {
        hdrs = ci_http_request_headers(req);
    } else if (REQ_TYPE == ICAP_RESPMOD) {
        hdrs = ci_http_response_headers(req);
    } else if (REQ_TYPE == ICAP_OPTIONS){
        // something? ;
        return NULL;//OPTIONSリクエストはスルー
    } else {
        // UNKNOWN protocol
        pyci_debug_printf(PYCI_ERROR_LEVEL,"Error unsupported ICAP_METHOD_TYPE %d", REQ_TYPE);
        //変なリクエストもスルー
        return NULL;
    }

    pTuple = PyTuple_New(2);//クラスの__init__に渡す引数の数。 TODO: 他にも情報を送る場合増やす必要がある。 //E+
    pMethodName = PyString_FromString(METHOD_TYPE);//E+
    PyTuple_SetItem(pTuple,0,pMethodName);//第一引数はメソッド名。"REQMOD"か"RESPMOD"。//E+
    //参照が引っこ抜かれるので、 DECREF(pMethodName)禁止
    if (hdrs != NULL) {
        pList = PyList_New(hdrs->used);//HTTPヘッダを入れるトコロ //E+
        for(i = 0; i < hdrs->used; i++) {// && i < PYCI_MAX_HTTP_HEADERS
            pString = PyString_FromString(hdrs->headers[i]);//次に入れるのはHTTPヘッダ //E+
            PyList_SetItem(pList,i,pString);//E+
            //参照が引っこ抜かれるので、 DECREF(pString)禁止
        }
    }
    PyTuple_SetItem(pTuple,1,pList);//第二引数はHTTPヘッダ。リスト型。サイズは変動 //E+

    //参照が引っこ抜かれるので、 DECREF(pList)禁止

    pDict = PyDict_New();//何もしない・・ //E+(NULL)
    //PyDict_SetItemString(pDict, PYCI_CLASS_ARG_MOD_TYPE_NAME, pMethodName);//E+
    PyObject * pInstance = PyInstance_New(pClass, pTuple, pDict);//E+
    //TODO: //E+のところでエラー吐いてたらgotoで一まとめにErr_Printする！
    Py_DECREF(pTuple);
    Py_DECREF(pDict);
    if (PyErr_Occurred()) {
        PyErr_Print();
        pyci_debug_printf(PYCI_ERROR_LEVEL, "Error at creating instance of class "PYCI_CLASS" \n");
        return NULL;
    }
    //TODO: クラスのカタチの決定
    //TODO: 手動ガベコレ
    //クラスのインスタンスの生成
    Py_INCREF(pInstance);//スコープ外れてもガベコレされないようにする。
    return (void *)pInstance;
}
