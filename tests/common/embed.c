#define PY_Sinput2_T_CLEAN
#include <Python.h>
#include <stdbool.h>

int call_leech_int(char *module, char *function, int input1, int input2) {
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

double call_leech_double(char *module, char *function, double input1, double input2) {
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pRet, *pValue;
    double output = 0.0;

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

            pValue = PyFloat_FromDouble(input1);
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert argument\n");
                return output;
            }
            PyTuple_SetItem(pArgs, 0, pValue);
            pValue = PyFloat_FromDouble(input2);
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
                output = PyFloat_AsDouble(pRet);
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

double call_ins_sim(char *module, char *function, double input1, double input2, int ins) {
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pRet, *pValue;
    double output = 0.0;

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
            pArgs = PyTuple_New(3);

            pValue = PyFloat_FromDouble(input1);
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert argument\n");
                return output;
            }
            PyTuple_SetItem(pArgs, 0, pValue);
            pValue = PyFloat_FromDouble(input2);
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert argument\n");
                return output;
            }
            PyTuple_SetItem(pArgs, 1, pValue);
            pValue = PyLong_FromLong(ins);
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert argument\n");
                return output;
            }
            PyTuple_SetItem(pArgs, 2, pValue);
            /* pArgs is the argument list */

			pRet = PyObject_CallObject(pFunc, pArgs);
            /* pRet is the return value from function call */

            Py_DECREF(pArgs);
            if (pRet != NULL) {
                output = PyFloat_AsDouble(pRet);
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

double call_chain_sim(char *module, char *function, double input1, double input2, double input3, double input4) {
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pRet, *pValue;
    double output = 0.0;

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
            pArgs = PyTuple_New(4);

            pValue = PyFloat_FromDouble(input1);
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert argument\n");
                return output;
            }
            PyTuple_SetItem(pArgs, 0, pValue);
            pValue = PyFloat_FromDouble(input2);
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert argument\n");
                return output;
            }
            PyTuple_SetItem(pArgs, 1, pValue);
            pValue = PyFloat_FromDouble(input3);
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert argument\n");
                return output;
            }
            PyTuple_SetItem(pArgs, 2, pValue);
            pValue = PyFloat_FromDouble(input4);
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert argument\n");
                return output;
            }
            PyTuple_SetItem(pArgs, 3, pValue);
            /* pArgs is the argument list */

			pRet = PyObject_CallObject(pFunc, pArgs);
            /* pRet is the return value from function call */

            Py_DECREF(pArgs);
            if (pRet != NULL) {
                output = PyFloat_AsDouble(pRet);
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

int call_func_sim(char *module, char *function, double* input1, double* input2, double* output, int size) {
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pRet, *pValue;
    PyObject *pLin1, *pLin2, *pLout;
    PyObject *pVin1, *pVin2, *pVout;

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
            pArgs = PyTuple_New(3);

			PyObject *l_in1 = PyList_New(size);
			PyObject *l_in2 = PyList_New(size);
			PyObject *l_out = PyList_New(size);
			/* l_* are list objecks */
            
            for(int i=0; i < size; i++){
			    pVin1 = PyFloat_FromDouble(input1[i]);
			    pVin2 = PyFloat_FromDouble(input2[i]);
			    pVout = PyFloat_FromDouble(0.00);
                if (!pVin1 || !pVin2 || !pVout) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return 1;
                }
                /* pValue reference stolen here: */
			    PyList_SetItem(l_in1, i, pVin1);
			    PyList_SetItem(l_in2, i, pVin2);
			    PyList_SetItem(l_out, i, pVout);
                //Py_DECREF(pVin1);
                //Py_DECREF(pVin2);
                //Py_DECREF(pVout);
			}
            /* pV*s take each value of lists and make Py Lists */
			
			pLin1 = Py_BuildValue("(O)", l_in1);
            PyTuple_SetItem(pArgs, 0, pLin1);
			pLin2 = Py_BuildValue("(O)", l_in2);
            PyTuple_SetItem(pArgs, 1, pLin2);
			pLout = Py_BuildValue("(O)", l_out);
            PyTuple_SetItem(pArgs, 2, pLout);
            /* pArgs is the argument list */

			pRet = PyObject_CallObject(pFunc, pArgs);
            /* pRet is the return value from function call */

            Py_DECREF(pArgs);
            //Py_DECREF(pLin1);
            //Py_DECREF(pLin2);
            //Py_DECREF(pLout);
            if (pRet != NULL) {
                for(int i=0; i < size; i++){
                    pValue = PyList_GetItem(pRet, i);
                    output[i] = PyFloat_AsDouble(pValue);
                    Py_DECREF(pValue);
                }
            } else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 1;
            }
            Py_DECREF(pRet);
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
        return 1;
    }

    // Releases the interpreter, but I get an error from this
    //if (Py_FinalizeEx() < 0) {
    //    return 120;
    //}
    
    return 0;
}

/* Library functions */
int rtlib_int(int in1, int in2, int ins) {
    char *module = "overload";
    char *function = "leech_2in";
    int input1 = in1 % 5 + 1;
    int input2 = in2 % 5 + 1;
    //fprintf(stdout, "Selected version is: %d\n", ins);
    int output = call_leech_int(module, function, input1, input2);
    //printf("Output in C: %d\n",output);
    return output;
}

double rtlib_double(double *in1, double *in2, int ins) {
    char *module = "overload";
    char *function = "leech_2in";
    double input1 = *in1;
    double input2 = *in2;
    //fprintf(stdout, "Selected version is: %d\n", ins);
    double output = call_leech_double(module, function, input1, input2);
    //printf("Output in C: %f\n",output);
    return output;
}

void log_val(double i) {
      printf("Log: %f\n", i);
}

// To replace instructions
double ins_replace(double in1, double in2, int ins) {
    char *module = "overload";
    char *function = "call_pe";
    //fprintf(stdout, "Selected version is: %d\n", ins);
    double output = call_ins_sim(module, function, in1, in2, ins);
    //printf("Output in C: %f\n",output);
    return output;
}

// To replace chains
void chain_replace(double in1, double in2, double in3, double in4) {
    char *module = "overload";
    char *function = "call_chain";
    double output = call_chain_sim(module, function, in1, in2, in3, in4);
    //printf("Output in C: %f\n",output);
    //return output;
}

// To replace functions
void func_replace(double* in1, double* in2, double* out) {
    char *module = "overload";
    char *function = "call_func";
    int ret = call_func_sim(module, function, in1, in2, out, 4096);
    if (ret > 0)
        fprintf(stderr, "Python function embedding returns error: %d!\n",ret);
}
