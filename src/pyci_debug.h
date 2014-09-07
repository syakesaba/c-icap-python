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

#ifndef PYCI_DEBUG_H_
#define PYCI_DEBUG_H_

#include "c_icap/debug.h"

#ifndef DEBUG
#define DEBUG 1
#endif

#define PYCI_ERROR_LEVEL 1
#define PYCI_WARN_LEVEL 3
#define PYCI_MESSAGE_LEVEL 5
#define PYCI_INFO_LEVEL 7
#define PYCI_DEBUG_LEVEL 9

#ifdef DEBUG
#define pyci_debug_printf(lev,msg, ...)  \
        ci_debug_printf(1, "(%d):%s():L%04d: "msg"\n", getpid(), __func__, __LINE__, ## __VA_ARGS__)
#endif

#ifndef DEBUG
#define pyci_debug_printf(lev,msg, ...)  \
        ci_debug_printf(lev, msg"\n", ## __VA_ARGS__)
#endif

#endif /* PYCI_DEBUG_H_ */
