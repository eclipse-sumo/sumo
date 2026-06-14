#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    diffMerge.py
# @author  Jakob Erdmann
# @date    2026-06-12

"""
Reads two diff files and flags merge conflicts
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import os
from collections import defaultdict

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa
from sumolib.options import ArgumentParser  # noqa


def parse_args():
    optParser = ArgumentParser()
    optParser.add_option("diffprefix", category="input",
                         help="first diff prefix")
    optParser.add_option("diffprefix2", category="input",
                         help="second diff prefix")
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    options = optParser.parse_args()
    return options


# file types to compare
TYPE_NODES = '.nod.xml'
TYPE_EDGES = '.edg.xml'
PLAIN_TYPES = [
    TYPE_NODES,
    TYPE_EDGES,
]


# run
def main(options):
    for type in PLAIN_TYPES:
        deleted = [set(), set()]
        created = [set(), set()]
        changed = [set(), set()]

        diff = options.diffprefix + type
        diff2 = options.diffprefix2 + type
        if options.verbose:
            print("comparing %s and %s" % (diff, diff2))

        for i, fname in enumerate([diff, diff2]):
            root = list(sumolib.xml.parse(fname, outputLevel=0))[0]
            target = None
            for o in root.getChildList(True):
                if o.isComment():
                    if "Deleted Elements" in o.getText():
                        target = deleted[i]
                    elif "Created Elements" in o.getText():
                        target = created[i]
                    elif "Changed Elements" in o.getText():
                        target = changed[i]
                else:
                    target.add(o.id)

        allIDs = [set.union(*[deleted[i], created[i], changed[i]]) for i in [0, 1]]
        print("%s id clashes: %s" % (type, ' '.join(allIDs[0] & allIDs[1]))) 


if __name__ == "__main__":
    main(parse_args())
