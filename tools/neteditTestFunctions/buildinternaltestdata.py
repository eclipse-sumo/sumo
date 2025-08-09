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

# @file    buildinternaltestdata.py
# @author  Pablo Alvarez Lopez
# @date    May 2025

"""
This script generates thre header file with all data needed for internal tests
"""
from __future__ import absolute_import
from __future__ import print_function

import ast
import os


def flattenClassTree(node, scope=None, result=None, local_vars=None):
    if scope is None:
        scope = []
    if result is None:
        result = {}
    if local_vars is None:
        local_vars = {}

    for item in node.body:
        if isinstance(item, ast.ClassDef):
            scope.append(item.name)
            flattenClassTree(item, scope, result, local_vars)
            scope.pop()

        elif isinstance(item, ast.Assign):
            for target in item.targets:
                if isinstance(target, ast.Name):
                    key = '.'.join(scope + [target.id])
                    value_node = item.value
                    try:
                        # evaluate if os possible
                        value = eval_ast(value_node, local_vars)
                        result[key] = value
                        local_vars[target.id] = value
                    except Exception:
                        result[key] = None
    return result


def eval_ast(node, local_vars):
    if isinstance(node, ast.Constant):
        return node.value
    elif isinstance(node, ast.Name):
        return local_vars.get(node.id)
    elif isinstance(node, ast.BinOp):
        left = eval_ast(node.left, local_vars)
        right = eval_ast(node.right, local_vars)
        return eval_binop(node.op, left, right)
    elif isinstance(node, ast.UnaryOp):
        operand = eval_ast(node.operand, local_vars)
        return eval_unaryop(node.op, operand)
    else:
        raise ValueError("Unsupported AST node: %s" % ast.dump(node))


def eval_binop(op, left, right):
    if isinstance(op, ast.Add):
        return left + right
    elif isinstance(op, ast.Sub):
        return left - right
    elif isinstance(op, ast.Mult):
        return left * right
    elif isinstance(op, ast.Div):
        return left / right
    elif isinstance(op, ast.FloorDiv):
        return left // right
    elif isinstance(op, ast.Mod):
        return left % right
    elif isinstance(op, ast.Pow):
        return left ** right
    else:
        raise ValueError("Unsupported binary operator: %s" % op)


def eval_unaryop(op, operand):
    if isinstance(op, ast.UAdd):
        return +operand
    elif isinstance(op, ast.USub):
        return -operand
    else:
        raise ValueError("Unsupported unary operator: %s" % op)


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


def parseRouteFile(outputFolder, file):
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
            if key.endswith(".up"):
                keyShorted = key[:-3]
                up = data[keyShorted + ".up"]
                down = data[keyShorted + ".down"]
                left = data[keyShorted + ".left"]
                right = data[keyShorted + ".right"]
                f.write('netedit.%s %s %s %s %s\n' % (keyShorted, up, down, left, right))


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
    parseRouteFile(outputFolder, "movements")
