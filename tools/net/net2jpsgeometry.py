#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    net2jpsgeometry.py
# @author  Jakob Erdmann
# @author  Matthias Schwamborn
# @date    2020-11-13

"""
This script converts a sumo network to a JuPedSim geometry
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import ezdxf

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


# defines
KEY_JPS_TYPE = "jps-type"
KEY_TRANSITION_ID = "transition-id"
KEY_FROM_ID = "from-object-id"
KEY_TO_ID = "to-object-id"
KEY_SUMO_ID = "sumo-object-id"

JPS_BOUNDARY_TYPE_ID = "boundary"
JPS_BOUNDARY_ID_SUFFIX = "_boundary"
JPS_BOUNDARY_SLICE_DELIMITER = "_slice_"
JPS_BOUNDARY_COLOR = sumolib.color.RGBAColor(255, 0, 0)
JPS_BOUNDARY_LAYER = 10

JPS_DOOR_TYPE_ID = "door"
JPS_DOOR_ID_DELIMITER = "_door_"
JPS_DOOR_COLOR = sumolib.color.RGBAColor(0, 0, 255)
JPS_DOOR_LAYER = 11

DXF_LAYER_NAME_BOUNDARY = "SUMONetBoundary"
DXF_LAYER_NAME_DOOR = "SUMONetDoors"

DEBUG = True


class DoorInfo:
    def __init__(self,
                 id=None,
                 shape=None,
                 parentPolygon=None,
                 atLengthsOfParent=None):
        assert (len(shape) == 2), "expected two positions for door (got %d instead)" % len(shape)  # noqa
        self._id = id
        self._shape = shape
        self._width = sumolib.geomhelper.polyLength(shape)
        self._parentPolygon = parentPolygon
        self._atLengthsOfParent = atLengthsOfParent

    _id = None
    _shape = None
    _width = 0
    _parentPolygon = None
    _atLengthsOfParent = None


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " -n <net-file> -s <selected-objects-file> <options>"
    argParser = sumolib.options.ArgumentParser(usage=USAGE)
    argParser.add_argument("-n", "--net-file", dest="netFile", help="The .net.xml file to convert")
    # see https://sumo.dlr.de/docs/sumo-gui.html#selecting_objects
    argParser.add_argument("-s", "--selected-objects-file", dest="selectedObjectsFile",
                           help="The txt file including the line-by-line list of objects to select")
    argParser.add_argument("-o", "--output-file", dest="outFile",
                           help="The JuPedSim dxf output file name")
    argParser.add_argument("-a", "--write-additional-file", action="store_true",
                           help="Export boundaries and doors to XML additional file")
    # see https://sumo.dlr.de/docs/Simulation/Pedestrians.html#non-exclusive_sidewalks
    argParser.add_argument("--no-exclusive-sidewalks", action="store_true",
                           help="Choose all lanes allowing pedestrians in case of ambiguities (experimental)")
    argParser.add_argument("--allow-junctions", action="store_true",
                           help="Allow junctions as feasible pedestrian areas for JuPedSim (experimental)")

    options = argParser.parse_args()
    if not options.netFile or not options.selectedObjectsFile:
        print("Missing arguments")
        argParser.print_help()
        exit()
    return options


def isDuplicate(polygon, polygons):
    for p in polygons:
        if p.id == polygon.id:
            return True
    return False


def calculateBoundingPolygon(shape, width):
    shiftPlus = sumolib.geomhelper.move2side(shape, width / 2.0)
    shiftMinus = sumolib.geomhelper.move2side(shape, -width / 2.0)
    polyShape = []
    for x, y in shiftPlus:
        polyShape.append((x, y))
    for x, y in reversed(shiftMinus):
        polyShape.append((x, y))
    polyShape.append(polyShape[0])
    return polyShape


def addInOutLaneToDoorList(polygon, inOutLane, net, doorInfoList, direction='in'):
    assert (direction == 'in' or direction == 'out')
    lane = net.getLane(polygon.attributes[KEY_SUMO_ID])
    if DEBUG:
        print("DEBUG: lane (%s) \'%s\' for current lane \'%s\'" % (direction, inOutLane.getID(), lane.getID()))
    shape = inOutLane.getShape()
    # handle special edges
    if inOutLane.getEdge().isSpecial():
        edgeFunc = inOutLane.getEdge().getFunction()
        if DEBUG:
            print("DEBUG: edge (%s) \'%s\' has special function \'%s\'" % (
                  direction, inOutLane.getEdge().getID(), edgeFunc))
        if edgeFunc == "internal":
            if direction == 'in':
                shape = inOutLane.getConnection(lane).getJunction().getShape()
            else:
                shape = lane.getConnection(inOutLane).getJunction().getShape()
    if DEBUG:
        inOutPolygon = sumolib.shapes.polygon.Polygon(id=inOutLane.getID(),
                                                      color=JPS_BOUNDARY_COLOR,
                                                      layer=JPS_BOUNDARY_LAYER,
                                                      shape=shape)
        print(inOutPolygon.toXML())
    lengths = sumolib.geomhelper.intersectsAtLengths2D(polygon.shape, shape)
    lengths.sort()
    positions = [sumolib.geomhelper.positionAtShapeOffset(polygon.shape, offset) for offset in lengths]
    doorId = polygon.attributes[KEY_SUMO_ID] + JPS_DOOR_ID_DELIMITER + inOutLane.getID()
    doorInfoList.append(DoorInfo(doorId, positions, polygon, lengths))
    if DEBUG:
        print("DEBUG: calculateDoors() p.shape: \'%s\'" % polygon.shape)
        print("DEBUG: calculateDoors() shape: \'%s\'" % shape)
        print("DEBUG: calculateDoors() lengths: \'%s\'" % lengths)
        print("DEBUG: calculateDoors() positions: \'%s\'" % positions)


def addIncidentEdgeToDoorList(polygon, edge, net, doorInfoList):
    assert net.hasEdge(edge.getID())
    if DEBUG:
        print("DEBUG: incident edge \'%s\' for current node \'%s\'" % (edge.getID(), polygon.attributes[KEY_SUMO_ID]))
    lane = getExclusiveSidewalkLane(edge)
    if lane is None:
        return
    shape = calculateBoundingPolygon(lane.getShape(includeJunctions=False), lane.getWidth())
    lengths = sumolib.geomhelper.intersectsAtLengths2D(polygon.shape, shape)
    lengths.sort()
    positions = [sumolib.geomhelper.positionAtShapeOffset(polygon.shape, offset) for offset in lengths]
    doorId = polygon.attributes[KEY_SUMO_ID] + JPS_DOOR_ID_DELIMITER + lane.getID()
    doorInfoList.append(DoorInfo(doorId, positions, polygon, lengths))


def subtractDoorsFromPolygon(polygon, doorInfoList):
    result = []
    lengths = []
    for doorInfo in doorInfoList:
        if DEBUG:
            print("DEBUG: doorInfo._atLengthsOfParent:", doorInfo._atLengthsOfParent)
        len1 = doorInfo._atLengthsOfParent[0]
        len2 = doorInfo._atLengthsOfParent[-1]
        assert (len1 < len2), "len1 should be smaller than len2 (len1=%d, len2=%d)" % (len1, len2)  # noqa
        if len2 - len1 > doorInfo._width:
            # corner case with inversely oriented door and closed parent polygon
            len1 = len2
            len2 = sumolib.geomhelper.polyLength(polygon.shape)
        if len1 in lengths and len2 in lengths:
            # ignore duplicates
            continue
        elif len1 not in lengths and len2 not in lengths:
            lengths.append(len1)
            lengths.append(len2)
        else:
            print("ERROR: only one of (len1, len2) found in length list, aborting...")
            sys.exit(1)
    if DEBUG:
        print("DEBUG: lengths:", lengths)
    lengths.sort()
    if DEBUG:
        print("DEBUG: lengths.sorted:", lengths)
    polySlices = sumolib.geomhelper.splitPolygonAtLengths2D(polygon.shape, lengths)
    # start at second slice if first door is at beginning of polygon
    startSliceIdx = 1 if lengths[0] == 0.0 else 0
    # stop at penultimate slice if last door is at end of polygon
    endSliceIdx = len(polySlices) - 1 if lengths[-1] == sumolib.geomhelper.polyLength(polygon.shape) else len(polySlices)  # noqa
    # intersectsAtLengths2D() filters out duplicate lengths -> no two doors are directly adjacent to each other
    for i in range(startSliceIdx, endSliceIdx, 2):
        p = sumolib.shapes.polygon.Polygon(id=polygon.id + JPS_BOUNDARY_SLICE_DELIMITER + str(i),
                                           color=JPS_BOUNDARY_COLOR,
                                           layer=JPS_BOUNDARY_LAYER,
                                           shape=polySlices[i])
        p.attributes[KEY_JPS_TYPE] = polygon.attributes[KEY_JPS_TYPE]
        p.attributes[KEY_SUMO_ID] = polygon.attributes[KEY_SUMO_ID]
        result.append(p)
    return result


def calculateDoors(polygons, net):
    result = []
    myPolygonSlices = []
    myLastTransitionId = 0
    for p in polygons:
        isLane = net.hasEdge(p.attributes[KEY_SUMO_ID][:-2])
        isNode = net.hasNode(p.attributes[KEY_SUMO_ID])
        if not isLane and not isNode:
            print("ERROR: objID \'%s\' not found as lane or node in network, aborting..." %
                  p.attributes[KEY_SUMO_ID])
            sys.exit(1)
        doorInfoList = []
        if isLane:
            lane = net.getLane(p.attributes[KEY_SUMO_ID])
            if DEBUG:
                print("DEBUG: calculateDoors() lane \'%s\'" % lane.getID())
            for inLane in lane.getIncoming(onlyDirect=True):
                addInOutLaneToDoorList(p, inLane, net, doorInfoList, direction='in')
            for outCon in lane.getOutgoing():
                addInOutLaneToDoorList(p, outCon.getToLane(), net, doorInfoList, direction='out')
        elif isNode:
            node = net.getNode(p.attributes[KEY_SUMO_ID])
            if DEBUG:
                print("DEBUG: calculateDoors() node \'%s\'" % node.getID())
            for inc in node.getIncoming():
                if inc.getID()[0] == ":":
                    continue
                addIncidentEdgeToDoorList(p, inc, net, doorInfoList)
            for out in node.getOutgoing():
                if out.getID()[0] == ":":
                    continue
                addIncidentEdgeToDoorList(p, out, net, doorInfoList)
        for doorInfo in doorInfoList:
            door = sumolib.shapes.polygon.Polygon(id=doorInfo._id,
                                                  color=JPS_DOOR_COLOR,
                                                  layer=JPS_DOOR_LAYER,
                                                  shape=doorInfo._shape)
            door.attributes[KEY_JPS_TYPE] = JPS_DOOR_TYPE_ID
            door.attributes[KEY_FROM_ID] = door.id.split(JPS_DOOR_ID_DELIMITER)[0]
            door.attributes[KEY_TO_ID] = door.id.split(JPS_DOOR_ID_DELIMITER)[1]
            door.attributes[KEY_TRANSITION_ID] = myLastTransitionId
            if not isDuplicate(door, result):
                result.append(door)
                myLastTransitionId += 1
        myPolygonSlices += subtractDoorsFromPolygon(p, doorInfoList)
    return result, myPolygonSlices


def addLaneToPolygons(lane, polygons):
    if not lane.allows("pedestrian"):
        print("WARNING: lane \'%s\' does not allow pedestrians, skipping..." % (lane.getID()))
        return
    polyShape = calculateBoundingPolygon(lane.getShape(includeJunctions=False), lane.getWidth())
    polygon = sumolib.shapes.polygon.Polygon(id=lane.getID() + JPS_BOUNDARY_ID_SUFFIX,
                                             color=JPS_BOUNDARY_COLOR,
                                             layer=JPS_BOUNDARY_LAYER,
                                             shape=polyShape)
    polygon.attributes[KEY_JPS_TYPE] = JPS_BOUNDARY_TYPE_ID
    polygon.attributes[KEY_SUMO_ID] = lane.getID()
    if not isDuplicate(polygon, polygons):
        polygons.append(polygon)
    return


def addNodeToPolygons(node, polygons):
    polyShape = node.getShape()
    polyShape.append(polyShape[0])
    polygon = sumolib.shapes.polygon.Polygon(id=node.getID() + JPS_BOUNDARY_ID_SUFFIX,
                                             color=JPS_BOUNDARY_COLOR,
                                             layer=JPS_BOUNDARY_LAYER,
                                             shape=polyShape)
    polygon.attributes[KEY_JPS_TYPE] = JPS_BOUNDARY_TYPE_ID
    polygon.attributes[KEY_SUMO_ID] = node.getID()
    if not isDuplicate(polygon, polygons):
        polygons.append(polygon)
    return


def getExclusiveSidewalkLane(edge):
    for i in range(edge.getLaneNumber()):
        lane = edge.getLanes()[i]
        if lane.allows("pedestrian"):
            if DEBUG:
                print("DEBUG: exclusive sidewalk of edge \'%s\' is lane \'%s\'..." % (edge.getID(), lane.getID()))
            return lane
    print("WARNING: edge \'%s\' has no exclusive sidewalk lane..." % (edge.getID()))
    return None


def writeToDxf(polygons, doors, options):
    # write DXF file
    doc = ezdxf.new(dxfversion='R2000')
    msp = doc.modelspace()  # add new entities to the modelspace
    doc.layers.new(name=DXF_LAYER_NAME_BOUNDARY, dxfattribs={'linetype': 'SOLID', 'color': 7})
    for p in polygons:
        msp.add_lwpolyline(p.shape, dxfattribs={'layer': DXF_LAYER_NAME_BOUNDARY})
    doc.layers.new(name=DXF_LAYER_NAME_DOOR, dxfattribs={'linetype': 'DASHED'})
    for d in doors:
        # use color attribute to encode transition id
        msp.add_lwpolyline(d.shape, dxfattribs={'layer': DXF_LAYER_NAME_DOOR, 'color': d.attributes[KEY_TRANSITION_ID]})
    doc.saveas(options.outFile)


if __name__ == "__main__":
    options = parse_args()
    net = sumolib.net.readNet(options.netFile, withInternal=True, withPedestrianConnections=True)
    selectedObjects = sumolib.files.selection.read(options.selectedObjectsFile, lanes2edges=False)

    polygons = []
    if "edge" in selectedObjects.keys():
        for edgeId in selectedObjects["edge"]:
            try:
                edge = net.getEdge(edgeId)
            except KeyError:
                print("WARNING: edge \'%s\' does not exist in the network, skipping..." % (edgeId))
                continue
            if not edge.allows("pedestrian"):
                print("WARNING: edge \'%s\' does not allow pedestrians, skipping..." % (edgeId))
                continue
            if not options.no_exclusive_sidewalks:
                addLaneToPolygons(getExclusiveSidewalkLane(edge), polygons)
            else:
                for lane in edge.getLanes():
                    addLaneToPolygons(lane, polygons)
    if "lane" in selectedObjects.keys():
        for laneId in selectedObjects["lane"]:
            try:
                lane = net.getLane(laneId)
            except KeyError:
                print("WARNING: lane \'%s\' does not exist in the network, skipping..." % (laneId))
                continue
            except IndexError:
                print("WARNING: lane \'%s\' does not exist in the network (please check lane index), skipping..." % (
                    laneId))
                continue
            if not options.no_exclusive_sidewalks:
                sidewalkLane = getExclusiveSidewalkLane(lane.getEdge())
                if laneId != sidewalkLane.getID():
                    print("WARNING: lane \'%s\' is not the exclusive sidewalk lane, skipping..." % (laneId))
                    continue
            addLaneToPolygons(lane, polygons)
    if options.allow_junctions and "junction" in selectedObjects.keys():
        for nodeId in selectedObjects["junction"]:
            try:
                node = net.getNode(nodeId)
            except KeyError:
                print("WARNING: node \'%s\' does not exist in the network, skipping..." % (nodeId))
                continue
            addNodeToPolygons(node, polygons)

    doors, polygonSlices = calculateDoors(polygons, net)
    if not options.outFile:
        options.outFile = options.netFile[:options.netFile.rfind(".net.xml")] + ".dxf"
    if options.write_additional_file:
        additionalFile = options.outFile[:options.outFile.rfind(".dxf")] + ".poly.xml"
        sumolib.files.additional.write(additionalFile, doors + polygonSlices)
    writeToDxf(polygonSlices, doors, options)
