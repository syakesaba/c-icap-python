#!/usr/local/env python
# encoding: utf-8

CI_NO_STATUS=0
CI_OK=1
CI_NEEDS_MORE=2
CI_ERROR=-1
CI_EOF=-2

CI_MOD_NOT_READY=0
CI_MOD_DONE=1
CI_MOD_CONTINUE=100
CI_MOD_ALLOW204=204
CI_MOD_ALLOW206=206
CI_MOD_ERROR=-1

#PYCI_CLASS="ICAP"
#PYCI_CLASS_LIST_SUPPORTED_METHODS="SUPPORTED_METHODS"
#PYCI_CLASS_STRING_SERVICE_NAME="NAME"
#PYCI_CLASS_BOOL_LOCK_DATA="lock_data"
#PYCI_CLASS_METHOD_HANDLE_PREVIEW_DATA="handle_preview_data"
#PYCI_CLASS_METHOD_HANDLE_END_OF_DATA="handle_end_of_data"
#PYCI_CLASS_METHOD_HANDLE_READ_DATA="handle_read_data"
#PYCI_CLASS_METHOD_HANDLE_WRITE_DATA="handle_write_data"

#DONT DO 
#class ICAP(object)

class ICAP:
    NAME = "CI-PYTHON"
    DESCRIPTION = "CI-PYTHON SCRIPT"
    SUPPORTED_METHODS = ["REQMOD", "RESPMOD"]
    ISTAG = "-PYTHON" # 他のとかぶらないでね

    def __init__(self, method, headers):
        """
        Args:
            method: the method of modification
            headers: HTTP HEADERS! not ICAP HEADER.

        基本的にSUPPORTED_METHODSで指定したメソッドしか
        呼び出されることはありません。
        """
        self.method = method
        self.headers = headers
        self.body = io.BytesIO()
        self.lock_data = True
        if headers:
            print
            for header in headers:
                self.debug_me("[HEADER]:" + header)

    def debug_me(self,msg):
        print "%s:%s[Python] %s" % (ICAP.NAME, self.method, msg) 

    def handle_preview_data(self, preview_data):
        """
        Args:
            preview_data: preview_data
        """
        self.handle_read_data(preview_data);
        #preview_data always the top of the body.
        #self.body += preview_data
        #will work too.
        return CI_MOD_CONTINUE
        #return CI_MOD_ALLOW204
        #return CI_MOD_ALLOW206

    def handle_read_data(self, read_data):
        """
        Args:
            read_data: the data. str.
        """
        self.body.write(read_data)
        return CI_OK

    def handle_write_data(self, write_len):
        """
        Args:
            write_len: Max length of the data can return.
        """
        #ret = self.body[:write_len]
        #self.body = self.body[write_len:]
        ret = b""
        if self.body.readable():
            ret += self.body.read(write_len)
        return ret
    def handle_end_of_data(self):
        #print
        #print "=========================="
        self.headers.append("XUNKO: unko")
        #print "=========================="
        return CI_MOD_DONE

print "Thank you! You Executed python script!"
