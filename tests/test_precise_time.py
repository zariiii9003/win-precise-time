import pytest


def test_module_import():
    import win_precise_time


def test_function_import():
    from win_precise_time import time


def test_correct_time():
    import win_precise_time
    import time

    assert win_precise_time.time() == pytest.approx(time.time())


def test_no_args():
    import win_precise_time

    with pytest.raises(TypeError):
        win_precise_time.time("arg")
