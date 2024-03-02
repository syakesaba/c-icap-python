#include <Python.h>
#include <c_icap/service.h>

int main(void) {
    if (!Py_IsInitialized()) {
    	Py_Initialize();
    }
    
    printf("%d", CI_MOD_ERROR);
    return 0;
}
