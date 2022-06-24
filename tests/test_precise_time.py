import pytest


def test_module_import():
    import win_precise_time


def test_function_import():
    from win_precise_time import time
    from win_precise_time import sleep


def test_correct_time():
    import win_precise_time
    import time

    assert win_precise_time.time() == pytest.approx(time.time())


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

    assert 1.5 < t1 - t0 < 2.0
