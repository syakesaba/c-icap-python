/*
 * util.c
 *
 *  Created on: 2013/09/01
 *      Author: user
 */

#include <Python.h>
#include <c_icap/request.h>
#include <c_icap/simple_api.h>

#include "pyci_service.h"
#include "pyci_debug.h"
#include "pyci_script.h"

/* Replaces the original headers with the headers returned by the Python code */
// TODO: 全く同じheaderを持っている場合はこの関数を実行しない
int replace_headers(ci_request_t *req)
{
	const int REQ_TYPE = ci_req_type(req);
	const char *(*addHeader)(ci_request_t *, const char *) = NULL;
	// RESPMODとREQMODの違いによって、関数ポインタを変えることで、同じ処理を２度書かなくてすむ。
	if (REQ_TYPE == ICAP_REQMOD)
	{
		ci_http_request_reset_headers(req); // TODO: Error management
		addHeader = ci_http_request_add_header;
	}
	else if (REQ_TYPE == ICAP_RESPMOD)
	{
		ci_http_response_reset_headers(req); // TODO: Error management
		addHeader = ci_http_response_add_header;
	} else {
		// Not REQMOD and RESPMOD
		pyci_debug_printf(PYCI_ERROR_LEVEL, "Request type is not REQMOD or RESPMOD");
		return CI_SERVICE_ERROR;
	}

	PyObject *pInstance = (PyObject *)ci_service_data(req); // Don't decref
	PyObject *pList = PyObject_GetAttrString(pInstance, PYCI_CLASS_LIST_HEADERS); // DECREF ME

	if (pList != NULL && PyList_Check(pList))
	{
		PyObject *pHeader = NULL;
		char *header = NULL;
		int pList_size = 0;
		pList_size = PyList_Size(pList); // E+ TODO: size check
		for (int i = 0; i < pList_size; i++)
		{
			pHeader = PyList_GetItem(pList, i); // E+ Don't DECREF
			if (pHeader && PyString_Check(pHeader))
			{
				header = PyString_AsString(pHeader); // E+ Don't free me. TODO: size check
				addHeader(req, header);				 // TODO: Error check
			}
			else
			{
				pyci_debug_printf(PYCI_MESSAGE_LEVEL, "http headers replacing must be string. ignoring...");
			}
		}
	}
	Py_XDECREF(pList);
	return 0;
}
