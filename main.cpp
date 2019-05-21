#include <iostream>
#include <Python.h>
#include "ScopeGuard.hpp"

PyObject* getInstance(
    char const* modName,
    char const* className,
    int argc,
    char* argv[]
) {
    PyObject *pName, *pModule, *pClass, *pNew, *pInit, *pObj;
    PyObject *pArgs, *pValue(NULL);
    int i;

    pName = PyString_FromString(modName);
    /* Error checking of pName left out */

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if(pModule == NULL) {
        if (PyErr_Occurred()) PyErr_Print();
        fprintf(stderr, "failed to load module \"%s\"\n", modName);
        return NULL;
    }
    SCOPE_EXIT{ Py_DECREF(pModule); };

    pClass = PyObject_GetAttrString(pModule, className);
    if(pClass == NULL) {
        if (PyErr_Occurred()) PyErr_Print();
        fprintf(stderr, "failed to load class \"%s\"\n", className);
        return NULL;
    }
    SCOPE_EXIT{ Py_DECREF(pClass); };

    if (PyCallable_Check(pClass)) {
        pArgs = PyTuple_New(argc - 3);
        SCOPE_EXIT{ Py_XDECREF(pArgs); };
        for (i = 0; i < argc - 3; ++i) {
            pValue = PyInt_FromLong(atoi(argv[i + 3]));
            if (!pValue) {
                fprintf(stderr, "Cannot convert argument\n");
                return NULL;
            }
            /* pValue reference stolen here: */
            PyTuple_SetItem(pArgs, i, pValue);
        }

        pObj = PyObject_Call(pClass, pArgs, NULL);
        Py_INCREF(pObj);
    } else {
        if (PyErr_Occurred()) PyErr_Print();
        fprintf(stderr, "constructor not callable???");
        return NULL;
    }

    return pObj;
}



int main(int argc, char *argv[]) {
    PyObject *pInstance, *pFunc, *pValue;

    Py_Initialize();
PyEval_InitThreads();

    if (argc < 3) {
        fprintf(stderr,"Usage: call pythonfile funcname [args]\n");
        return 1;
    }

    pInstance = getInstance(argv[1], argv[2], argc, argv);
    if(pInstance == NULL) {
        if (PyErr_Occurred()) PyErr_Print();
        fprintf(stderr, "failed to get instance");
        return 1;
    }
    SCOPE_EXIT{ Py_XDECREF(pInstance); };

    pFunc = PyObject_GetAttrString(pInstance, "do");
    if(pFunc == NULL) {
        if (PyErr_Occurred()) PyErr_Print();
        fprintf(stderr, "failed to get do");
        return 1;
    }
    SCOPE_EXIT{ Py_XDECREF(pFunc); };

    pValue = PyObject_CallObject(pFunc, NULL);
    if(pValue == NULL) {
        if (PyErr_Occurred()) PyErr_Print();
        fprintf(stderr, "failed to call do");
        return 1;
    } else {
        std::cout << PyInt_AsLong(pValue) << std::endl;
    }
    SCOPE_EXIT{ Py_XDECREF(pValue); };

    Py_Finalize();
    return 0;
}
