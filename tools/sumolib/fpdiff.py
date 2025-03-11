# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    fpdiff.py
# @author  Michael Behrisch
# @date    2014-08-26


def _getNumberAt(line, pos):
    start = pos
    eSeen = False
    dotSeen = False
    while start > 0 and line[start-1] in "1234567890.eE-+":
        if line[start-1] in "eE":
            if eSeen:
                break
            eSeen = True
        if line[start-1] == ".":
            if dotSeen:
                break
            dotSeen = True
        start -= 1
    end = pos
    while end < len(line) and line[end] in "1234567890.eE-+":
        if line[end] in "eE":
            if eSeen:
                break
            eSeen = True
        if line[end] == ".":
            if dotSeen:
                break
            dotSeen = True
        end += 1
    return line[start:end], line[end:]


def _fpequalAtPos(l1, l2, tolerance, relTolerance, pos):
    number1, l1 = _getNumberAt(l1, pos)
    number2, l2 = _getNumberAt(l2, pos)
    try:
        equal = False
        deviation = abs(float(number1) - float(number2))
        if tolerance is not None and deviation <= tolerance:
            equal = True
        elif relTolerance is not None:
            referenceValue = abs(float(number1))
            if referenceValue == 0:
                equal = (deviation == 0)
            elif deviation / referenceValue <= relTolerance:
                equal = True
    except ValueError:
        pass
    return equal, l1, l2


def _fpequal(l1, l2, tolerance, relTolerance):
    pos = 0
    while pos < min(len(l1), len(l2)):
        if l1[pos] != l2[pos]:
            equal, l1, l2 = _fpequalAtPos(l1, l2, tolerance, relTolerance, pos)
            if not equal:
                return False
            pos = 0
        else:
            pos += 1
    if len(l1) == len(l2):
        return True
    else:
        return _fpequalAtPos(l1, l2, tolerance, relTolerance, pos)[0]


def fpfilter(fromlines, tolines, tolerance, relTolerance=None):
    out = []
    for fromline, toline in zip(fromlines, tolines):
        if fromline == toline or _fpequal(fromline, toline, tolerance, relTolerance):
            out.append(fromline)
        else:
            out.append(toline)
    return out + tolines[len(fromlines):]


def diff(fromlines, tolines, tolerance, relTolerance=None):
    out = []
    for fromline, toline in zip(fromlines, tolines):
        if fromline != toline and not _fpequal(fromline, toline, tolerance, relTolerance):
            out.append("< " + fromline)
            out.append("> " + toline)
    for line in tolines[len(fromlines):]:
        out.append("> " + line)
    for line in fromlines[len(tolines):]:
        out.append("< " + line)
    return out
