#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2015-2022 German Aerospace Center (DLR) and others.
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
from os.path import dirname, exists, getmtime, join



def generateTemplate(templateHeaderFile, templatePath, templateFile):
    # get XMLtemplate file
    XMLTemplateFile = join(templatePath, templateFile)
    # read XML
    with open(XMLTemplateFile, 'r') as f:
        template = f.readlines()
    # write template
    templateHeaderFile.write("std::string " + templateFile.replace(".xml", "") + "Template = ")
    for line in template:
        # replace characters
        lineStrip = line.strip("\n")
        lineStrip = lineStrip.replace('"', '\\"')
        # write line
        templateHeaderFile.write('\"' + lineStrip + '\"' + ' ' + '\\' + '\n')
    # write last
    templateHeaderFile.write("\"\";\n\n")
    

if __name__ == "__main__":
    srcDir = join(dirname(__file__), '..', '..', 'src')
    if len(sys.argv) > 1:
        srcDir = sys.argv[1]
    # get template path
    templatePath = join(dirname(__file__), '..', '..', 'data', 'templates');
    # write templates.h
    with open("templates.h", 'w') as templateHeaderFile:
        # write header
        templateHeaderFile.write("#include <string>\n\n")
        # generate template for all files placed in data/templates
        for templateFile in os.listdir(templatePath):
            generateTemplate(templateHeaderFile, templatePath, templateFile)
