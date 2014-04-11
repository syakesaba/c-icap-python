TODO
=============
1. Make many python scripts in one c-icap server (this software still in one global interpreter)
Using Py_Initialize() in the library,all "ICAP" class in each scripts will be overriden by others. 
To solve the probrem: 
2. HTTP1.1(gzip,deflare), SPDY(?) support

TOSOLVE
=============
* Using '''PyThreadState* Py_NewInterpreter()'''
* Share the global interpreter with each scripts, replacing "ICAP" class name to unique one(like "FILENAME" class)
* Use many c-icap server -> :D
* zlib?

1 is hard. 
2 is easy and we can refer an object between the other ICAP services. 
But this will allow people writing ICAP service in Python pick up the others data and streal them. 
3 is funny. 


