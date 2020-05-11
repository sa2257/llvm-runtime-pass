#define PY_Sinput2_T_CLEAN
#include <Python.h>
#include <stdbool.h>

int call_leech(char *module, char *function, int input1, int input2) {
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pRet, *pValue;
    int output = 0;

    Py_Initialize();
    /* These two lines allow looking up for modules in the current directory */
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");

    pName = PyUnicode_DecodeFSDefault(module);
    /* Error checking of pName left out */

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, function);
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(2);

            pValue = PyLong_FromLong(input1);
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert argument\n");
                return output;
            }
            PyTuple_SetItem(pArgs, 0, pValue);
            pValue = PyLong_FromLong(input2);
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert argument\n");
                return output;
            }
            PyTuple_SetItem(pArgs, 1, pValue);
            /* pArgs is the argument list */

			pRet = PyObject_CallObject(pFunc, pArgs);
            /* pRet is the return value from function call */

            Py_DECREF(pArgs);
            if (pRet != NULL) {
                output = PyLong_AsLong(pRet);
                Py_DECREF(pRet);
            } else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return output;
            }
        } else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", function);
        }
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", module);
        return output;
    }

    if (Py_FinalizeEx() < 0) {
        return output;
    }

    return output;
}

int rtlib(int in1, int in2, int ins) {
    char *module = "overload";
    char *function = "leech_2in";
    int input1 = in1 % 5 + 1;
    int input2 = in2 % 5 + 1;
    fprintf(stdout, "Selected version is: %d", ins);
    int output = call_leech(module, function, input1, input2);
    //printf("Output in C: %d\n",output);
    return output;
}
