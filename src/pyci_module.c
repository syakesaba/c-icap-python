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
 * このクラスの役割は、モジュールデータの実体化です。
 */

#include <Python.h>
#include "c_icap/module.h"

#include "pyci_module.h"

CI_DECLARE_DATA extern service_handler_module_t module = {
	"python_handler",		  /* 名前 */
	".py",					  /* 拡張子 */
	init_python_handler,	  /* スクリプトハンドラ初期化処理 */
	post_init_python_handler, /* スクリプトハンドラ初期化後処理 */
	release_python_handler,	  /* スクリプトハンドラの解放処理 */
	load_python_module,		  /* 各スクリプトをICAPサービスとして登録する処理 */
	NULL					  /* TODO: conf_table */
};

extern PyObject *pClass = NULL;