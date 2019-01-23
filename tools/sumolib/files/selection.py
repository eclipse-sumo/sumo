# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    selection.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2013-05-06
# @version $Id$


def read(file, lanes2edges=True):
    ret = {}
    fd = open(file)
    for line in fd:
        vals = line.strip().split(":")
        if lanes2edges and vals[0] == "lane":
            vals[0] = "edge"
            vals[1] = vals[1][:vals[1].rfind("_")]
        if vals[0] not in ret:
            ret[vals[0]] = set()
        ret[vals[0]].add(vals[1])
    fd.close()
    return ret


def write(fdo, entries):
    for t in entries:
        writeTyped(fdo, t, entries[t])


def writeTyped(fdo, typeName, entries):
    for e in entries:
        fdo.write("%s:%s\n" % (typeName, e))
