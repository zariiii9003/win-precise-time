# SPDX-FileCopyrightText: 2022-present Artur Drogunow <artur.drogunow@zf.com>
#
# SPDX-License-Identifier: MIT

from pathlib import Path
from setuptools import Extension, setup

import_pkg_name = "win_precise_time"
pkg_dir = Path("src") / import_pkg_name

t_module = Extension(
    f"{import_pkg_name}._t",
    sources=[str(pkg_dir / "_t.c")],
    include_dirs=[pkg_dir.as_posix()],
)

setup(ext_modules=[t_module])
