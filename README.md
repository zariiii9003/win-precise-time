# win-precise-time

[![PyPI - Version](https://img.shields.io/pypi/v/win-precise-time.svg)](https://pypi.org/project/win-precise-time)
[![PyPI - Python Version](https://img.shields.io/pypi/pyversions/win-precise-time.svg)](https://pypi.org/project/win-precise-time)
[![Documentation Status](https://readthedocs.org/projects/win-precise-time/badge/?version=latest)](https://win-precise-time.readthedocs.io/en/latest/?badge=latest)

The documentation is available [here](https://win-precise-time.readthedocs.io/en/latest/).

-----

**Table of Contents**

- [Description](#description)
- [Installation](#installation)
- [Usage](#usage)
- [Test](#test)
- [Build](#build)
- [License](#license)

## Description

On Windows the builtin `time.time()` provides a timestamp with ~15ms resolution. 
This package provides the function `win_precise_time.time()` to retrieve an accurate timestamp by using
[GetSystemTimePreciseAsFileTime](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtimepreciseasfiletime).
The function is implemented as a C extension so the performance is equal to the builtin `time.perf_counter()` 
(both functions rely on [QueryPerformanceCounter](https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter)).

The function `win_precise_time.sleep()` reimplements the more precise `time.sleep()` version of CPython >= 3.11 for older python versions. 
This enables sub-millisecond sleep precision without increasing the Windows timer resolution via [timeBeginPeriod](https://docs.microsoft.com/en-us/windows/win32/api/timeapi/nf-timeapi-timebeginperiod).

## Installation

```console
pip install win-precise-time
```

## Usage

```python-repl
>>> import win_precise_time as wpt
>>> wpt.time()  # retrieve current time
1654539449.4548845
>>> wpt.sleep(0.001)  # sleep for 1ms
```

## Test

```console
pip install tox
tox
```

## Build

```console
pip install build
python -m build .
```

## License

`win-precise-time` is distributed under the terms of the [MIT](https://spdx.org/licenses/MIT.html) license.
