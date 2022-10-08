Library API
-----------

.. _timestamp functions:

Timestamp Functions
^^^^^^^^^^^^^^^^^^^

.. function:: win_precise_time.time() -> float

   Return the time in seconds since the `epoch`_ as a floating point number.
   See :func:`time.time` for a detailed description. This function provides
   a timestamp with <1us resolution while the builtin :func:`~time.time` function
   has a resolution of around 15ms.

   :return:
      timestamp in seconds


.. function:: win_precise_time.time_ns() -> int

   Similar to :func:`time` but returns time as an integer number of nanoseconds
   since the `epoch`_.

   :return:
      timestamp in nanoseconds

.. _sleep functions:

Sleep Functions
^^^^^^^^^^^^^^^

.. function:: win_precise_time.sleep(secs: float, /) -> None

   Suspend execution of the calling thread for the given number of seconds.
   See :func:`time.sleep` for a detailed description. This function provides
   the same accuracy as the Python 3.11 implementation.
   Earlier Python version were unable to achieve sub-millisecond precision.

   :param float secs:
      the sleep duration in seconds

.. function:: win_precise_time.sleep_until(t_wakeup_s: float, /) -> None

   Suspend execution of the calling thread until :func:`time` == *t_wakeup_s*.

   :param float t_wakeup_s:
      the wakeup time in seconds since the `epoch`_

.. function:: win_precise_time.sleep_until_ns(t_wakeup_ns: int, /) -> None

   Suspend execution of the calling thread until :func:`time_ns` == *t_wakeup_ns*.

   :param int t_wakeup_ns:
      the wakeup times in nanoseconds since the `epoch`_

.. function:: win_precise_time.hotloop_until_ns(t_wakeup_ns: int, /) -> None

   Run a busy loop until :func:`time_ns` == *t_wakeup_ns*.
   This function fully loads the CPU core and does not release the
   global interpreter lock (GIL). This function must only be used for
   very short wait time to achieve the highest wakeup time precision.

   :param int t_wakeup_ns:
      the wakeup time in nanoseconds since the `epoch`_

   .. warning::
      :func:`hotloop_until_ns` makes the process unresponsive.
      It is only useful for very short high precision waiting.

.. _epoch: https://docs.python.org/3/library/time.html#epoch