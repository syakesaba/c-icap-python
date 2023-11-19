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

#ifndef PYCI_SCRIPT_H_
#define PYCI_SCRIPT_H_

#define PYCI_CLASS "ICAP" /* 必須。type。CLASS */
#define PYCI_CLASS_LIST_SUPPORTED_METHODS "SUPPORTED_METHODS" /* 対応メソッド[リスト]。必須。CLASS.SUPPORTED_METHOD */
#define PYCI_CLASS_STRING_SERVICE_NAME "NAME" /* 必須 str。CLASS.name */
#define PYCI_CLASS_STRING_SERVICE_DESCRIPTION "DESCRIPTION" /* 必須 str。CLASS.DESCRIPTION */
#define PYCI_CLASS_STRING_SERVICE_ISTAG "ISTAG" /* 必須？ INSTANCE.istag */
#define PYCI_CLASS_BOOL_LOCK_DATA "lock_data" /* 必須 bool。INSTANCE.lock */
#define PYCI_CLASS_LIST_HEADERS "headers" /* 必須? list。INSTANCE.headers */
#define PYCI_CLASS_METHOD_HANDLE_PREVIEW_DATA "handle_preview_data" /* 必須 INSTANCE.preview */
#define PYCI_CLASS_METHOD_HANDLE_END_OF_DATA "handle_end_of_data" /* 必須 INSTANCE.handle_end_of_data */
#define PYCI_CLASS_METHOD_HANDLE_READ_DATA "handle_read_data" /* 必須 INSTANCE.handle_io_read */
#define PYCI_CLASS_METHOD_HANDLE_WRITE_DATA "handle_write_data" /* 必須 INSTANCE.io_write */

#define PYTHONPATH_LOCATION_IN_SYS "path" //sys.path = PYTHONPATH

#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif

#endif /* PYCI_SCRIPT_H_ */
