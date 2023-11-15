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
#include "c_icap/request.h"

#include "pyci_module.h"
#include "pyci_debug.h"
#include "pyci_script.h"

/*
 * ================================================================
 * Prev is <- ../pyci_module/load_module.c
 * Next is -> ../pyci_module/post_init_handler.c
 * ================================================================
 */

/*
 * ICAPが起動した直後一度だけ動作する関数
 * ここではスクリプトが示したsrv_xdataに関係する情報をSquidに送りたい。
 * return CI_OK on success, CI_ERROR on any error
 */
int python_init_service(ci_service_xdata_t *srv_xdata, struct ci_server_conf *server_conf) {
    pyci_debug_printf(PYCI_INFO_LEVEL,"starts");

    PyObject * pIStag;

    /* ===================================================
     *  Pythonスクリプトから、ISTAGを取得する処理（ないならスルー）
     *    TODO: ユーザが大量の文字数を指定してきたとき、AsStringが危険。クラウド公開できない。
     * ===================================================
     */
    char * istag;
    if (! PyObject_HasAttrString(pClass, PYCI_CLASS_STRING_SERVICE_ISTAG)) {
        goto istag_ignore;//E+ N
    }
    pIStag = PyObject_GetAttrString(pClass, PYCI_CLASS_STRING_SERVICE_ISTAG);
    if (pIStag != NULL) {
        if (PyString_Check(pIStag)) {
            istag = PyString_AsString(pIStag);//dont dealloc
            if (istag == NULL) {
                pyci_debug_printf(PYCI_MESSAGE_LEVEL, "ISTAG is not (str). ignored");
                goto istag_ignore;
            }
            if (strnlen(istag,ISTAG_SIZE) >= ISTAG_SIZE) { //なぜ>にしないかというと、最後のNULLが入らないから
                pyci_debug_printf(PYCI_ERROR_LEVEL, "Error ISTAG over %d Bytes", ISTAG_SIZE-1);
                istag = NULL;
                goto istag_ignore;
            }
            char * cp_istag = strndup(istag,ISTAG_SIZE);
            if (cp_istag == NULL) {
                pyci_debug_printf(PYCI_ERROR_LEVEL, "Error allocating memory %d Bytes for ISTAG copy", ISTAG_SIZE);
                istag = NULL;
            } else {
                cp_istag[ISTAG_SIZE-1] = '\0';
                //サービスのISTAGという特殊な識別子。文字列。
                ci_service_set_istag(srv_xdata, cp_istag);
                pyci_debug_printf(PYCI_MESSAGE_LEVEL, "ISTAG is set %s", cp_istag);
                free(cp_istag);
            }
        }
    }
istag_ignore:
if (PyErr_Occurred()) {
        PyErr_Print();
        return CI_ERROR;
    }
    Py_XDECREF(pIStag);

    //TODO: 全部送っちゃったら多少データ量が多くなる。必要に応じてadd_xoptsを追加する。
    //色々な識別子
    ci_service_set_xopts(srv_xdata,
            CI_XCLIENTIP | CI_XSERVERIP | CI_XSUBSCRIBERID|
            CI_XAUTHENTICATEDUSER | CI_XAUTHENTICATEDGROUPS);

    //TODO: 全部（略
    //ICAPに送ってきてもいいけど、previewするわ、っていう拡張子
    ci_service_set_transfer_preview(srv_xdata, "*");
    //ci_service_set_transfer_preview(srv_xdata,"html,js,php,cgi,");

    //TODO: 全部（略
    //ICAPに送ってこないで　っていう拡張子
    ci_service_set_transfer_ignore(srv_xdata,"");
    //ci_service_set_transfer_ignore(srv_xdata,"zip, tar");

    //TODO: 全（略
    //previewせず、全部もらう拡張子
    ci_service_set_transfer_complete(srv_xdata,"");

    //TODO: スクリプトの平均実行時間によってTTLを設定し、タイムアウトを実装する
    ci_service_set_options_ttl(srv_xdata, 3600);

    //TODO: スクリプトからとってくる
    //自分のXINCLUDEを追加できる！イイネ！！
    ci_service_add_xincludes(srv_xdata, NULL);

    /* previwサイズ。そのまんま。*/
    /* 1024という数字は、どうなのだろう */
    ci_service_set_preview(srv_xdata, 1024);

    /*Tell to the icap clients that we support 204 responses*/
    /* そのまんま */
    ci_service_enable_204(srv_xdata);

    /*Tell to the icap clients that we support 204 responses*/
    /* そのまんま */
    ci_service_enable_206(srv_xdata);

    return CI_OK;
}
