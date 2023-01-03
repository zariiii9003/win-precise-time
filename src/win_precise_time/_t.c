/* SPDX-FileCopyrightText: 2022-present Artur Drogunow <artur.drogunow@zf.com>
#
# SPDX-License-Identifier: MIT */

#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <Windows.h>

#define S_TO_100NS 10000000
#define FILETIME_OFFSET_NS 11644473600000000000ULL


static VOID(CALLBACK *wpt_GetSystemTimePreciseAsFileTime)(LPFILETIME) = NULL;
static HANDLE(CALLBACK *wpt_CreateWaitableTimerExW)(LPSECURITY_ATTRIBUTES, 
                                                    LPCWSTR, 
                                                    DWORD, 
                                                    DWORD) = NULL;
static BOOL(CALLBACK *wpt_SetWaitableTimerEx)(HANDLE,
                                              LARGE_INTEGER*,
                                              LONG,
                                              PTIMERAPCROUTINE,
                                              LPVOID,
                                              PREASON_CONTEXT,
                                              ULONG) = NULL;


static ULONGLONG _wpt_time_ns()
{
    FILETIME file_time;
    ULARGE_INTEGER large;

    wpt_GetSystemTimePreciseAsFileTime(&file_time);
    large.LowPart = file_time.dwLowDateTime;
    large.HighPart = file_time.dwHighDateTime;

    return (large.QuadPart * 100 - FILETIME_OFFSET_NS);
}


// helper function to check wether remaining sleep duration is below threshold
static BOOL _sleep_time_is_below_threshold(LONGLONG due_time, LONGLONG thr)
{
    if (due_time < 0)
    {
        // relative due time
        return (-1 * due_time < thr);
    }
    else
    {
        // absolute due time
        ULONGLONG current_time = _wpt_time_ns() / 100;  // current time in 100ns intervals
        return ((due_time - (LONGLONG)current_time) < thr);
    }
}


static int
_sleep_until(LARGE_INTEGER *due_time)
{
    HANDLE timer = wpt_CreateWaitableTimerExW(NULL, 
                                              NULL,
                                              CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                              TIMER_ALL_ACCESS);
    if (timer == NULL)
    {
        DWORD firstError = GetLastError();
        if (firstError == 87ul)
        {
            // try fallback without CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
            timer = wpt_CreateWaitableTimerExW(NULL, 
                                               NULL,
                                               0,
                                               TIMER_ALL_ACCESS);
            if (timer == NULL)
            {
                PyErr_Format(PyExc_OSError, "CreateWaitableTimerExW failed with error %lu", GetLastError());
                return (-1);
            }
        }
        else
        {
            PyErr_Format(PyExc_OSError, "CreateWaitableTimerExW failed with error %lu", firstError);
            return (-1);
        }
    }

    if (!wpt_SetWaitableTimerEx(timer,
                                due_time,
                                0,         // no period; the timer is signaled once
                                NULL,      // callback from signalling thread
                                NULL,      // argument to callback
                                NULL,      // no wake context; do not resume from suspend
                                0))        // no tolerable delay for timer coalescing
    {
        PyErr_Format(PyExc_OSError, "SetWaitableTimerEx failed with error %lu", GetLastError());
        goto error;
    }

    DWORD rc;
    // Only the main thread can be interrupted by SIGINT.
    // Signal handlers are only executed in the main thread.
    if (_PyOS_IsMainThread()) {
        HANDLE sigint_event = _PyOS_SigintEvent();

        while (1) {
            
            // Check for pending SIGINT signal before resetting the event
            if (PyErr_CheckSignals()) {
                goto error;
            }
            ResetEvent(sigint_event);

            HANDLE events[] = {timer, sigint_event};
            
            BOOL keep_gil = _sleep_time_is_below_threshold(due_time->QuadPart, 3000LL);  // thr=0.3ms
            if (keep_gil)
            {
                rc = WaitForSingleObject(timer, INFINITE);
            }
            else
            {
                Py_BEGIN_ALLOW_THREADS
                rc = WaitForMultipleObjects(Py_ARRAY_LENGTH(events), events,
                                            // bWaitAll
                                            FALSE,
                                            // No wait timeout
                                            INFINITE);
                Py_END_ALLOW_THREADS
            }

            if (rc == WAIT_FAILED) {
                PyErr_Format(PyExc_OSError, "WaitForMultipleObjects failed with error %lu", GetLastError());
                goto error;
            }

            if (rc == WAIT_OBJECT_0) {
                // Timer signaled: we are done
                break;
            }

            if (!keep_gil)
            {
                assert(rc == (WAIT_OBJECT_0 + 1));
                // The sleep was interrupted by SIGINT: restart sleepin
            }
        }
    }
    else 
    {
        BOOL keep_gil = _sleep_time_is_below_threshold(due_time->QuadPart, 3000LL);  // thr=0.3ms
        if (keep_gil)
        {
            rc = WaitForSingleObject(timer, INFINITE);
        }
        else
        {
            Py_BEGIN_ALLOW_THREADS
            rc = WaitForSingleObject(timer, INFINITE);
            Py_END_ALLOW_THREADS
        }

        if (rc == WAIT_FAILED) {
            PyErr_Format(PyExc_OSError, "WaitForSingleObject failed with error %lu", GetLastError());
            goto error;
        }

        assert(rc == WAIT_OBJECT_0);
        // Timer signaled: we are done            
    }

    CloseHandle(timer);
    return (0);

error:
    CloseHandle(timer);
    return (-1);
}


PyDoc_STRVAR(time_ns_doc,
             "time_ns() -> int\n"
             "Similar to :func:`time` but returns time as an integer number of nanoseconds since the epoch.");

static PyObject *
wpt_time_ns(PyObject *self, PyObject *args)
{
    return (PyLong_FromUnsignedLongLong(_wpt_time_ns()));
}


PyDoc_STRVAR(time_doc,
             "time() -> float\n"
             "Return the time in seconds since the epoch as a floating point "
             "number. See :func:`time.time` for a detailed description. "
             "This function provides a timestamp with <1us resolution while "
             "the builtin :func:`~time.time` function has a resolution of around 15ms.");

static PyObject *
wpt_time(PyObject *self, PyObject *args)
{
    return (PyFloat_FromDouble((double)(_wpt_time_ns()) * 1e-9));
}

PyDoc_STRVAR(sleep_doc,
             "sleep(secs: float, /) -> None\n"
             "Suspend execution of the calling thread for the given number of seconds. "
             "See :func:`time.sleep` for a detailed description. "
             "This function provides the same accuracy as the Python 3.11 implementation. "
             "Earlier Python version were unable to achieve sub-millisecond precision.\n\n"
             ":param secs:\n"
             "   the sleep duration in seconds");

static PyObject *
wpt_sleep(PyObject *self, PyObject *arg)
{

    double timeout_s;

    if (PyFloat_Check(arg))
    {
        timeout_s = PyFloat_AsDouble(arg);
    }
    else if (PyLong_Check(arg))
    {
        timeout_s = PyLong_AsDouble(arg);
    }
    else
    {
        PyErr_Format(PyExc_TypeError,
                     "Argument \"secs\" to %s must be a float object not a \"%s\"",
                     __FUNCTION__, Py_TYPE(arg)->tp_name);
        return (NULL);
    }

    if (timeout_s == -1.0 && PyErr_Occurred()) {
        return (NULL);
    }

    // take shortcut if timeout is very small or negative
    if (timeout_s <= 1e-4)
    {
        Py_RETURN_NONE;
    }

    // convert timeout from seconds to 100ns intervals and add it to current time
    LARGE_INTEGER due_time;
    due_time.QuadPart = -1 * (LONGLONG)(timeout_s * S_TO_100NS);

    if (!_sleep_until(&due_time))
    {
        Py_RETURN_NONE;
    }
    return (NULL);
}


PyDoc_STRVAR(sleep_until_doc,
             "sleep_until(t_wakeup_s: float, /) -> None\n"
             "Suspend execution of the calling thread until :func:`time` == *t_wakeup_s*.\n\n"
             ":param t_wakeup_s:\n"
             "   the wakeup time in seconds since the epoch (unix time)");

static PyObject *
wpt_sleep_until(PyObject *self, PyObject *arg)
{
    double t_wakeup_s;

    if (PyFloat_Check(arg))
    {
        t_wakeup_s = PyFloat_AsDouble(arg);
    }
    else if (PyLong_Check(arg))
    {
        t_wakeup_s = PyLong_AsDouble(arg);
    }
    else
    {
        PyErr_Format(PyExc_TypeError,
                     "Argument \"t_wakeup_s\" to %s must be a float object not a \"%s\"",
                     __FUNCTION__, Py_TYPE(arg)->tp_name);
        return (NULL);
    }

    if (t_wakeup_s == -1.0 && PyErr_Occurred()) {
        return (NULL);
    }

    // convert wakeup time from seconds to 100ns intervals and add it to current time
    LARGE_INTEGER due_time;
    due_time.QuadPart = ((LONGLONG)(t_wakeup_s * 1e9) + FILETIME_OFFSET_NS) / 100;

    if (!_sleep_until(&due_time))
    {
        Py_RETURN_NONE;
    }
    return (NULL);
}


PyDoc_STRVAR(sleep_until_ns_doc,
             "sleep_until_ns(t_wakeup_ns: int, /) -> None\n"
             "Suspend execution of the calling thread until :func:`time_ns` ``== t_wakeup_ns``.\n\n"
             ":param t_wakeup_ns:\n"
             "   the wakeup time in nanoseconds since the epoch");

static PyObject *
wpt_sleep_until_ns(PyObject *self, PyObject *arg)
{
    if (!PyLong_Check(arg))
    {
        PyErr_Format(PyExc_TypeError,
                     "Argument \"t_wakeup_ns\" to %s must be an int object not a \"%s\"",
                     __FUNCTION__, Py_TYPE(arg)->tp_name);
        return (NULL);
    }
    LONGLONG t_wakeup_ns = PyLong_AsLongLong(arg);
    if (t_wakeup_ns == -1LL && PyErr_Occurred()) {
        return (NULL);
    }

    // convert wakeup time from nanoseconds to 100ns intervals and add it to current time
    LARGE_INTEGER due_time;
    due_time.QuadPart = (t_wakeup_ns + FILETIME_OFFSET_NS) / 100;

    if (!_sleep_until(&due_time))
    {
        Py_RETURN_NONE;
    }
    return (NULL);
}


PyDoc_STRVAR(hotloop_until_ns_doc,
             "hotloop_until_ns(t_wakeup_ns: int, /) -> None\n"
             "Run a busy loop until :func:`time_ns` ``== t_wakeup_ns``. "
             "This function fully loads the CPU core and does not release the "
             "global interpreter lock (GIL). This function must only be used for "
             "very short wait time to achieve the highest wakeup time precision.\n\n"
             ":param t_wakeup_ns:\n"
             "   the wakeup time in nanoseconds since the epoch\n\n"
             ".. warning::\n"
             "   :func:`hotloop_until_ns` makes the process unresponsive. "
             "   It is only useful for very short high precision waiting.");

static PyObject *
wpt_hotloop_until_ns(PyObject *self, PyObject *arg)
{
    if (!PyLong_Check(arg))
    {
        PyErr_Format(PyExc_TypeError,
                     "Argument \"t_wakeup_ns\" to %s must be an int object not a \"%s\"",
                     __FUNCTION__, Py_TYPE(arg)->tp_name);
        return NULL;
    }
    ULONGLONG t_wakeup_ns = PyLong_AsUnsignedLongLong(arg);
    if (t_wakeup_ns == (ULONGLONG)-1 && PyErr_Occurred()) {
        return NULL;
    }

    while (_wpt_time_ns() < t_wakeup_ns);
    Py_RETURN_NONE;
}


static struct PyMethodDef methods[] = {
    {"time",
     wpt_time,
     METH_NOARGS,
     time_doc},
    {"time_ns",
     wpt_time_ns,
     METH_NOARGS,
     time_ns_doc},
    {"sleep",
     wpt_sleep,
     METH_O,
     sleep_doc},
    {"sleep_until",
     wpt_sleep_until,
     METH_O,
     sleep_until_doc},
     {"sleep_until_ns",
     wpt_sleep_until_ns,
     METH_O,
     sleep_until_ns_doc},
     {"hotloop_until_ns",
     wpt_hotloop_until_ns,
     METH_O,
     hotloop_until_ns_doc},
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
    if (wpt_GetSystemTimePreciseAsFileTime == NULL)
    { 
        PyErr_SetString(PyExc_OSError, "Win32 function \"GetSystemTimePreciseAsFileTime\" not available.");
        return (NULL);
    }

    *(FARPROC *)&wpt_CreateWaitableTimerExW = GetProcAddress(hKernel32, "CreateWaitableTimerExW");
    if (wpt_CreateWaitableTimerExW == NULL)
    { 
        PyErr_SetString(PyExc_OSError, "Win32 function \"CreateWaitableTimerExW\" not available.");
        return (NULL);
    }

    *(FARPROC *)&wpt_SetWaitableTimerEx = GetProcAddress(hKernel32, "SetWaitableTimerEx");
    if (wpt_SetWaitableTimerEx == NULL)
    { 
        PyErr_SetString(PyExc_OSError, "Win32 function \"SetWaitableTimerEx\" not available.");
        return (NULL);
    }

    PyObject *module_p;
    module_p = PyModule_Create(&module);

    if (module_p == NULL) return (NULL);

    return (module_p);
}
