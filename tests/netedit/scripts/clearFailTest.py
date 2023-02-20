#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    clearFailTest.py
# @author  Pablo Alvarez Lopez
# @date    2022-03-21

with open("failTests.txt", "r") as fp:
    lines = fp.readlines()

with open("failTests.txt", "w") as fp:
    # write header
    fp.write("-tp appdata=netedit.gui\n")
# iterate over lines
    for line in lines:
        lineStrip = line.strip("\n")
        if (("Test\t" not in lineStrip) and
                ("Summary\t" not in lineStrip) and
                ("test_default.daily" not in lineStrip) and
                ("daily." not in lineStrip) and
                ("0" not in lineStrip)):
            # clear substrings
            lineStrip = lineStrip.replace("\t", " ")
            lineStrip = lineStrip.replace("(+)", " ")
            lineStrip = lineStrip.replace("ok", "")
            lineStrip = lineStrip.replace("N/A", "")
            lineStrip = lineStrip.replace("ts-sim-build-ba", "")
            lineStrip = lineStrip.replace("errors different", "")
            lineStrip = lineStrip.replace("net different", "")
            lineStrip = lineStrip.replace("routes different", "")
            lineStrip = lineStrip.replace("log different", "")
            lineStrip = lineStrip.replace("output different", "")
            lineStrip = lineStrip.replace("additionals different", "")
            lineStrip = lineStrip.replace("output new", "")
            while (lineStrip.find('  ') > 0):
                lineStrip = lineStrip.replace("  ", "")
            lineStrip = lineStrip.replace(" /n", "/n")
            lineStrip = lineStrip.replace(" ", "/")
            if ((len(lineStrip) > 0) and (lineStrip[-1] == '/')):
                lineStrip = lineStrip[:-1]
        # write line
            fp.write(lineStrip)
        # check if write \n
            if (len(lineStrip) > 0):
                fp.write("\n")
