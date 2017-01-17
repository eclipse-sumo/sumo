"""
@file    selection.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2013-05-06
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""


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
        fdo.write("%s:%s" % (typeName, entries))
