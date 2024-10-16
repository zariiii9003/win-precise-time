# SPDX-FileCopyrightText: 2022-present Artur Drogunow <artur.drogunow@zf.com>
#
# SPDX-License-Identifier: MIT

from win_precise_time._t import (
    time as time,
    time_ns as time_ns,
    sleep as sleep,
    sleep_until as sleep_until,
    sleep_until_ns as sleep_until_ns,
    hotloop_until_ns as hotloop_until_ns,
)
from win_precise_time._version import __version__
