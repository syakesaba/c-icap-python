Please make another module
=============
__A python wrapper for c-icap is strongly needed!__  
Like [mod\_wsgi](http://code.google.com/p/modwsgi/), we have to change interpreters for each ICAP Requests.  
Unfortunally I have no sense of programming. I hope someone else make another python wrapper for c-icap.

---

c-icap-python
=============

[c-icap](http://c-icap.sourceforge.net/) module for python
 
A python port of c-icap-ruby module by Roman Shterenzon. 

***NOT WORK YET!!!***
(works only icap.py)

Install
=============
1. make libc-icap-python.so
2. copy and rename it to /path/to/lib/c_icap/srv_python.so and edit /path/to/lib/c_icap/srv_python.la 
3. add line "Module service_handler srv_python.so" into the c-icap.conf
4. add line "Service ServiceName /path/to/script.py" into the c-icap.conf
5. start c-icap

Depends
=============
- c-icap(include files,libraries)
- libpython (Python.h)

License
=============
GPLv3

See ***COPYING*** for detail.

Author
=============
SYA-KE 
 
Thanks to Roman.
