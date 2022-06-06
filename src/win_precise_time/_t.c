/* SPDX-FileCopyrightText: 2022-present Artur Drogunow <artur.drogunow@zf.com>
#
# SPDX-License-Identifier: MIT */

#ifndef Py_LIMITED_API
    #define Py_LIMITED_API
#endif
#include "_t.h"
#include <Windows.h>


static VOID (CALLBACK *wpt_GetSystemTimePreciseAsFileTime)(LPFILETIME) = NULL;


static PyObject *precise_time(PyObject *self, PyObject *args)
{
    FILETIME file_time;
    ULARGE_INTEGER large;
    ULONGLONG offset = 11644473600000000000;

    wpt_GetSystemTimePreciseAsFileTime(&file_time);
    large.LowPart = file_time.dwLowDateTime;
    large.HighPart = file_time.dwHighDateTime;

    return PyFloat_FromDouble((double)(large.QuadPart * 100 - offset) * 1e-9);
}


static struct PyMethodDef methods[] = {
    {
        "time",
        precise_time,
        METH_NOARGS,
        "Retrieve the current system date and time with the highest possible level of precision (<1us)."
    },
    { NULL }  // sentinel
};

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "win_precise_time._t",
    .m_doc = "",
    .m_size = -1,
    .m_methods = methods
};

PyMODINIT_FUNC PyInit__t(void)
{
    HINSTANCE hKernel32;

    hKernel32 = GetModuleHandleW(L"KERNEL32");
    /* Function available on Windows 8, Windows Server 2012 and newer */
    *(FARPROC*)&wpt_GetSystemTimePreciseAsFileTime = GetProcAddress(hKernel32, "GetSystemTimePreciseAsFileTime");

     /* ensure that the system clock works */
     if (wpt_GetSystemTimePreciseAsFileTime == NULL){
         return (NULL);
     }


    PyObject *module_p;
    module_p = PyModule_Create(&module);

    if (module_p == NULL) {
        return (NULL);
    }

    return (module_p);
}
