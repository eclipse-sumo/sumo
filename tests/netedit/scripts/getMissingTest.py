#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    getMissingTest.py
# @author  Pablo Alvarez Lopez
# @date    2022-03-21

import os
import scandir


def removeFrom(line):
    index = line.find("netedit.attrs.")
    if index != -1:
        return line[index:]
    else:
        return line


def removeTo(line):
    solution = ""
    while ((len(line) > 0) and ((line[0] == ".") or line[0].isalpha() or line[0].isnumeric())):
        solution += line[0]
        line = line[1:]
    return solution


# file lists
fileList = []

# get all test.py
for paths, dirs, files in scandir.walk("../"):
    for file in files:
        if file.endswith("test.py"):
            fileList.append(os.path.join(paths, file))

# and all references in sumo tools
for paths, dirs, files in scandir.walk("../../../tools/neteditTestFunctions"):
    for file in files:
        if file.endswith(".py"):
            fileList.append(os.path.join(paths, file))

# list of references
references = []

# get all lines with "netedit.attrs"
for file in fileList:
    with open(file, "r") as fp:
        lines = fp.readlines()
    for line in lines:
        if ("netedit.attrs." in line):
            references.append(line)
        elif ("attrs." in line):
            line = line.replace("attrs.", "netedit.attrs.")
            references.append(line)

"""
# save references
with open("references.txt", "w") as fp:
    for reference in references:
        fp.write(reference)
"""

# cleaned references
cleanedReferences = []

# iterate over lines
for reference in references:
    if ("." in line):
        # remove first element all until (
        reference = removeFrom(reference)
        # remove last element until ,
        reference = removeTo(reference)
        # replace extra characters
        reference = reference.replace("netedit.attrs.", "")
        if (len(reference) > 0):
            # add endline
            if (reference[-1] != "\n"):
                reference += "\n"
            # add into cleanedReferences
            cleanedReferences.append(reference)


# save cleanedReferences
with open("cleanedReferences.txt", "w") as fp:
    for cleanedReference in cleanedReferences:
        fp.write(cleanedReference)


# open enumsXML.txt and append to cleanedReferences
with open("enumsXML.txt", "r") as fp:
    cleanedReferences += fp.readlines()

# sort
cleanedReferences.sort()


# save
with open("cleanedReferencesAndEnums.txt", "w") as fp:
    for cleanedReference in cleanedReferences:
        fp.write(cleanedReference)


# dictionary
dic = {"dummy": 1000}

# get number of
for reference in cleanedReferences:
    # remove all spaces
    reference = reference.replace(" ", "")
    # remove all spaces
    reference = reference.replace("+1", "")
    # check number of dots
    if (reference.count(".") > 1):
        found = False
        for key in dic:
            if (key == reference):
                dic[key] += 1
                found = True
    # add in diccionary
        if not found:
            dic[reference] = 0

# save missing test
with open("missingTest.txt", "w") as fp:
    for key in dic:
        if (dic[key] == 0):
            fp.write(key)
