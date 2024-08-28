#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    longpath.py
# @author  Michael Behrisch
# @date    2024-08-28

import winreg

kr = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"SYSTEM\CurrentControlSet\Control\FileSystem")
value, type_id = winreg.QueryValueEx(kr, "LongPathsEnabled")
if value == 0:
    kw = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,
                        r"SYSTEM\CurrentControlSet\Control\FileSystem", access=winreg.KEY_WRITE)
    winreg.SetValueEx(kw, "LongPathsEnabled", None, type_id, 1)
    print("Registry value for long path names successfully set.")
else:
    print("Long path names are already enabled in the registry.")
