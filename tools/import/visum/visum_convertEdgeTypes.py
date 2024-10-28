#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    visum_convertEdgeTypes.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2009-05-27

"""

This script converts edge type definitions (STRECKENTYP) into their
 SUMO-representation.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
# MAIN
if __name__ == '__main__':
    op = sumolib.options.ArgumentParser(description="Convert VISUM edge type defintions into sumo edge type")
    op.add_option("visumNet", category="input", type=op.file, help="the VISUM network file")
    op.add_option("output", category="output", type=op.file, help="the output xml file")
    options = op.parse_args()

    print("Reading VISUM...")
    fd = sumolib.miscutils.openz(options.visumNet, encoding='latin1')
    fdo = sumolib.miscutils.openz(options.output, 'w', encoding='utf8')
    sumolib.writeXMLHeader(fdo, "$Id$", "types", options=options)
    parsingTypes = False
    attributes = []
    for line in fd:
        if parsingTypes:
            if line[0] == '*' or line[0] == '$' or line.find(";") < 0:
                parsingTypes = False
                continue

            values = line.strip().split(";")
            map = {}
            for i in range(0, len(attributes)):
                map[attributes[i]] = values[i]
            fdo.write('   <type id="' + map["nr"])
            fdo.write('" priority="' + str(100 - int(map["rang"])))
            fdo.write('" numLanes="' + map["anzfahrstreifen"])
            fdo.write('" speed="' + str(float(map["v0iv"].replace("km/h", "")) / 3.6))
            fdo.write('"/>\n')

        if line.find("$STRECKENTYP") == 0:
            parsingTypes = True
            attributes = line[len("$STRECKENTYP:"):].lower().split(";")
    fdo.write("</types>\n")
    fdo.close()
    fd.close()
