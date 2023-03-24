#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    filterDebugDLL.py
# @author  Michael Behrisch
# @date    2021-11-02

"""
Filters a list of filenames whether the names are debug DLLs.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys


def filterDLL(filelist):
    baselist = [os.path.basename(d) for d in filelist]
    for idx, dll in enumerate(baselist):
        keep = dll != "FOXDLLD-1.6.dll"
        for suffix in ("d.dll", "D.dll", "-d.dll", "-D.dll", "_d.dll", "_D.dll"):
            if dll.endswith(suffix) and dll[:-len(suffix)] + ".dll" in baselist:
                keep = False
                break
        if keep:
            yield filelist[idx]


if __name__ == "__main__":
    sys.stdout.write(";".join(filterDLL(sys.argv[1:])))
