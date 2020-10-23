#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2007-10-25

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import xml.dom.minidom as dom
sys.path.append(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', "tools"))
from sumolib import checkBinary  # noqa

changes = [
    ["net/edge[0]@from", ""],
    ["net/edge[1]@from", ""],
    ["net/edge[0]@from", "a"],
    ["net/edge[1]@from", "a"],
    ["net/edge[0]@from", "<remove>"],
    ["net/edge[1]@from", "<remove>"],
    ["net/edge[0]@function", "a"],
    ["net/edge[1]@function", "a"],
    ["net/edge[0]@id", ""],
    ["net/edge[1]@id", ""],
    ["net/edge[0]@id", "a"],
    ["net/edge[1]@id", "a"],
    ["net/edge[0]@id", "<remove>"],
    ["net/edge[1]@id", "<remove>"],
    ["net/edge[0]@to", ""],
    ["net/edge[1]@to", ""],
    ["net/edge[0]@to", "a"],
    ["net/edge[1]@to", "a"],
    ["net/edge[0]@to", "<remove>"],
    ["net/edge[1]@to", "<remove>"],
    ["net/edge[0]", "<remove>"],
    ["net/edge[1]", "<remove>"],
    ["net/edge[0]", "<duplicate>"],
    ["net/edge[1]", "<duplicate>"],

    ["net/edge[0]/lane[0]@index", ""],
    ["net/edge[0]/lane[1]@index", ""],
    ["net/edge[1]/lane[0]@index", ""],
    ["net/edge[1]/lane[1]@index", ""],
    ["net/edge[0]/lane[0]@index", "a"],
    ["net/edge[0]/lane[1]@index", "a"],
    ["net/edge[1]/lane[0]@index", "a"],
    ["net/edge[1]/lane[1]@index", "a"],
    ["net/edge[0]/lane[0]@id", ""],
    ["net/edge[0]/lane[1]@id", ""],
    ["net/edge[1]/lane[0]@id", ""],
    ["net/edge[1]/lane[1]@id", ""],
    ["net/edge[0]/lane[0]@id", "a_0"],
    ["net/edge[0]/lane[1]@id", "a_1"],
    ["net/edge[1]/lane[0]@id", "a_0"],
    ["net/edge[1]/lane[1]@id", "a_1"],
    ["net/edge[0]/lane[0]@id", "<remove>"],
    ["net/edge[0]/lane[1]@id", "<remove>"],
    ["net/edge[1]/lane[0]@id", "<remove>"],
    ["net/edge[1]/lane[1]@id", "<remove>"],
    ["net/edge[0]/lane[0]@length", ""],
    ["net/edge[0]/lane[1]@length", ""],
    ["net/edge[1]/lane[0]@length", ""],
    ["net/edge[1]/lane[1]@length", ""],
    ["net/edge[0]/lane[0]@length", "a"],
    ["net/edge[0]/lane[1]@length", "a"],
    ["net/edge[1]/lane[0]@length", "a"],
    ["net/edge[1]/lane[1]@length", "a"],
    ["net/edge[0]/lane[0]@length", "<remove>"],
    ["net/edge[0]/lane[1]@length", "<remove>"],
    ["net/edge[1]/lane[0]@length", "<remove>"],
    ["net/edge[1]/lane[1]@length", "<remove>"],
    ["net/edge[0]/lane[0]@shape", ""],
    ["net/edge[0]/lane[1]@shape", ""],
    ["net/edge[1]/lane[0]@shape", ""],
    ["net/edge[1]/lane[1]@shape", ""],
    ["net/edge[0]/lane[0]@shape", "a"],
    ["net/edge[0]/lane[1]@shape", "a"],
    ["net/edge[1]/lane[0]@shape", "a"],
    ["net/edge[1]/lane[1]@shape", "a"],
    ["net/edge[0]/lane[0]@shape", "<remove>"],
    ["net/edge[0]/lane[1]@shape", "<remove>"],
    ["net/edge[1]/lane[0]@shape", "<remove>"],
    ["net/edge[1]/lane[1]@shape", "<remove>"],
    ["net/edge[0]/lane[0]@shape", "a,-4.95 100.00,-4.95"],
    ["net/edge[0]/lane[1]@shape", "a,-4.95 100.00,-4.95"],
    ["net/edge[1]/lane[0]@shape", "a,-4.95 100.00,-4.95"],
    ["net/edge[1]/lane[1]@shape", "a,-4.95 100.00,-4.95"],
    ["net/edge[0]/lane[0]@shape", "0.00,a 100.00,-4.95"],
    ["net/edge[0]/lane[1]@shape", "0.00,a 100.00,-4.95"],
    ["net/edge[1]/lane[0]@shape", "0.00,a 100.00,-4.95"],
    ["net/edge[1]/lane[1]@shape", "0.00,a 100.00,-4.95"],
    ["net/edge[0]/lane[0]@shape", "0.00,-4.95 a,-4.95"],
    ["net/edge[0]/lane[1]@shape", "0.00,-4.95 a,-4.95"],
    ["net/edge[1]/lane[0]@shape", "0.00,-4.95 a,-4.95"],
    ["net/edge[1]/lane[1]@shape", "0.00,-4.95 a,-4.95"],
    ["net/edge[0]/lane[0]@shape", "0.00,-4.95 a 100.00,-4.95"],
    ["net/edge[0]/lane[1]@shape", "0.00,-4.95 a 100.00,-4.95"],
    ["net/edge[1]/lane[0]@shape", "0.00,-4.95 a 100.00,-4.95"],
    ["net/edge[1]/lane[1]@shape", "0.00,-4.95 a 100.00,-4.95"],
    ["net/edge[0]/lane[0]@shape", "100.00,-4.95"],
    ["net/edge[0]/lane[1]@shape", "0100.00,-4.95"],
    ["net/edge[1]/lane[0]@shape", "100.00,-4.95"],
    ["net/edge[1]/lane[1]@shape", "100.00,-4.95"],
    ["net/edge[0]/lane[0]", "<remove>"],
    ["net/edge[0]/lane[1]", "<remove>"],
    ["net/edge[1]/lane[0]", "<remove>"],
    ["net/edge[1]/lane[1]", "<remove>"],
    ["net/edge[0]/lane[0]", "<duplicate>"],
    ["net/edge[0]/lane[1]", "<duplicate>"],
    ["net/edge[1]/lane[0]", "<duplicate>"],
    ["net/edge[1]/lane[1]", "<duplicate>"],

    ["net/junction[0]@id", ""],
    ["net/junction[1]@id", ""],
    ["net/junction[2]@id", ""],
    ["net/junction[0]@id", "a"],
    ["net/junction[1]@id", "a"],
    ["net/junction[2]@id", "a"],
    ["net/junction[0]@id", "<remove>"],
    ["net/junction[1]@id", "<remove>"],
    ["net/junction[2]@id", "<remove>"],
    ["net/junction[0]@type", ""],
    ["net/junction[1]@type", ""],
    ["net/junction[2]@type", ""],
    ["net/junction[0]@type", "a"],
    ["net/junction[1]@type", "a"],
    ["net/junction[2]@type", "a"],
    ["net/junction[0]@type", "<remove>"],
    ["net/junction[1]@type", "<remove>"],
    ["net/junction[2]@type", "<remove>"],
    ["net/junction[1]@incLanes", "a_0 1_1"],
    ["net/junction[1]@incLanes", "1_0 a_1"],
    ["net/junction[2]@incLanes", "a_0 2_1"],
    ["net/junction[2]@incLanes", "2_0 a_1"],
    ["net/junction[0]", "<remove>"],
    ["net/junction[1]", "<remove>"],
    ["net/junction[2]", "<remove>"],
    ["net/junction[0]", "<duplicate>"],
    ["net/junction[1]", "<duplicate>"],
    ["net/junction[2]", "<duplicate>"],

    ["net/connection[0]@from", ""],
    ["net/connection[1]@from", ""],
    ["net/connection[0]@from", "a"],
    ["net/connection[1]@from", "a"],
    ["net/connection[0]@from", "<remove>"],
    ["net/connection[1]@from", "<remove>"],
    ["net/connection[0]@to", ""],
    ["net/connection[1]@to", ""],
    ["net/connection[0]@to", "a"],
    ["net/connection[1]@to", "a"],
    ["net/connection[0]@to", "<remove>"],
    ["net/connection[1]@to", "<remove>"],
    ["net/connection[0]", "<duplicate>"],
    ["net/connection[1]", "<duplicate>"],

]


def tinyPath(xmlStruct, path, newValue):
    attribute = None
    if path.find("@") >= 0:
        path, attribute = path.split("@")
    defs = path.split("/")
    item = xmlStruct
    for d in defs:
        if d.find("[") >= 0:
            iname = d[:d.find("[")]
            index = int(d[d.find("[") + 1:d.find("]")])
        else:
            iname = d
            index = 0
        items = item.getElementsByTagName(iname)
        item = items[index]
    if attribute:
        if newValue == "<remove>":
            item.removeAttribute(attribute)
        else:
            item.setAttribute(attribute, newValue)
    else:
        if newValue == "<remove>":
            item.parentNode.removeChild(item)
        elif newValue == "<duplicate>":
            item.parentNode.insertBefore(item.cloneNode(True), item)
        else:
            print("Unsupported modification defined", file=sys.stderr)


if sys.argv[1] == "sumo":
    call = [checkBinary('sumo'), "--no-step-log", "--no-duration-log"]
elif sys.argv[1] == "netconvert":
    call = [checkBinary('netconvert'), "-o", "/dev/null"]
elif sys.argv[1] == "dfrouter":
    call = [checkBinary('dfrouter'), "--detector-files", "input_additional.add.xml"]
elif sys.argv[1] == "duarouter" or sys.argv[1] == "jtrrouter":
    call = [checkBinary(sys.argv[1]), "--no-step-log",
            "-o", "dummy.xml", "-a", "input_additional.add.xml"]
else:
    print("Unsupported application defined", file=sys.stderr)
call += sys.argv[2:] + ["-n"]
if sys.argv[1] == "netconvert":
    call[-1] = "-s"

netconvertBinary = checkBinary('netconvert')

# build the correct network, first
print(">>> Building the correct network")
retcode = subprocess.call([netconvertBinary, "-c", "netconvert.netccfg"])
print(">>> Trying the correct network")
retcode = subprocess.call(call + ["correct.net.xml"])
if retcode != 0:
    print("Error on processing the 'correct' network!")
    sys.exit()
print(">>> ok...\n")

# check broken network processing
print("Running broken net")
for c in changes:
    tree = dom.parse("correct.net.xml")
    tinyPath(tree, c[0], c[1])
    writer = open('mod.net.xml', 'w')
    tree.writexml(writer)
    writer.close()
    print("------------------ " + c[0] + ":" + c[1], file=sys.stderr)
    sys.stderr.flush()
    retcode = subprocess.call(call + ["mod.net.xml"])
    sys.stderr.flush()
    sys.stdout.flush()
    if retcode != 1:
        print(" Wrong error code returned (%s)!" % retcode, file=sys.stderr)
