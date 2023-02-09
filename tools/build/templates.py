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
from subprocess import check_output, Popen, PIPE


def formatBinTemplate(templateStr):
    print (templateStr)

    # remove endlines in Windows
    templateStr = templateStr.replace("\\r", '')
    # replace " with \"
    templateStr = templateStr.replace('"', '\\"')
    # split lines
    templateStr = templateStr.replace("\\n", '"\n    "')
    # avoid unused lines
    templateStr = templateStr.replace('    ""\n', "")
    # replace first backspace
    templateStr = templateStr.replace("\\b'", '"')
    # remove two last characters
    templateStr = templateStr[:-2]
    templateStr += '";'
    # update last line
    templateStr = templateStr.replace('\n    ";', ';\n\n')
    return templateStr

def formatToolTemplate(templateStr):
    # remove first backslash
    templateStr = templateStr.replace('\\', '')
    # replace " with \"
    templateStr = templateStr.replace('"', '\\"')
    # add quotes and end lines
    templateStr = templateStr.replace("<", '"<')
    templateStr = templateStr.replace("\n", '"\n')
    templateStr = templateStr[:-1]
    # update last line
    templateStr += ';\n\n'
    return templateStr


def generateSUMOTemplate(binDir):
    # create a list with all sumo binaries
    for sumoBin in [binDir + "/sumo", binDir + "/sumo.exe", binDir + "/sumoD", binDir + "/sumoD.exe"]:
        if os.path.exists(sumoBin):
            # obtain template piping stdout using check_output
            template = str(check_output([sumoBin, "--save-template", "stdout"]))
            # join variable and formated template
            return formatBinTemplate(join("const std::string sumoTemplate = ", template))
    # if binary wasn't found, then raise exception
    raise Exception("SUMO Template cannot be generated. SUMO binary not found. "
                    "Make sure that sumo or sumoD was generated in bin folder")


def generateToolTemplate(srcDir, toolDir, toolName):
    # get toolPath
    toolPath = toolDir + "/" + toolName + ".py";
    
    print (["python", toolPath, "--save-template stdout"])
    
    # check if exists
    if os.path.exists(toolPath):
        # obtain template saving it toolTemplate.xml
        Popen("python " + toolPath + " --save-template toolTemplate.xml")
        # read XML
        with open(srcDir + "/netedit/toolTemplate.xml", 'r') as f:
            template = f.read()
        # join variable and formated template
        return formatToolTemplate(join("const std::string " + toolName + "Template = ", template))
    # if tool wasn't found, then raise exception
    raise Exception(toolName + "Template cannot be generated. '" + toolPath + "' not found.")


if __name__ == "__main__":
    srcDir = join(dirname(__file__), '..', '..', 'src')
    if len(sys.argv) > 1:
        srcDir = sys.argv[1]
    # get bin dir path (SUMO/bin)
    binDir = join(dirname(__file__), '..', '..', 'bin')
    # get tool dir path (SUMO/tools)
    toolDir = join(dirname(__file__), '..')
    # generate SUMO template
    sumoTemplate = generateSUMOTemplate(binDir)
    # generate Tool template
    toolTemplate = generateToolTemplate(srcDir, toolDir, "generateRerouters")
    # write templates.h
    with open("templates.h", 'w') as templateHeaderFile:
        # Convert all of the items in lst to strings (to avoid quotes)
        templateMap = map(str, ["#include <string>\n\n", sumoTemplate, toolTemplate])  
        # Join the items together and write to the file
        templateHeaderFile.write("".join(templateMap))