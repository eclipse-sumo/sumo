#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    tripinfoAddSpeed.py
# @author  Mark Colley
# @date    2026-11-22

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from lxml import etree as ET

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
else:
    sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import sumolib  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Add average speed attribute to tripinfo XML elements")
    ap.add_argument("-t", "--tripinfo-file", dest="tripinfo_file", category="input", required=True,
                    type=ap.file, help="Input tripinfo XML file")
    ap.add_argument("-o", "--output", category="output", required=True,
                    type=ap.file, help="Output tripinfo XML file with speed attribute")
    ap.add_argument("--precision", type=int, default=2,
                    help="Decimal precision for computed speed in m/s")
    return ap.parse_args(args=args)


def main(options):
    with sumolib.openz(options.tripinfo_file, "rb") as inf:
        tree = ET.parse(inf)
    fmt = "%%.%df" % options.precision
    for elem in tree.getroot().iter():
        if elem.tag in ("tripinfo", "walk", "ride"):
            length = elem.get("routeLength")
            duration = elem.get("duration")
            if length is not None and duration is not None:
                dur_val = float(duration)
                speed = float(length) / dur_val if dur_val > 0 else 0.0
                elem.set("speed", fmt % speed)
    with sumolib.openz(options.output, "wb") as outf:
        tree.write(outf, xml_declaration=True, encoding="UTF-8")


if __name__ == "__main__":
    main(get_options())
