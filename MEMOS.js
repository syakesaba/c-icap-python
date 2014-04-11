
Py_NewInterpreter={"http://stackoverflow.com/questions/1480490/python-interpreter-as-a-c-class",
                   "http://docs.python.org/2/c-api/init.html#Py_NewInterpreter"}
Pthread_and_Python={"http://www.ioremap.net/node/538/"}

running = 0;

function c-icap() {
 init_hander.c();
 Py_initialize()
 //PyEval_initThreads()

 load_module.c();//Gain each ICAP script information(LIKE ISTAG)s in main thread. 
 //mainInterpreter=PyEval_SaveThread();
 //PyEval_AcquireLock()
 //struct_t->state=Py_NewInterpreter()
 //PyEval_ReleaseThread(struct_t->state)
 //PyEval_ReleaseLock();

//PyEval_AcquireLock()
//PyEval_AcquireThread(struct_t->state)
 init_service.c();
//PyEval_ReleaseThread(struct_t->state)
 post_init_handler.c();
// Check if initialization is done. *mainInterpreter
//PyEval_AcquireLock()
//PyEval_AcquireThread(struct_t->state)
 post_init_service.c();
//PyEval_ReleaseThread(struct_t->state)
// Check if python no error. *subInterpreter
 running = 1;
//*mainInterpreter
 while (running != 0 && noError) { // *subInterpreter
  wait_and_handle(icap_request) { // *subInterpreter
   init_requet_data.c();// *subInterpreter
   check_preview_hadler.c(); // *subInterpreter
   service_io.c(); // *subInterpreter
   end_of_data_handler.c(); // *subInterpreter
   release_request_data.c(); // *subInterpreter
 } // *subInterpreter
 close_service.c(); // release subInterpreter *subInterpreter
 release_handler.c(); // release all Interpreter *mainInterpreter
}

concerns = {"c-icap runs on multiple threads and processes.is Python c-api work in multi process?"}
    PyEval_AcquireThread(myThreadState); // get GIL + swap in thread state


    // call python code
    PyEval_ReleaseThread(myThreadState);
    // any other code
    // finish with interpreter
    PyEval_AcquireThread(myThreadState);
    PyRun_SimpleString("A=2;");
    Py_EndInterpreter(myThreadState);
    PyEval_ReleaseLock();                // release the GIL


    PyEval_RestoreThread(mainThreadState);
    PyRun_SimpleString("print A;");
    Py_Finalize();

