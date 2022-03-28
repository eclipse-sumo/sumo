# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2022 German Aerospace Center (DLR) and others.
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

import re


# remove from
def removeFrom(line):
    while ((len(line) > 0) and (line[0] != '(')):
        line = line[1:]
    if (len(line) > 0):
        line = line[1:]
    return line
        
# remove to
def removeTo(line):
    solution = ""
    while ((len(line) > 0) and (line[0] != ',')):
        solution += line[0]
        line = line[1:]
    return solution

# dictionary
dic = {'dummy': 1000}

with open("report.txt", "r") as fp:
    lines = fp.readlines()

# iterate over lines
for line in lines:
    if ("netedit.attrs" in line):
        # remove first element all until (
        line = removeFrom(line)
        # remove "referencePosition, "
        line = line.replace("referencePosition, ", "")
        # remove last element until ,
        line = removeTo(line)
        # replace extra characters
        line = line.replace(')', '')
        # check size
        if (len(line) > 0):
            found = False
            for key in dic:
                if (key == line):
                    dic[key] += 1
                    found = True
            
            if (found == False):
                dic[line] = 0
# save
with open("missingTest.txt", "w") as fp:
    for key in dic:
        if (dic[key] == 0):
            if (key[-1] == '\n'):
                fp.write(key)
            else :
                fp.write(key + "\n")