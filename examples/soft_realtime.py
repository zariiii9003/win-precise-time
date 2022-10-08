"""Python soft realtime example"""

import os
from ctypes import windll
from enum import IntEnum

import win_precise_time as wpt


class SoftRealtime1ms:
    def __init__(self) -> None:
        self._stopped = False

        self._cycle_time_ns = 1_000_000  # 1 millisecond
        self._counter = self._cycle_time_ns

        self._t0 = wpt.time()
        t_wakeup_ns = wpt.time_ns() + self._cycle_time_ns
        while not self._stopped:
            wpt.sleep_until_ns(t_wakeup_ns - 400_000)
            wpt.hotloop_until_ns(t_wakeup_ns)
            self._main_task()
            t_wakeup_ns += self._cycle_time_ns

    def _main_task(self) -> None:
        if self._counter % 1_000_000 == 0:
            self._task_1ms()
        if self._counter % 5_000_000 == 0:
            self._task_5ms()
        if self._counter % 10_000_000 == 0:
            self._task_10ms()
        self._counter += self._cycle_time_ns

    def _task_1ms(self) -> None:
        print(f"{wpt.time() - self._t0:.5f} - 1ms task")

    def _task_5ms(self) -> None:
        print(f"{wpt.time() - self._t0:.5f} - 5ms task")

    def _task_10ms(self) -> None:
        print(f"{wpt.time() - self._t0:.5f} - 10ms task")
        self.stop()

    def stop(self) -> None:
        self._stopped = True


class ProcessPriority(IntEnum):
    ABOVE_NORMAL_PRIORITY_CLASS = 0x00008000
    BELOW_NORMAL_PRIORITY_CLASS = 0x00004000
    HIGH_PRIORITY_CLASS = 0x00000080
    IDLE_PRIORITY_CLASS = 0x00000040
    NORMAL_PRIORITY_CLASS = 0x00000020
    PROCESS_MODE_BACKGROUND_BEGIN = 0x00100000
    PROCESS_MODE_BACKGROUND_END = 0x00200000
    REALTIME_PRIORITY_CLASS = 0x00000100


def set_process_priority(priority: ProcessPriority) -> None:
    pid = os.getpid()
    handle = windll.kernel32.OpenProcess(0x0200 | 0x0400, False, pid)
    windll.kernel32.SetPriorityClass(handle, priority)


if __name__ == "__main__":
    set_process_priority(ProcessPriority.HIGH_PRIORITY_CLASS)
    rt = SoftRealtime1ms()
