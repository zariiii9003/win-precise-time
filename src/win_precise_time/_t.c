/* SPDX-FileCopyrightText: 2022-present Artur Drogunow <artur.drogunow@zf.com>
#
# SPDX-License-Identifier: MIT */

#ifndef Py_LIMITED_API
    #define Py_LIMITED_API
#endif
#include "_t.h"
#include <Windows.h>

static VOID(CALLBACK *wpt_GetSystemTimePreciseAsFileTime)(LPFILETIME) = NULL;

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

// precise sleep function based on the CPython 3.11 implementation
static PyObject *precise_sleep(PyObject *self, PyObject *args)
{
    // get time of function call
    FILETIME file_time;
    wpt_GetSystemTimePreciseAsFileTime(&file_time);

    double timeout_s;
    if (!PyArg_ParseTuple(args, "d", &timeout_s))
        return NULL;

    // convert timeout from seconds to 100ns intervals and add it to current time
    LARGE_INTEGER due_time;
    due_time.LowPart = file_time.dwLowDateTime;
    due_time.HighPart = file_time.dwHighDateTime;
    due_time.QuadPart += (LONGLONG)(timeout_s * 10000000);

    HANDLE timer = CreateWaitableTimerExW(NULL, NULL,
                                          CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                          TIMER_ALL_ACCESS);
    if (timer == NULL)
    {
        PyErr_SetFromWindowsErr(0);
        return NULL;
    }

    if (!SetWaitableTimerEx(timer, &due_time,
                            0,          // no period; the timer is signaled once
                            NULL, NULL, // no completion routine
                            NULL,       // no wake context; do not resume from suspend
                            0))         // no tolerable delay for timer coalescing
    {
        PyErr_SetFromWindowsErr(0);
        goto error;
    }

    DWORD rc;
    DWORD wait_timeout_ms = 1111;  // check for SIGINT in regular intervals
    while (1)
    {
        Py_BEGIN_ALLOW_THREADS
            rc = WaitForSingleObject(timer, wait_timeout_ms);
        Py_END_ALLOW_THREADS

        if (rc == WAIT_OBJECT_0)
        {
            // Timer signaled: we are done
            break;
        }

        if (rc == WAIT_FAILED)
        {
            PyErr_SetFromWindowsErr(0);
            goto error;
        }

        if (PyErr_CheckSignals())
        {
            goto error;
        }
    }

    CloseHandle(timer);
    Py_RETURN_NONE;

error:
    CloseHandle(timer);
    return NULL;
}

static struct PyMethodDef methods[] = {
    {"time",
     precise_time,
     METH_NOARGS,
     "Retrieve the current system time with the highest possible level of precision (<1us)."},
    {"sleep",
     precise_sleep,
     METH_VARARGS,
     "Sleep for the given time using CREATE_WAITABLE_TIMER_HIGH_RESOLUTION."},
    {NULL} // sentinel
};

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "win_precise_time._t",
    .m_doc = "",
    .m_size = -1,
    .m_methods = methods};

PyMODINIT_FUNC PyInit__t(void)
{
    HINSTANCE hKernel32;

    hKernel32 = GetModuleHandleW(L"KERNEL32");
    /* Function available on Windows 8, Windows Server 2012 and newer */
    *(FARPROC *)&wpt_GetSystemTimePreciseAsFileTime = GetProcAddress(hKernel32, "GetSystemTimePreciseAsFileTime");

    /* ensure that the system clock works */
    if (wpt_GetSystemTimePreciseAsFileTime == NULL)
    {
        return (NULL);
    }

    PyObject *module_p;
    module_p = PyModule_Create(&module);

    if (module_p == NULL)
    {
        return (NULL);
    }

    return (module_p);
}
