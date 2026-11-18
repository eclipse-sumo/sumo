#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    remap_attributes.py
# @author  Jakob Erdmann
# @date    2026-02-26

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import subprocess

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import sumolib  # noqa
from sumolib import geomhelper  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Remap edge attributes from one network to another")
    ap.add_argument("--orig-net", dest="origNet", required=True, category="input", type=ap.net_file,
                    help="SUMO network for reading attributes", metavar="FILE")
    ap.add_argument("--target-net", dest="targetNet", required=True, category="input", type=ap.net_file,
                    help="SUMO network for receiving attributes", metavar="FILE")
    ap.add_argument("-o", "--output-file", dest="output", required=True, category="output", type=ap.net_file,
                    help="File for writing the patched network", metavar="FILE")
    ap.add_argument("-p", "--patch-file-prefix", dest="patchPrefix", category="output", default="patch",
                    help="prefix for patch files")
    ap.add_argument("-a", "--attributes", required=True,
                    help="the list of edge attributes that shall be transferred")
    gp = ap.add_mutually_exclusive_group(required=False)
    gp.add_argument("--osm.origid", action="store_true", dest="osmOrigId", default=False,
                    help="match objects based on OSM ids (stored in params from --output.original-names)")
    gp.add_argument("--origid", action="store_true", dest="origId", default=False,
                    help="match objects based on origIDs after renaming")
    ap.add_argument("--radius", type=float, default=20.0,
                    help="search radius in m for geometrical matching")
    ap.add_argument("--angle-tolerance", dest="atol", type=float, default=20.0,
                    help="maximum angle difference in degrees for geometrical matching")
    ap.add_argument("-v", "--verbose", action="store_true", dest="verbose",
                    default=False, help="tell me what you are doing")

    options = ap.parse_args(args=args)

    options.attributes = options.attributes.split(',')
    options.edgfile = options.patchPrefix + ".edg.xml"
    return options


class Ambiguous:
    pass


# override canonical method name if defaults if needed
RETRIEVERS = {
    'numLanes': 'getLaneNumber',
}


def getAttr(edge, a):
    method = RETRIEVERS.get(a, 'get' + a[0].upper() + a[1:])
    return getattr(edge, method)()


def getAttrs(options, edge):
    return [getAttr(edge, a) for a in options.attributes]


def map_point_to_orig(net, net2, pt):
    if net.hasGeoProj() and net2.hasGeoProj():
        lon, lat = net2.convertXY2LonLat(pt[0], pt[1])
        return net.convertLonLat2XY(lon, lat)
    off1 = net.getLocationOffset()
    off2 = net2.getLocationOffset()
    return (pt[0] - off2[0] + off1[0], pt[1] - off2[1] + off1[1])


def match_edge_geometrically(net, net2, edge2, radius, atol):
    shape2 = edge2.getShape()
    length2 = geomhelper.polyLength(shape2)
    if length2 <= 0:
        return []
    fracs = [0.5] if length2 < 20.0 else [0.25, 0.5, 0.75]
    matched = []
    for frac in fracs:
        off2 = length2 * frac
        pt2 = geomhelper.positionAtShapeOffset(shape2, off2)
        rot2 = geomhelper.rotationAtShapeOffset(shape2, min(off2, max(0.0, length2 - 1e-3)))
        if pt2 is None or rot2 is None:
            continue
        deg2 = geomhelper.naviDegree(rot2)
        pt1 = map_point_to_orig(net, net2, pt2)
        best = None
        for cand, _ in net.getNeighboringEdges(pt1[0], pt1[1], radius):
            cshape = cand.getShape()
            clen = geomhelper.polyLength(cshape)
            if clen <= 0:
                continue
            coff, cdist = geomhelper.polygonOffsetAndDistanceToPoint(pt1, cshape)
            if cdist > radius:
                continue
            crot = geomhelper.rotationAtShapeOffset(cshape, min(max(0.0, coff), max(0.0, clen - 1e-3)))
            if crot is None:
                continue
            cdeg = geomhelper.naviDegree(crot)
            if geomhelper.minAngleDegreeDiff(cdeg, deg2) > atol:
                continue
            key = (cdist, cand.getID())
            if best is None or key < best[0]:
                best = (key, cand.getID())
        if best is not None and best[1] not in matched:
            matched.append(best[1])
    return matched


def main(options):
    if options.verbose:
        print("Reading orig-net '%s'" % options.origNet)
    net = sumolib.net.readNet(options.origNet)
    if options.verbose:
        print("Reading target-net '%s'" % options.targetNet)
    net2 = sumolib.net.readNet(options.targetNet)

    lookup = {}  # key -> values
    if options.osmOrigId:
        # lookup is based on edge-origIds in orig-net
        for edge in net.getEdges():
            lane = edge.getLanes()[0]
            origIds = lane.getParam("origId") or edge.getParam("origId")
            if origIds is None:
                continue
            origIds = origIds.split()
            attrs = getAttrs(options, edge)
            for origId in origIds:
                attrs2 = lookup.get(origId)
                if attrs2 is None:
                    lookup[origId] = attrs
                else:
                    attrs3 = []
                    for a, v, v2 in zip(options.attributes, attrs, attrs2):
                        if v == v2 or v2 is None:
                            attrs3.append(v)
                        elif v is None:
                            attrs3.append(v2)
                        else:
                            print("Ambiguous attribute '%s' for origId '%s' (%s != %s)" % (
                                a, origId, v, v2), file=sys.stderr)
                            attrs3.append(Ambiguous)
                    lookup[origId] = attrs3
    else:
        # lookup is based on edge ids in orig-net (used by --origid and geometrical matching)
        for edge in net.getEdges():
            attrs = getAttrs(options, edge)
            lookup[edge.getID()] = attrs

    patchedValues = {}  # edgeID -> new attributes
    for edge in net2.getEdges():
        if options.osmOrigId or options.origId:
            lane = edge.getLanes()[0]
            origIds = lane.getParam("origId") or edge.getParam("origId")
            if origIds is None:
                continue
            origIds = origIds.split()
        else:
            origIds = match_edge_geometrically(net, net2, edge, options.radius, options.atol)
            if not origIds:
                continue
        attrs = getAttrs(options, edge)
        attrs2 = []
        for i, a in enumerate(options.attributes):
            v = None
            for origId in origIds:
                if origId in lookup:
                    v2 = lookup[origId][i]
                    if v2 == Ambiguous:
                        v = None
                        break
                    if v is None:
                        v = v2
                    elif v2 is not None and v != v2:
                        print("Ambiguous attribute '%s' for edge '%s' with origIds '%s' (%s != %s)" % (
                            a, edge.getID(), origIds, v, v2), file=sys.stderr)
                        v = None
                        break
            if v == attrs[i]:
                # no need to patch if the attribute is already correct
                v = None
            attrs2.append(v)
        if any([v is not None for v in attrs2]):
            patchedValues[edge.getID()] = attrs2

    with sumolib.openz(options.edgfile, 'w') as fout:
        sumolib.writeXMLHeader(fout, "$Id$", "edges", schemaPath="edgediff_file.xsd", options=options)
        for edgeID in sorted(patchedValues.keys()):
            validAttrs = [(a, v) for a, v in zip(options.attributes, patchedValues[edgeID]) if v is not None]
            fout.write('    <edge id="%s" %s/>\n' % (
                edgeID, ' '.join(['%s=%s' % (a, sumolib.xml.quoteattr(str(v))) for a, v in validAttrs])))
        fout.write('</edges>\n')

    NETCONVERT = sumolib.checkBinary('netconvert')
    if subprocess.call([NETCONVERT,
                        '-s', options.targetNet,
                        '-e', options.edgfile,
                        '-o', options.output]) != 0:
        sys.exit(1)


if __name__ == "__main__":
    main(get_options())
