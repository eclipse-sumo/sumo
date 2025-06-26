#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    internaltestdata.py
# @author  Pablo Alvarez Lopez
# @date    May 2025

"""
This script generates thre header file with all data needed for internal tests
"""
from __future__ import absolute_import
from __future__ import print_function

import ast
import os


def flattenClassTree(node, scope=None, result=None):
    if scope is None:
        scope = []
    if result is None:
        result = {}
    for item in node.body:
        if isinstance(item, ast.ClassDef):
            scope.append(item.name)
            flattenClassTree(item, scope, result)
            scope.pop()
        elif isinstance(item, ast.Assign):
            for target in item.targets:
                if isinstance(target, ast.Name):
                    key = '.'.join(scope + [target.id])
                    value_node = item.value
                    # simple constant (number, string, etc.)
                    if isinstance(value_node, ast.Constant):
                        result[key] = value_node.value
                    # negative number -n
                    elif (isinstance(value_node, ast.UnaryOp)
                            and isinstance(value_node.op, ast.USub)
                            and isinstance(value_node.operand, ast.Constant)):
                        result[key] = -value_node.operand.value
    return result


def parseIntFile(outputFolder, file):
    # read python file
    pythonFile = "./enums/" + file + ".py"
    with open(pythonFile, "r", encoding="utf-8") as f:
        tree = ast.parse(f.read(), filename=pythonFile)
    # flatten data
    data = flattenClassTree(tree)
    # write header file
    outputFile = outputFolder + "/" + file + ".txt"
    with open(outputFile, "w", encoding="utf-8") as f:
        for key, value in sorted(data.items()):
            f.write('netedit.%s %s\n' % (key, value))


def parsePositionFile(outputFolder, file):
    # read python file
    pythonFile = "./enums/" + file + ".py"
    with open(pythonFile, "r", encoding="utf-8") as f:
        tree = ast.parse(f.read(), filename=pythonFile)
    # flatten data
    data = flattenClassTree(tree)
    # write header file
    outputFile = outputFolder + "/" + file + ".txt"
    with open(outputFile, "w", encoding="utf-8") as f:
        for key in sorted(data.keys()):
            if key.endswith(".x"):
                keyShorted = key[:-2]
                x = data[keyShorted + ".x"]
                y = data[keyShorted + ".y"]
                f.write('netedit.%s %s %s\n' % (keyShorted, x, y))


# main
if __name__ == "__main__":
    outputFolder = "./../../data/tests"
    # check if create folder
    if not os.path.exists(outputFolder):
        os.mkdir(outputFolder)
    # calculate files for data folder
    parseIntFile(outputFolder, "attributesEnum")
    parseIntFile(outputFolder, "contextualMenuOperations")
    parsePositionFile(outputFolder, "viewPositions")
