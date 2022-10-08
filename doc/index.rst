win-precise-time
================

.. only:: html

   .. image:: https://img.shields.io/pypi/v/win-precise-time.svg
       :target: https://pypi.org/project/win-precise-time
       :alt: PyPI - Version
   .. image:: https://img.shields.io/pypi/pyversions/win-precise-time.svg
       :target: https://pypi.org/project/win-precise-time
       :alt: PyPI - Python Version
   .. image:: https://readthedocs.org/projects/win-precise-time/badge/?version=latest
       :target: https://win-precise-time.readthedocs.io/en/latest/?badge=latest
       :alt: Documentation Status
   .. image:: https://github.com/zariiii9003/win-precise-time/actions/workflows/test.yml/badge.svg
       :target: https://github.com/zariiii9003/win-precise-time/actions
       :alt: Github Actions

.. toctree::
   :maxdepth: 2

   self
   api
   examples

Description
-----------

This Windows-only library provides a few improvements over the builtin :mod:`time` functions.

The :ref:`timestamp functions` of ``win-precise-time`` achieve a much better resolution than the builtin function
by using `GetSystemTimePreciseAsFileTime`_. On Windows the builtin :func:`time.time` function has a resolution of 15ms
while :func:`win_precise_time.time` has nanosecond resolution as seen in the :ref:`timestamp resolution example`.

The :ref:`sleep functions` use `high resolution timers`_ to enable sleep with sub-milliseconds precision. This change
was also implemented in `CPython 3.11 <https://github.com/python/cpython/pull/29203>`_. High resolution timers
are supported in Windows 10 version 1803 and later. Older Windows versions will not see any sleep precision improvement
with this library.

All functions of this library are implemented in C.

Installation
------------
You can install ``win-precise-time`` from `PyPI`_::

   python -m pip install win-precise-time

Usage
-----

>>> import win_precise_time as wpt
>>> wpt.time()  # retrieve current time
1654539449.4548845
>>> wpt.sleep(0.001)  # sleep for 1ms


Test
----

Run the tests with::

   python -m pip install tox
   tox


Build
-----

Build ``win-precise-time`` with::

   python -m pip install build
   python -m build .


License
-------

``win-precise-time`` is distributed under the terms of the `MIT`_ license.


.. _GetSystemTimePreciseAsFileTime: https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtimepreciseasfiletime
.. _high resolution timers: https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createwaitabletimerexw
.. _PyPI: https://pypi.org/project/win-precise-time/
.. _MIT: https://spdx.org/licenses/MIT.html