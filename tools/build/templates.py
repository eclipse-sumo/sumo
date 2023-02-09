#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2015-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    templates.py
# @author  Pablo Alvarez Lopez
# @date    Dec 2022

"""
This script rebuilds "src/netedit/templates.h" the files
representing the templates.
It does this by parsing the data from the sumo data dir.
"""

from __future__ import print_function
from __future__ import absolute_import

import sys
import os
from os.path import dirname, join
from subprocess import check_output

def formatTemplate(templateStr):
    # remove endlines in Windows und linux
    templateStr = templateStr.replace("\\r", " ")
    templateStr = templateStr.replace("\\n", " ")
    # avoid double spaces
    templateStr = templateStr.replace("  ", " ")
    # replace " with \"
    templateStr = templateStr.replace('"', '\\"')
    # replace first backspace
    templateStr = templateStr.replace("\\b'", '"')
    # remove two last characters
    templateStr = templateStr[:-2]
    templateStr += '";'
    return templateStr


# def generateToolTemplate(toolDir):


def generateSUMOTemplate(binDir):
    # create a list with all sumo binaries
    for sumoBin in [binDir + "/sumo", binDir + "/sumo.exe", binDir + "/sumoD", binDir + "/sumoD.exe"]:
        if os.path.exists(sumoBin):
            # obtain template piping stdout using check_output
            template = str(check_output([sumoBin, "--save-template", "stdout"]))
            # join variable and formated template
            return formatTemplate(join("const std::string sumoTemplate = ", template))
    # if binary wasn't found, then raise exception
    raise Exception("SUMO Template cannot be generated. SUMO binary not found. "
                    "Make sure that sumo or sumoD was generated in bin folder")


if __name__ == "__main__":
    srcDir = join(dirname(__file__), '..', '..', 'src')
    if len(sys.argv) > 1:
        srcDir = sys.argv[1]
    # get bin dir path (SUMO/tools)
    binDir = join(dirname(__file__), '..', '..', 'bin')
    # get tool dir path (SUMO/tools)
    toolDir = join(dirname(__file__), '..', '..', 'tool')
    # generate SUMO template
    sumoTemplate = generateSUMOTemplate(binDir)
    # write templates.h
    with open("templates.h", 'w') as templateHeaderFile:
        # Convert all of the items in lst to strings (to avoid quotes)
        templateMap = map(str, ["#include <string>\n\n", sumoTemplate])  
        # Join the items together and write to the file
        templateHeaderFile.write("".join(templateMap))