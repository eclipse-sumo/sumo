"""
@file    makeRoutes.py
@author  Daniel Krajzewicz
@date    2014-10-06
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
@date    2014-09-01
"""

from __future__ import print_function

flows = [
    ["nm", [
        ["ms", 383, 7],
        ["me", 54, 4],
        ["mw", 32, 6]
    ]],

    ["wm", [
        ["me", 472, 6],
        ["mn", 65, 6],
        ["ms", 58, 8]
    ]],

    ["em", [
        ["mw", 552, 7],
        ["mn", 76, 5],
        ["ms", 186, 6]
    ]],

    ["sm", [
        ["mn", 402, 11],
        ["me", 387, 5],
        ["mw", 48, 9]
    ]]

]


fd = open("input_flows.flows.xml", "w")
print("<routes>", file=fd)


for s in flows:
    for d in s[1]:
        id = s[0] + '2' + d[0]
        noLKW = int(float(d[1]) * float(d[2]) / 100.)  # hmph, so korrekt?
        noPKW = d[1] - noLKW
        print('     <flow id="%sPKW" from="%s" to="%s" number="%s" type="PKW" begin="0" end="3600" departPos="base" arrivalPos="-1" departSpeed="max" departLane="best"/>' % (
            id, s[0], d[0], noPKW), file=fd)
        print('     <flow id="%sLKW" from="%s" to="%s" number="%s" type="LKW" begin="0" end="3600" departPos="base" arrivalPos="-1" departSpeed="max" departLane="best"/>' % (
            id, s[0], d[0], noLKW), file=fd)
    print("", file=fd)

print("</routes>\n", file=fd)
fd.close()
