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
 * スクリプトの数だけ複数回よばれる
 * ================================================================
 * Prev is -> init_handler.c
 * Next is -> pyci_service/init_service.c
 * ================================================================
 */

#include <Python.h>
#include <libgen.h>
#include <c_icap/c-icap.h>
#include <c_icap/service.h>

#include "pyci_module.h"
#include "pyci_service.h"
#include "pyci_script.h"
#include "pyci_debug.h"

int append_python_path(const char *service_file);         // if ret != 0, ERROR.
PyObject *import_python_module(const char *service_file); // if NULL, ERROR.

/*
 * TODO: 一つのpythonファイルmoduleにつき一つの
 * PyState * pState = Py_NewInterpreter();
 * を呼び出し、C言語側でstateを管理する。
 * =>どうやって？
 */

/*
 * typedef struct pyci_vars {
 *     PyThreadState * interpreter;　とでもするか？
 *     PyObject * pClass;
 * }pyci_vars_t
 *     Py_Initialize();
 *  PyEval_InitThreads();
 *  PyThreadState * mainThreadState = PyEval_SaveThread();
 *  PyEval_AcquireLock();                // get the GIL
    PyThreadState * myThreadState = Py_NewInterpreter();

    PyRun_SimpleString("A=1;");

    PyEval_ReleaseThread(myThreadState); // swap out thread state + release the GIL
 *
 * んでload_python_module関数でサブインタプリタ生成する？
 * んでクラス読みこんだらすぐにsaveして開放？
 * んでserviceの時は
 */

/*
 * service_fileは残念ながらフルパスです。
 */
ci_service_module_t *load_python_module(const char *service_file, const char *argv[])
{
    pyci_debug_printf(PYCI_INFO_LEVEL, "starts");
    pyci_debug_printf(PYCI_INFO_LEVEL, "loading %s", service_file);

    PyObject *pModule = NULL;
    PyObject *pObj = NULL;
    PyObject *pList = NULL;
    PyObject *pService_Name = NULL;
    PyObject *pService_Descr = NULL;

    ci_service_module_t *service = NULL; /* こいつはpythonスクリプト一個につき一個だけ生成される */

    /* ===================================================
     *  このファイルの核。サービス構造体の作成処理
     * ===================================================
     */
    service = (ci_service_module_t *)malloc(sizeof(ci_service_module_t));
    if (service == NULL)
    {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at Allocating memory for ci_service_module_t of %s", service_file);
        return NULL;
    }

    /* ===================================================
     *  あらゆるフルパスであってもPythonのパスに突っ込む処理
     * ===================================================
     */
    if (append_python_path(service_file) != EXIT_SUCCESS)
    {
        goto invalid_path_error;
    }

    /* ===================================================
     *  あらゆるフルパスであってもPythonファイルをimportする処理
     * ===================================================
     */
    pModule = import_python_module(service_file);
    if (pModule == NULL)
    {
        goto cant_import_error;
    }

    /* ===================================================
     *  クラスをPythonファイルから新規参照する処理
     * ===================================================
     */
    pObj = PyObject_GetAttrString(pModule, PYCI_CLASS);
    if (pObj == NULL)
    {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "Error at PyObject_GetAttrString(pModule, " PYCI_CLASS ")");
        goto no_class_error;
    }

    /* "ICAP"という名前のクラスをpythonファイルから持ってくる */
    if (PyCallable_Check(pObj))
    {
        /* pythonではClassをインスタンスにするときも関数を呼び出すときも、Callをする。*/
        Py_INCREF(pModule); /* pClassオブジェクトはpModuleの下にくっついているので、親のpModuleが解放されないようにする */
        pClass = pObj;
        /* グローバル空間にこの雛形を格納!!!!!!!!!!!!!!!!!!!!!!!!!!ここ重要 */
    }
    else
    {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at PyCallable_Check(pObj) : class '" PYCI_CLASS "' is not Callable");
        goto cant_call_class_error;
    }

    // ここまでがPythonファイルからクラスをとってきてグローバル空間に落とし込む処理
    // ここからがserviceハンドラを保持する構造体の作成
    // この時点ではpModule(2),pClass(1)という参照数

    /* ===================================================
     *  Pythonスクリプトから、RESPMOD,REQMODの対応状況を見る処理
     * ===================================================
     */
    int mode = 0;
    pList = PyObject_GetAttrString(pClass, PYCI_CLASS_LIST_SUPPORTED_METHODS); // E+
    if (pList != NULL && PyList_Check(pList))
    {
        int sizeof_pList = -1;
        sizeof_pList = PyList_Size(pList); // エラー放出するだろうけど、Python公式ドキュメントに書いてないw
        if (sizeof_pList == -1)
        {
            goto pList_size_error;
        }
        char *item = NULL;
        for (int i = 0; i < sizeof_pList; i++)
        {
            PyObject *pItem = PyList_GetItem(pList, i); // 借り参照。DECREF禁止。//E+
            if (pItem != NULL && PyString_Check(pItem))
            {
                // int error = PyString_AsStringAndSize(pItem, item, max);
                // TODO: ユーザが大量の文字数を指定してきやがった場合↑。本当はこうすべきだが
                item = PyString_AsString(pItem); // dont dealloc
                if (item == NULL)
                {
                    goto item_not_string_error;
                }
                //                ci_debug_printf(1,"D0 ret -> %d\n", ret);
                if (item != NULL)
                {
                    // strncmpではない理由は、ヌルキャラの存在により比較失敗するのを恐れたため。
                    // ここもstrnstrにすべきだが・・・倍返しだ！
                    if (strstr(item, ci_method_string(ICAP_REQMOD)))
                    {
                        mode = mode | ICAP_REQMOD;
                        pyci_debug_printf(PYCI_MESSAGE_LEVEL, "(%s) : REQMOD enabled", service_file);
                    }
                    if (strstr(item, ci_method_string(ICAP_RESPMOD)))
                    {
                        mode = mode | ICAP_RESPMOD;
                        pyci_debug_printf(PYCI_MESSAGE_LEVEL, "(%s) : RESPMOD enabled", service_file);
                    }
                }
            }
            else
            {
                pyci_debug_printf(PYCI_ERROR_LEVEL,
                                  "(%s) invalid value in " PYCI_CLASS "." PYCI_CLASS_LIST_SUPPORTED_METHODS "[%d]",
                                  service_file, i);
                goto invalid_value_in_supported_list_error;
            }
        }
    }
    if (mode == 0)
    {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at mode selection for %s : see " PYCI_CLASS "." PYCI_CLASS_LIST_SUPPORTED_METHODS, service_file);
        goto no_mode_declared_error;
    }

    /* ===================================================
     *  Pythonスクリプトから、サービス名を取得する処理
     * ===================================================
     */
    char *service_name = NULL;
    pService_Name = PyObject_GetAttrString(pClass, PYCI_CLASS_STRING_SERVICE_NAME); // E+ N
    if (pService_Name != NULL && PyString_Check(pService_Name))
    {
        // int error = PyString_AsStringAndSize
        // TODO: ユーザが大量の文字数を指定してきやがった場合↑。本当はこうすべきだが
        service_name = PyString_AsString(pService_Name); // dont dealloc
        if (service_name == NULL)
        {
            goto service_name_not_string_error;
        }
        pyci_debug_printf(PYCI_MESSAGE_LEVEL, "(%s) : SERVICE_NAME is %s", service_file, service_name);
    }
    else
    {
        goto invalid_service_name_error;
    }

    /* ===================================================
     *  Pythonスクリプトから、サービスの概要を取得する処理
     *  (サービス名よりもサービスの概要が優先されて表示されるので注意）
     * ===================================================
     */
    char *service_descr = NULL;
    pService_Descr = PyObject_GetAttrString(pClass, PYCI_CLASS_STRING_SERVICE_DESCRIPTION); // E+ N
    if (pService_Descr != NULL && PyString_Check(pService_Descr))
    {
        // int error = PyString_AsStringAndSize
        // TODO: ユーザが大量の文字数を指定してきやがった場合↑。本当はこうすべきだが
        service_descr = PyString_AsString(pService_Descr); // dont dealloc
        if (service_descr == NULL)
        {
            goto service_descr_not_string_error;
        }
        pyci_debug_printf(PYCI_MESSAGE_LEVEL, "(%s) : SERVICE_DESCRIPTION is %s", service_file, service_descr);
    }
    else
    {
        goto invalid_service_descr_error;
    }

    /* ===================================================
     *  service構造体の完成
     * ===================================================
     */
    service->mod_data = NULL;       // 常にNULL
    service->mod_conf_table = NULL; // c-icap.confのデータが入ったci_conf_entry構造体が入るらしい。設定はいじりません
    service->mod_init_service = python_init_service;
    service->mod_post_init_service = python_post_init_service;
    service->mod_close_service = python_close_service;
    service->mod_init_request_data = python_init_request_data;
    service->mod_release_request_data = python_release_request_data;
    service->mod_check_preview_handler = python_check_preview_handler;
    service->mod_end_of_data_handler = python_end_of_data_handler;
    service->mod_service_io = python_service_io;
    service->mod_name = service_name;
    service->mod_short_descr = service_descr;
    service->mod_type = mode;

    pyci_debug_printf(PYCI_MESSAGE_LEVEL,
                      "(%s) : OK! service loaded", service_file);
    return service; // サービスを返す

/* ===================================================
 *  一度でもエラーが起きたらここへ飛び、PyObject*の後始末、PyErrの表示をさせる
 * ===================================================
 */
invalid_path_error:
cant_import_error:
no_class_error:
cant_call_class_error:
pList_size_error:
invalid_value_in_supported_list_error:
item_not_string_error:
no_mode_declared_error:
service_name_not_string_error:
invalid_service_name_error:
service_descr_not_string_error:
invalid_service_descr_error:

    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    Py_XDECREF(pService_Descr);
    Py_XDECREF(pService_Name);
    Py_XDECREF(pList);
    Py_XDECREF(pClass);
    Py_XDECREF(pModule);
    free(service);
    return NULL;
}

#define PYLIST_APPEND_SUCCESS 0
#define PYSYS_SET_OBJECT_SUCCESS 0

int append_python_path(const char *service_file)
{
    int ret = 0;
    char *cp_service_file = NULL;
    const char *dir_name = NULL;

    cp_service_file = strndup(service_file, strlen(service_file)); // returns copied-allocated one.
    dir_name = dirname(cp_service_file);                           // returns pointer. not allocted one.
    PyObject *pName = PyString_FromString(dir_name);               // Don't DECREF(pName)
    free(cp_service_file);
    if (pName == NULL)
    {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "Error in PyString_FromString() of %s", service_file);
        return -1;
    }
    PyObject *pPath = PySys_GetObject(PYTHONPATH_LOCATION_IN_SYS); // NULLきてもErr無し！
    if (pPath == NULL)
    {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "Error in PySys_GetObject(" PYTHONPATH_LOCATION_IN_SYS ") of %s", service_file);
        return -1;
    }
    ret = PyList_Append(pPath, pName); // E+
    if (ret != PYLIST_APPEND_SUCCESS)
    {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "Error in PyList_Append(pPath,pName) of %s", service_file);
        return ret;
    }
    ret = PySys_SetObject(PYTHONPATH_LOCATION_IN_SYS, pPath);
    if (ret != PYSYS_SET_OBJECT_SUCCESS)
    {
        return ret;
    }
    return ret;
}

#undef VAR_PATH_NAME
#undef PYLIST_APPEND_SUCCESS

/*
 * on Error = NULL (python error may raise)
 * on Success = pModule
 */
PyObject *import_python_module(const char *service_file)
{
    char *cp_service_file = strndup(service_file, strlen(service_file));
    if (cp_service_file == NULL)
    {
        pyci_debug_printf(PYCI_ERROR_LEVEL, "Could not allocating memory for %s", service_file);
        return NULL;
    }
    char *service_file_name = (char *)basename(cp_service_file);
    service_file_name[strlen(service_file_name) - 3] = '\0';      // -3 = strlen(".py") equal to basename $1 .py
    PyObject *pModule = PyImport_ImportModule(service_file_name); // NewRef,E+
    free(cp_service_file);                                        // service_file_nameもfreeされます。たぶん。
    /* import文を使い、pythonファイルをモジュールとしてインポートする */
    if (pModule == NULL)
    {
        /* pythonでは、モジュールのインポートでもエラーが起こる。*/
        pyci_debug_printf(PYCI_ERROR_LEVEL, "FAIL at PyImport_Import(%s)", service_file_name);
        return NULL;
    }
    return pModule;
}
