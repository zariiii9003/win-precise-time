# SPDX-FileCopyrightText: 2022-present Artur Drogunow <artur.drogunow@zf.com>
#
# SPDX-License-Identifier: MIT

__version__ = "1.3.1"

from win_precise_time._t import (
    time,
    time_ns,
    sleep,
    _sleep_until,
    _sleep_until_ns,
    _hotloop_until_ns,
)
