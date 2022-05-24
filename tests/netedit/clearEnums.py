#!/usr/bin/env python
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

# @file    clearEnums.py
# @author  Pablo Alvarez Lopez
# @date    2022-03-21


# remove '=' until end
def removeEqual(line):
    solution = ""
    while ((len(line) > 0) and (line[0] != '=')):
        solution += line[0]
        line = line[1:]
    return solution


# load
with open("../attributesEnum.py", "r") as fp:
    lines = fp.readlines()

# process lines
matrix = []
for line in lines:
    if ("#" not in line):
        line = removeEqual(line)
        line = line.replace("class ", "")
        line = line.replace("    ", "\t")
        line = line.replace(":", "")
        line = line.replace("\n", "")
        if ("='" in line):
            line = line[0:-4]
        if ("=" in line):
            line = line[0:-3]
        if ("=" in line):
            line = line[0:-2]
        if (len(line) > 1):
            if (line.count('\t') == 0):
                matrix.append([line])
            elif (line.count('\t') == 1):
                line = line.replace("\t", "")
                matrix.append([[], line])
            elif (line.count('\t') == 2):
                line = line.replace("\t", "")
                matrix.append([[], [], line])
            elif (line.count('\t') == 3):
                line = line.replace("\t", "")
                matrix.append([[], [], [], line])
            elif (line.count('\t') == 4):
                line = line.replace("\t", "")
                matrix.append([[], [], [], [], line])
            elif (line.count('\t') == 5):
                line = line.replace("\t", "")
                matrix.append([[], [], [], [], [], line])

# fill
for j in range(10):
    for i in range(len(matrix)):
        if ((len(matrix[i]) > j) and (matrix[i][j] == [])):
            matrix[i][j] = matrix[i-1][j]

# save
with open("enumsXML.txt", "w") as fp:
    for i in range(len(matrix)):
        line = ""
        for j in range(0, 10):
            if (len(matrix[i]) > j):
                line += str(matrix[i][j]) + "."
        line = line[:-1]
        line += '\n'
        fp.write(line)
