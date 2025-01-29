#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    poly2edgedata.py
# @author  Jakob Erdmann
# @date    2025-01-23

"""
Transform polygons with params into edgedata with attributes
For each polygon a unique edge is selected that gives the best geometrical match

The following syntax is supported in the patch file (one patch per line):

# lines starting with '#' are ignored as comments
# rev overrides the reverse edge of EDGEID to be REVEDGEID
rev EDGEID REVEDGEID
# edg overrides the edge to assign for POLYID to be EDGEID
edg POLYID EDGEID
# dat overrrides the data attribute ATTR for POLYID to take on value VALUE
dat POLYID ATTR VALUE

any ID or VALUE may bet set to 'None' to signify that
- a reverse edge should not be assigned
- a polygon should not be mapped
- data should be ignored
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from math import fabs, degrees
from collections import defaultdict
THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, '..'))
import sumolib  # noqa
import sumolib.geomhelper as gh  # noqa

PATCH_NONE = 'None'


def get_options(args=None):
    op = sumolib.options.ArgumentParser(description="Transform polygons with params into edgedata with attributes")

    op.add_option("polyfiles", nargs='+', category="input", type=op.file_list,
                  help="List of polygon files to convert")
    op.add_option("-n", "--netfile", category="input", required=True, type=op.net_file,
                  help="Network file on which to map the polygons")
    op.add_option("-p", "--patchfile", category="input",  type=op.file,
                  help="Load a file with patches to apply during import")
    op.add_option("-r", "--radius", type=float, default=20,
                  help="radius for finding edges near polygons")
    op.add_option("--shapecut", type=float, default=40,
                  help="Shorten polygon and edge shapes to FLOAT to increase robustness of angular comparison")
    op.add_option("--angle-tolerance", type=float, default=20, dest="atol",
                  help="Match polygons and edges if their angle differs by no more than DEGRESS")
    op.add_option("--min-length", type=float, default=10, dest="minLength",
                  help="minimum edge length that may be mapped to")
    op.add_option("-s", "--split-attributes", dest="splitAttrs",
                  help="If a reverse edge is found, split the values of the given attribute list among edge and reverse edge")  # noqa
    op.add_option("-S", "--nosplit-attributes", dest="noSplitAttrs",
                  help="If a reverse edge is found, split the values of all attributes except the given attribute list among edge and reverse edge")  # noqa
    op.add_option("-f", "--filter", dest="filter",
                  help="Read a list of triplets ATTR,MIN,MAX and only keep polygons where value ATTR is within [MIN,MAX]")  # noqa
    op.add_option("-b", "--begin", default=0, type=op.time,
                  help="edgedata interval begin time")
    op.add_option("-e", "--end", default="1:0:0:0", type=op.time,
                  help="edgedata interval end time)")
    op.add_option("-o", "--output-file", category="output", dest="outfile", required=True, type=op.file,
                  help="output file")

    try:
        options = op.parse_args()
    except (NotImplementedError, ValueError) as e:
        print(e, file=sys.stderr)
        sys.exit(1)

    options.splitAttrs = set(options.splitAttrs.split(',')) if options.splitAttrs else []
    options.noSplitAttrs = set(options.noSplitAttrs.split(',')) if options.noSplitAttrs else []
    tuples = options.filter.split(',') if options.filter else []
    options.filter = {}  # attr -> (min, max)
    for i in range(0, len(tuples), 3):
        options.filter[tuples[i]] = (float(tuples[i + 1]), float(tuples[i + 2]))
    return options


def hasReverse(edge):
    for cand in edge.getToNode().getOutgoing():
        if cand.getToNode() == edge.getFromNode():
            return True
    return False


def readPatches(net, pfile):
    patchEdg = {}  # polyID->edge
    patchRev = {}  # forwardEdge->reverseEdge
    patchDat = defaultdict(lambda: {})  # polyID->attr->data (-1 ignores)
    if pfile is not None:
        with open(pfile) as pf:
            for line in pf:
                items = line.split()
                patchtype = items[0]
                if patchtype == "rev":
                    edgeID, reverseID = items[1:]
                    patchRev[edgeID] = reverseID
                    net.getEdge(edgeID)  # provoke error if edgeID does not exist in net
                elif patchtype == "edg":
                    polyID, edgeID = items[1:]
                    patchEdg[polyID] = edgeID
                elif patchtype == "dat":
                    polyID, attrName, value = items[1:]
                    patchDat[polyID][attrName] = value
                elif patchtype == "#":
                    # comment
                    continue
                else:
                    print("unknown patchtype '%s'" % patchtype)

    return patchEdg, patchRev, patchDat


def main(options):
    net = sumolib.net.readNet(options.netfile)
    patchEdg, patchRev, patchDat = readPatches(net, options.patchfile)
    usedEdges = set()  # do not assign different polygons/counts to the same edge
    scut = options.shapecut

    with open(options.outfile, 'w') as foutobj:
        foutobj.write('<meandata>\n')
        foutobj.write('    <interval begin="%s" end="%s" id="%s">\n' % (
            options.begin, options.end, options.polyfiles[0]))
        for fname in options.polyfiles:
            for poly in sumolib.xml.parse(fname, 'poly'):

                shape = []
                for lonlat in poly.shape.split():
                    lon, lat = lonlat.split(',')
                    shape.append(net.convertLonLat2XY(float(lon), float(lat)))
                shapelen = gh.polyLength(shape)
                cx, cy = gh.positionAtShapeOffset(shape, shapelen / 2)
                edges = net.getNeighboringEdges(cx, cy, options.radius)
                if not edges:
                    print("No edges near %.2f,%.2f (poly %s)" % (cx, cy, poly.id), file=sys.stderr)
                    continue
                edges = [(e, d) for e, d in edges if e.allows("passenger")]
                if not edges:
                    print("No car edges near %.2f,%.2f (poly %s)" % (cx, cy, poly.id), file=sys.stderr)
                    continue
                edges = [(e, d) for e, d in edges if e.getLength() >= options.minLength]
                if not edges:
                    print("No long edges near %.2f,%.2f (poly %s)" % (cx, cy, poly.id), file=sys.stderr)
                    continue

                if shapelen < scut:
                    polyAngle = gh.angleTo2D(shape[0], shape[-1])
                else:
                    polyAngle = gh.angleTo2D(gh.positionAtShapeOffset(shape, shapelen / 2 - scut / 2),
                                             gh.positionAtShapeOffset(shape, shapelen / 2 + scut / 2))

                cands = []
                for e, d in edges:
                    if e.getLength() < scut:
                        angle = gh.angleTo2D(e.getFromNode().getCoord(), e.getToNode().getCoord())
                        revAngle = gh.angleTo2D(e.getToNode().getCoord(), e.getFromNode().getCoord())
                    else:
                        eShape = e.getShape()
                        offset = gh.polygonOffsetWithMinimumDistanceToPoint((cx, cy), eShape)
                        offset1 = max(0, offset - scut / 2)
                        offset2 = min(gh.polyLength(eShape), offset + scut / 2)
                        angle = gh.angleTo2D(gh.positionAtShapeOffset(eShape, offset1),
                                             gh.positionAtShapeOffset(eShape, offset2))
                        revAngle = gh.angleTo2D(gh.positionAtShapeOffset(eShape, offset2),
                                                gh.positionAtShapeOffset(eShape, offset1))
                    if ((degrees(fabs(polyAngle - angle)) < options.atol or
                         degrees(fabs(polyAngle - revAngle)) < options.atol)):
                        cands.append(e)
                edges = cands
                if not edges:
                    print("No edges with angle %.2f found near %.2f,%.2f (poly %s)" % (
                        degrees(polyAngle), cx, cy, poly.id), file=sys.stderr)
                    continue

                bestDist = 1e10
                bestEdge = None
                bestReverse = None
                for e in edges:
                    if shapelen > e.getLength():
                        maxDist = max([gh.distancePointToPolygon(xy, shape) for xy in e.getShape()])
                    else:
                        maxDist = max([gh.distancePointToPolygon(xy, e.getShape()) for xy in shape])
                    if maxDist < bestDist:
                        bestDist = maxDist
                        bestEdge = e
                # apply edge patch
                if poly.id in patchEdg:
                    if patchEdg[poly.id] == PATCH_NONE:
                        continue
                    bestEdge = net.getEdge(patchEdg[poly.id])

                if bestEdge in usedEdges:
                    patchInfo = " (was patched)" if poly.id in patchEdg else ""
                    print("Duplicate assignment to edge %s from poly %s%s" % (
                        bestEdge.getID(), poly.id, patchInfo),  file=sys.stderr)
                    continue
                # find opposite direction for undivided road
                for e in edges:
                    if e.getFromNode() == bestEdge.getToNode() and e.getToNode() == bestEdge.getFromNode():
                        bestReverse = e
                        break
                # apply revers edge patch
                if bestEdge.getID() in patchRev:
                    revID = patchRev.get(bestEdge.getID())
                    if revID != PATCH_NONE:
                        bestReverse = net.getEdge(revID)
                    else:
                        bestReverse = None
                # find opposite direction for divided road
                elif bestReverse is None:
                    bestDist = 1e10
                    bestAngle = gh.angleTo2D(bestEdge.getFromNode().getCoord(), bestEdge.getToNode().getCoord())
                    for e in edges:
                        if hasReverse(e):
                            continue
                        reverseAngle = gh.angleTo2D(e.getToNode().getCoord(), e.getFromNode().getCoord())
                        if degrees(fabs(bestAngle - reverseAngle)) < 20:
                            maxDist = max([gh.distancePointToPolygon(xy, bestEdge.getShape()) for xy in e.getShape()])
                            if maxDist < bestDist:
                                bestDist = maxDist
                                bestReverse = e

                if bestReverse in usedEdges:
                    patchInfo = " (was patched)" if bestEdge.getID() in patchRev else ""
                    print("Duplicate assignment to reverse edge %s from poly %s%s" % (
                        bestReverse.getID(), poly.id, patchInfo),  file=sys.stderr)
                    continue

                attrs = 'polyID="%s"' % poly.id
                skip = False
                if poly.param:
                    for param in poly.param:
                        value = param.value
                        if poly.id in patchDat:
                            if param.key in patchDat[poly.id]:
                                value = patchDat[poly.id][param.key]
                                # print("patched %s to %s" % (param.value, value))
                                if value == PATCH_NONE:
                                    continue
                        if param.key in options.filter:
                            if ((float(value) < options.filter[param.key][0] or
                                 float(value) > options.filter[param.key][1])):
                                skip = True
                        if (bestReverse is not None
                                and (param.key in options.splitAttrs
                                     or options.noSplitAttrs and param.key not in options.noSplitAttrs)):
                            try:
                                value = float(value) / 2
                            except ValueError:
                                pass
                        attrs += ' %s="%s"' % (param.key, value)

                if skip:
                    continue
                comment = ''
                if bestReverse is not None:
                    usedEdges.add(bestReverse)
                    comment = '  <!-- reverse: %s -->' % bestEdge.getID()
                    foutobj.write('        <edge id="%s" %s/> %s\n' % (bestReverse.getID(), attrs, comment))
                    comment = '  <!-- reverse: %s -->' % bestReverse.getID()
                usedEdges.add(bestEdge)
                foutobj.write('        <edge id="%s" %s/> %s\n' % (bestEdge.getID(), attrs, comment))
            foutobj.write('    </interval>\n')
            foutobj.write('</meandata>\n')


if __name__ == "__main__":
    main(get_options())
