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
 * このファイルの役割は、./pyci_service/が提供するすべての関数を
 * インクルードするためのものです。
 * init_serviceでつかいます。
 */

#ifndef PYCI_SERVICE_H_
#define PYCI_SERVICE_H_

#include "pyci_service/init_service.h"
#include "pyci_service/post_init_service.h"
#include "pyci_service/close_service.h"
#include "pyci_service/init_request_data.h"
#include "pyci_service/check_preview_handler.h"
#include "pyci_service/service_io.h"
#include "pyci_service/end_of_data_handler.h"
#include "pyci_service/release_request_data.h"
#include "pyci_service/utils/pyci_service_util.h"

#endif /* PYCI_SERVICE_H_ */
