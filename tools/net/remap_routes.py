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

# @file    remap_routes.py
# @author  Jakob Erdmann
# @date    2026-11-19

"""
Remap route and trip files from one SUMO network to another based on geometry matching.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from statistics import median

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(THIS_DIR)
sys.path.append(os.path.join(THIS_DIR, ".."))
import sumolib  # noqa
from sumolib import geomhelper as gh  # noqa
from sumolib.miscutils import openz  # noqa
from sumolib.xml import parse  # noqa
import remap_additionals  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Remap routes and trips from one network to another via geometry")
    ap.add_argument("--orig-net", dest="origNet", required=True, category="input", type=ap.net_file,
                    help="SUMO network on which input routes are defined", metavar="FILE")
    ap.add_argument("--target-net", dest="targetNet", required=True, category="input", type=ap.net_file,
                    help="SUMO network onto which routes shall be mapped", metavar="FILE")
    ap.add_argument("-r", "--route-file", dest="routes", required=True, category="input", type=ap.route_file,
                    help="Input route/trip XML file", metavar="FILE")
    ap.add_argument("-o", "--output-file", dest="output", required=True, category="output", type=ap.route_file,
                    help="Output route/trip XML file", metavar="FILE")
    ap.add_argument("--radius", type=float, default=20.0,
                    help="Search radius in meters for candidate target edges")
    ap.add_argument("--shapecut", type=float, default=40.0,
                    help="Shorten edge shapes to FLOAT for local angular comparison")
    ap.add_argument("--angle-tolerance", type=float, default=20.0, dest="atol",
                    help="Maximum angle difference in degrees for matching edges")
    ap.add_argument("--no-fill-gaps", action="store_false", dest="fillGaps", default=True,
                    help="Do not fill gaps between non-adjacent mapped route edges via shortest path")
    ap.add_argument("-v", "--verbose", action="store_true", dest="verbose", default=False,
                    help="Print progress and summary")
    return ap.parse_args(args=args)


def _parse_float_pos(val, edge_len):
    if val is None:
        return None
    try:
        f = float(val)
        return (edge_len + f) if f < 0 else f
    except ValueError:
        return None


def map_edge_wrap_safe(options, obj, edge_id, pos=None, vclass=None):
    if not options.net.hasEdge(edge_id):
        print("Unknown edge '%s' on %s" % (edge_id, obj.name), file=sys.stderr)
        return None, None
    edge = options.net.getEdge(edge_id)
    shape = edge.getShape()
    shapelen = gh.polyLength(shape)
    if shapelen <= 0:
        return None, None
    relpos = (pos / edge.getLength()) if (pos is not None and edge.getLength() > 0) else 0.5
    relpos = min(1.0, max(0.0, relpos + (1.0 if relpos < 0 else 0.0)))
    s_off = shapelen * relpos
    x, y = gh.positionAtShapeOffset(shape, s_off)
    x2, y2 = options.remap_xy((x, y))
    neighbors = options.net2.getNeighboringEdges(x2, y2, options.radius)
    if not neighbors:
        print("No edges near %.2f,%.2f (origEdge %s)" % (x2, y2, edge_id), file=sys.stderr)
        return None, None

    rot_orig = gh.rotationAtShapeOffset(shape, min(max(0.0, s_off), max(0.0, shapelen - 1e-3)))
    deg_orig = gh.naviDegree(rot_orig)
    cands = []
    for cand, _ in neighbors:
        cshape = cand.getShape()
        clen = gh.polyLength(cshape)
        if clen <= 0:
            continue
        coff = gh.polygonOffsetWithMinimumDistanceToPoint((x2, y2), cshape)
        crot = gh.rotationAtShapeOffset(cshape, min(max(0.0, coff), max(0.0, clen - 1e-3)))
        if crot is not None and gh.minAngleDegreeDiff(deg_orig, gh.naviDegree(crot)) <= options.atol:
            cands.append(cand)
    if not cands:
        print("No edges with matching angle near %.2f,%.2f (origEdge %s)" % (x2, y2, edge_id), file=sys.stderr)
        return None, None

    if vclass:
        vc_cands = [e for e in cands if e.allows(vclass)]
        if vc_cands:
            cands = vc_cands
    else:
        perms = set(edge.getPermissions())
        perm_cands = [e for e in cands if perms.issubset(e.getPermissions())]
        if perm_cands:
            cands = perm_cands

    shape2 = [options.remap_xy(pt) for pt in shape]
    best_edge = None
    best_key = None
    for cand in cands:
        cshape = cand.getShape()
        if shapelen > cand.getLength():
            dist_med = median([gh.distancePointToPolygon(xy, shape2) for xy in cshape])
        else:
            dist_med = median([gh.distancePointToPolygon(xy, cshape) for xy in shape2])
        key = (dist_med, cand.getID())
        if best_key is None or key < best_key:
            best_key = key
            best_edge = cand

    pos2 = None
    if pos is not None and best_edge is not None:
        bshape = best_edge.getShape()
        blen = gh.polyLength(bshape)
        pos2 = (gh.polygonOffsetWithMinimumDistanceToPoint((x2, y2), bshape) / blen * best_edge.getLength()
                if blen > 0 else 0.0)
    return best_edge, pos2


def remap_edge_sequence(options, obj, edge_ids, fill_gaps=True, vclass=None):
    if not edge_ids:
        return None
    mapped_edges = []
    n = len(edge_ids)
    for idx, eid in enumerate(edge_ids):
        if not options.net.hasEdge(eid):
            print("Unknown edge '%s' on %s" % (eid, obj.name), file=sys.stderr)
            return None
        orig_len = options.net.getEdge(eid).getLength()
        sample_fracs = [0.5]
        if n == 1:
            sample_fracs = [0.15, 0.85] if orig_len > 20.0 else [0.5]
        elif idx == 0 and orig_len > 20.0:
            sample_fracs = [0.15, 0.5]
        elif idx == n - 1 and orig_len > 20.0:
            sample_fracs = [0.5, 0.85]
        for frac in sample_fracs:
            e2, _ = map_edge_wrap_safe(options, obj, eid, pos=orig_len * frac, vclass=vclass)
            if e2 is None:
                return None
            if not mapped_edges or mapped_edges[-1] != e2:
                mapped_edges.append(e2)

    if not fill_gaps or len(mapped_edges) <= 1:
        return [e.getID() for e in mapped_edges]

    connected = [mapped_edges[0]]
    for nxt in mapped_edges[1:]:
        prev = connected[-1]
        if prev == nxt:
            continue
        if nxt not in prev.getOutgoing():
            path, _ = options.net2.getShortestPath(prev, nxt, vClass=vclass)
            if path:
                for mid in path[1:-1]:
                    if connected[-1] != mid:
                        connected.append(mid)
            else:
                print("Warning: Disconnected mapped edges '%s' -> '%s' on %s" % (
                    prev.getID(), nxt.getID(), obj.name), file=sys.stderr)
        connected.append(nxt)
    return [e.getID() for e in connected]


def remap_stop(options, stop_obj, vclass=None):
    pos_attrs = remap_additionals.getPosAttrs(stop_obj)
    pos = None
    size = None
    if len(pos_attrs) == 1:
        pos = pos_attrs[0][1]
        stop_obj.setAttribute("friendlyPos", "true")
    elif len(pos_attrs) > 1:
        vals = [av[1] for av in pos_attrs]
        pos = sum(vals) / float(len(vals))
        size = max(vals) - min(vals)
        stop_obj.setAttribute("friendlyPos", "true")

    if stop_obj.hasAttribute("lane"):
        if not options.net.hasEdge(sumolib.net.lane2edge(stop_obj.lane)):
            return False
        lane2, pos2 = remap_additionals.remap_lane(options, stop_obj, stop_obj.lane, pos, size)
        if not lane2:
            return False
        stop_obj.setAttribute("lane", lane2)
        if pos is not None and pos2 is not None:
            for attr_name, orig_val in pos_attrs:
                stop_obj.setAttribute(attr_name, "%.2f" % max(0.0, pos2 + orig_val - pos))
        return True
    if stop_obj.hasAttribute("edge"):
        e2, pos2 = map_edge_wrap_safe(options, stop_obj, stop_obj.edge, pos=pos, vclass=vclass)
        if e2 is None:
            return False
        stop_obj.setAttribute("edge", e2.getID())
        if pos is not None and pos2 is not None:
            for attr_name, orig_val in pos_attrs:
                stop_obj.setAttribute(attr_name, "%.2f" % max(0.0, pos2 + orig_val - pos))
        return True
    return True


def remap_element(options, obj, vtypes, inherited_vclass=None):
    if obj.name in ("vType", "vTypeDistribution"):
        if obj.name == "vType" and obj.hasAttribute("id"):
            vtypes[obj.id] = obj.getAttributeSecure("vClass", "passenger")
        return True

    vclass = inherited_vclass
    if obj.name in ("person", "personFlow", "walk", "personTrip"):
        vclass = "pedestrian"
    elif obj.hasAttribute("type"):
        vclass = vtypes.get(obj.type, inherited_vclass)

    if obj.name == "stop":
        return remap_stop(options, obj, vclass=vclass)

    endpoint_specs = (("from", "departPos", 0.15), ("to", "arrivalPos", 0.85), ("edge", "pos", 0.5))
    for attr_edge, attr_pos, default_frac in endpoint_specs:
        # Note 'from' may be stored as attr_from by sumolib compound_object keyword prefixing
        has_attr = obj.hasAttribute(attr_edge) or (attr_edge == "from" and obj.hasAttribute("attr_from"))
        if has_attr:
            attr_key = "attr_from" if (attr_edge == "from" and obj.hasAttribute("attr_from")) else attr_edge
            orig_eid = obj.getAttribute(attr_key)
            if not options.net.hasEdge(orig_eid):
                print("Could not map %s on %s '%s'" % (obj.name, attr_edge, orig_eid), file=sys.stderr)
                return False
            orig_len = options.net.getEdge(orig_eid).getLength()
            num_pos = _parse_float_pos(obj.getAttributeSecure(attr_pos), orig_len)
            sample_pos = num_pos if num_pos is not None else (orig_len * default_frac)
            e2, pos2 = map_edge_wrap_safe(options, obj, orig_eid, pos=sample_pos, vclass=vclass)
            if e2 is None:
                print("Could not map %s on %s '%s'" % (obj.name, attr_edge, orig_eid), file=sys.stderr)
                return False
            target_key = "attr_from" if (attr_edge == "from" and obj.hasAttribute("attr_from")) else attr_edge
            obj.setAttribute(target_key, e2.getID())
            if num_pos is not None and pos2 is not None:
                obj.setAttribute(attr_pos, "%.2f" % max(0.0, min(e2.getLength(), pos2)))

    if obj.hasAttribute("via"):
        via_mapped = remap_edge_sequence(options, obj, obj.via.split(), fill_gaps=False, vclass=vclass)
        if via_mapped is None:
            return False
        obj.setAttribute("via", " ".join(via_mapped))

    if obj.hasAttribute("edges"):
        mapped_seq = remap_edge_sequence(options, obj, obj.edges.split(), fill_gaps=options.fillGaps, vclass=vclass)
        if mapped_seq is None:
            print("Could not map %s edges '%s'" % (obj.name, obj.edges), file=sys.stderr)
            return False
        obj.setAttribute("edges", " ".join(mapped_seq))

    for child in obj.getChildList():
        if not remap_element(options, child, vtypes, inherited_vclass=vclass):
            if child.name == "stop":
                print("Warning: Commenting out unmappable stop on %s" % obj.getAttributeSecure("id", obj.name),
                      file=sys.stderr)
                child.setCommented()
            else:
                return False
    return True


def main(options):
    if options.verbose:
        print("Reading orig-net '%s'" % options.origNet)
    options.net = sumolib.net.readNet(options.origNet)
    if options.verbose:
        print("Reading target-net '%s'" % options.targetNet)
    options.net2 = sumolib.net.readNet(options.targetNet)

    if options.net.hasGeoProj() and options.net2.hasGeoProj():
        def remap_xy(xy):
            lon, lat = options.net.convertXY2LonLat(*xy)
            return options.net2.convertLonLat2XY(lon, lat)
        options.remap_xy = remap_xy
    else:
        off1 = options.net.getLocationOffset()
        off2 = options.net2.getLocationOffset()
        options.remap_xy = lambda xy: (xy[0] - off1[0] + off2[0], xy[1] - off1[1] + off2[1])

    vtypes = {}
    with openz(options.output, "w") as fout:
        sumolib.writeXMLHeader(fout, "$Id$", "routes", options=options)
        for obj in parse(options.routes):
            if not remap_element(options, obj, vtypes):
                obj.setCommented()
            fout.write(obj.toXML(initialIndent=" " * 4))
        fout.write("</routes>\n")


if __name__ == "__main__":
    main(get_options())
