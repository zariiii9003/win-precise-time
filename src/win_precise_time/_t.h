/* SPDX-FileCopyrightText: 2022-present Artur Drogunow <artur.drogunow@zf.com>
#
# SPDX-License-Identifier: MIT */

#pragma once

#ifndef Py_LIMITED_API
    #define Py_LIMITED_API
#endif
#ifndef PY_SSIZE_T_CLEAN
    #define PY_SSIZE_T_CLEAN
#endif
#include <Python.h>

static PyObject *precise_time(PyObject *self, PyObject *args);
static PyObject *precise_sleep(PyObject *self, PyObject *timeout_obj);
