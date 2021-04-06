#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    debug2sel.py
# @author  Jakob Erdmann
# @date    2021-03-11


"""build (edge) selection file from cmdline id list of edges or lanes
"""
from __future__ import print_function
import sys

outfile = sys.argv[1]
ids = sys.argv[2].split()
lanes = bool(sys.argv[3]) if len(sys.argv) > 3 else False

if lanes:
    ids = ['_'.join(id.split('_')[:-1]) for id in ids]

with open(outfile, 'w') as outf:
    for id in ids:
        outf.write("edge:%s\n" % id)
    print("wrote %s edges to '%s'" % (len(ids), outfile))
