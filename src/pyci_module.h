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
 * このファイルの役割は、CI_DECLARE_DATAにより、
 * C-ICAPに識別子を提供することです。
 */

#ifndef PYCI_MODULE_H_
#define PYCI_MODULE_H_

#include <Python.h>
#include "c_icap/c-icap.h"
#include "c_icap/module.h"

#include "./pyci_module/init_handler.h"
#include "./pyci_module/post_init_handler.h"
#include "./pyci_module/load_module.h"
#include "./pyci_module/release_handler.h"

CI_DECLARE_DATA service_handler_module_t module;
extern PyObject * pClass; /* module.cでのみ実体化されます。 */

#endif /* PYCI_MODULE_H_ */
