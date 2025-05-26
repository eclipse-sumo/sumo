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
import sys


# flatten class tree
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
                    elif isinstance(value_node, ast.UnaryOp) and isinstance(value_node.op, ast.USub) and isinstance(value_node.operand, ast.Constant):
                        result[key] = -value_node.operand.value
    return result

# parse int file


def parseIntFile(inputFolder, outputFolder, file, prefix):
    print("Generating internal auxiliar test file '" + file + ".txt'")
    # read python file
    pythonFile = inputFolder + "/" + file + ".py"
    with open(pythonFile, "r", encoding="utf-8") as f:
        tree = ast.parse(f.read(), filename=pythonFile)
    # flatten data
    data = flattenClassTree(tree)
    # write header file
    headerFile = outputFolder + "/" + file + ".txt"
    with open(headerFile, "w", encoding="utf-8") as f:
        for key, value in sorted(data.items()):
            f.write(f'netedit.{prefix}.{key} {value}\n')

# parse position file


def parsePositionFile(inputFolder, outputFolder, file, prefix):
    print("Generating internal auxiliar test file '" + file + ".txt'")
    # read python file
    pythonFile = inputFolder + "/" + file + ".py"
    with open(pythonFile, "r", encoding="utf-8") as f:
        tree = ast.parse(f.read(), filename=pythonFile)
    # flatten data
    data = flattenClassTree(tree)
    # write header file
    headerFile = outputFolder + "/" + file + ".txt"
    with open(headerFile, "w", encoding="utf-8") as f:
        for key in sorted(data.keys()):
            if key.endswith(".x"):
                keyShorted = key[:-2]
                x = data[keyShorted + ".x"]
                y = data[keyShorted + ".y"]
                f.write(f'netedit.{prefix}.{keyShorted} {x} {y}\n')


# main
if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit("Arguments: <pathToSumoSrcFolder>")
    inputFolder = sys.argv[1] + "/../../../../tests/netedit"
    outputFolder = sys.argv[1] + "/../../../../data/tests"
    # check if create folder
    if os.path.exists(outputFolder) == False:
        os.mkdir(outputFolder)
    # obtain header files
    parseIntFile(inputFolder, outputFolder, "attributesEnum", "attrs")
    parseIntFile(inputFolder, outputFolder, "contextualMenuOperations", "contextualMenu")
    parsePositionFile(inputFolder, outputFolder, "viewPositions", "positions")
