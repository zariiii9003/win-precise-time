Examples
========

.. _timestamp resolution example:

Timestamp Resolution Example
----------------------------

.. literalinclude:: ../examples/timestamp_resolution.py
   :language: python
   :linenos:

Output::

   time.time()=1665189245.756171703 - wpt.time()=1665189245.756698370
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756715059
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756718636
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756721973
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756725311
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756728649
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756731749
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756735563
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756738901
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756742001
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756745338
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756748676
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756752014
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756755829
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756758928
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756762028
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756765366
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756768465
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756771326
   time.time()=1665189245.756171703 - wpt.time()=1665189245.756774426

:func:`win_precise_time.time` always provides a fresh timestamp while the builtin
:func:`time.time` is updated every 15ms.

Soft Realtime Example
---------------------

.. literalinclude:: ../examples/soft_realtime.py
   :language: python
   :linenos:

Output::

   0.00101 - 1ms task
   0.00200 - 1ms task
   0.00301 - 1ms task
   0.00400 - 1ms task
   0.00504 - 1ms task
   0.00508 - 5ms task
   0.00600 - 1ms task
   0.00705 - 1ms task
   0.00806 - 1ms task
   0.00902 - 1ms task
   0.01000 - 1ms task
   0.01004 - 5ms task
   0.01005 - 10ms task

.. note::
   In larger applications the garbage collector (:mod:`gc`) will become problematic for timing precision.
   Use :func:`gc.disable` if your application allows it.
