#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    neteditToolTemplate.py
# @author  Josh
# @date    2026-11-26

"""Return a tool's option template using Netedit's Python interpreter."""

import os
import subprocess
import sys


def main():
    if len(sys.argv) != 2:
        sys.exit("Usage: neteditToolTemplate.py TOOL")
    env = dict(os.environ, PYTHONIOENCODING="utf-8")
    result = subprocess.run([sys.executable, sys.argv[1], "--save-template", "stdout"],
                            env=env, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode == 0 and result.stdout.lstrip().startswith(b"<"):
        sys.stdout.buffer.write(result.stdout)
        return
    diagnostics = result.stderr.decode("utf-8", errors="replace").strip().splitlines()
    missing_module = next((line for line in reversed(diagnostics) if line.startswith("ModuleNotFoundError:")), None)
    if missing_module is not None:
        sys.exit("%s\nInstall the missing package in the Python environment used by Netedit (%s)."
                 % (missing_module, sys.executable))
    sys.exit(diagnostics[-1] if diagnostics else "The Python tool did not return an XML option template.")


if __name__ == "__main__":
    main()
