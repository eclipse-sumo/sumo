"""
@file    dumo.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2011-06-15
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import

import collections
from xml.sax import parse, handler


class DumpReader(handler.ContentHandler):

    def __init__(self, attrsToCollect, edgesToCollect=None):
        self._values = collections.defaultdict(list)
        if isinstance(attrsToCollect, dict):
            self._attrsToCollect = attrsToCollect
        else:
            self._attrsToCollect = {}
            for a in attrsToCollect:
                self._attrsToCollect[a] = a
        if edgesToCollect is None or isinstance(edgesToCollect, dict):
            self._edgesToCollect = edgesToCollect
        else:
            self._edgesToCollect = {}
            for e in edgesToCollect:
                self._edgesToCollect[e] = e
        self._intervalBegins = []
        self._beginTime = None

    def startElement(self, name, attrs):
        if name == 'interval':
            self._beginTime = float(attrs['begin'])
            self._intervalBegins.append(self._beginTime)
            for a in self._attrsToCollect.itervalues():
                self._values[a].append(collections.defaultdict(int))
        if name == 'edge' or name == 'lane':
            id = attrs['id']
            if self._edgesToCollect is not None:
                if id in self._edgesToCollect:
                    id = self._edgesToCollect[id]
                else:
                    return
            for a in attrs.keys():
                if a not in self._attrsToCollect:
                    continue
                self._values[
                    self._attrsToCollect[a]][-1][id] += float(attrs[a])

    def join(self, what, how):
        for a in what:
            self._singleJoin(a, how)

    def get(self, what):
        return self._values[what]

    def getIntervalStarts(self):
        return self._intervalBegins

    def _singleJoin(self, what, how):
        ret = {}
        no = {}
        for i in self._values[what]:
            for e in i:
                if e not in ret:
                    ret[e] = 0
                    no[e] = 0
                ret[e] = ret[e] + i[e]
                no[e] = no[e] + 1
        if how == "sum":
            return ret
        elif how == "average":
            for e in i:
                ret[e] = ret[e] / float(no[e])
        self._values[what] = [ret]


def readDump(file, attrsToCollect, edgesToCollect=None):
    dump = DumpReader(attrsToCollect, edgesToCollect)
    parse(file, dump)
    return dump
