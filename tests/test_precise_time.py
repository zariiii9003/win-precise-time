import pytest


def test_module_import():
    import win_precise_time


def test_function_import():
    from win_precise_time import time
    from win_precise_time import time_ns
    from win_precise_time import sleep
    from win_precise_time import sleep_until
    from win_precise_time import sleep_until_ns
    from win_precise_time import hotloop_until_ns


def test_correct_time():
    import win_precise_time
    import time

    assert win_precise_time.time() == pytest.approx(time.time())


def test_correct_time_ns():
    import win_precise_time
    import time

    assert win_precise_time.time_ns() == pytest.approx(time.time_ns())


def test_no_args():
    import win_precise_time

    with pytest.raises(TypeError):
        win_precise_time.time("arg")


def test_sleep():
    import win_precise_time
    import time

    t0 = time.perf_counter()
    win_precise_time.sleep(1.5)
    t1 = time.perf_counter()

    assert t1 - t0 >= 1.5
    assert t1 - t0 < 2.0

    # test integer

    t0 = time.perf_counter()
    win_precise_time.sleep(1)
    t1 = time.perf_counter()

    assert t1 - t0 >= 1.0
    assert t1 - t0 < 1.5


def test_sleep_until():
    import win_precise_time
    import time

    t_wakeup = time.time() + 1.5
    win_precise_time.sleep_until(t_wakeup)
    t1 = time.time()

    assert t1 == pytest.approx(t_wakeup)


def test_sleep_until_ns():
    import win_precise_time
    import time

    t_wakeup = time.time_ns() + 1_500_000_000
    win_precise_time.sleep_until_ns(t_wakeup)
    t1 = time.time_ns()

    assert t1 == pytest.approx(t_wakeup)
