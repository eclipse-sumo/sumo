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
JPS_POLYGON_TYPE_NAME = "jps-boundary"
JPS_POLYGON_COLOR = sumolib.color.RGBAColor(255, 0, 0)
DXF_LAYER_NAME = "SUMONetPolygons"
DEBUG = True


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " -n <net> -s <selected-objects-file> <options>"
    argParser = sumolib.options.ArgumentParser(usage=USAGE)
    argParser.add_argument("-n", "--net-file", dest="netFile", help="The .net.xml file to convert")
    # see https://sumo.dlr.de/docs/sumo-gui.html#selecting_objects
    argParser.add_argument("-s", "--selected-objects-file", dest="selectedObjectsFile",
                           help="The txt file including the line-by-line list of objects to select")
    argParser.add_argument("-o", "--output-file", dest="outFile",
                           help="The JuPedSim dxf output file name")
    argParser.add_argument("-d", "--debug", action="store_true", default=True,
                           help="Export outer borders and handoff lines to debug file")
    # see https://sumo.dlr.de/docs/Simulation/Pedestrians.html#non-exclusive_sidewalks
    argParser.add_argument("-x", "--exclusive-sidewalks", action="store_true", default=True,
                           help="Choose exclusive sidewalk lane in case of ambiguities")

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


def addLaneToPolygons(lane, polygons):
    if not lane.allows("pedestrian"):
        print("WARNING: lane \'%s\' does not allow pedestrians, skipping..." % (lane.getID()))
        return
    polyShape = calculateBoundingPolygon(lane.getShape(includeJunctions=False), lane.getWidth())
    polygon = sumolib.shapes.polygon.Polygon(id=lane.getID(), type=JPS_POLYGON_TYPE_NAME,
                                             color=JPS_POLYGON_COLOR, shape=polyShape)
    if not isDuplicate(polygon, polygons):
        polygons.append(polygon)
    return


def addNodeToPolygons(node, polygons):
    polyShape = node.getShape()
    polyShape.append(polyShape[0])
    polygon = sumolib.shapes.polygon.Polygon(id=node.getID(), type=JPS_POLYGON_TYPE_NAME,
                                             color=JPS_POLYGON_COLOR, shape=polyShape)
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


def writeToDxf(polygons, options):
    # write DXF file
    doc = ezdxf.new(dxfversion='R2000')
    msp = doc.modelspace()  # add new entities to the modelspace
    doc.layers.new(name=DXF_LAYER_NAME, dxfattribs={'linetype': 'SOLID', 'color': 7})
    for p in polygons:
        msp.add_lwpolyline(p.shape, dxfattribs={'layer': DXF_LAYER_NAME})
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
            if options.exclusive_sidewalks:
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
                print("WARNING: lane \'%s\' does not exist in the network (please check lane index), skipping..." % (laneId))
                continue
            if options.exclusive_sidewalks:
                sidewalkLane = getExclusiveSidewalkLane(lane.getEdge())
                if laneId != sidewalkLane.getID():
                    print("WARNING: lane \'%s\' is not the exclusive sidewalk lane, skipping..." % (laneId))
                    continue
            addLaneToPolygons(lane, polygons)
    if "junction" in selectedObjects.keys():
        for nodeId in selectedObjects["junction"]:
            try:
                node = net.getNode(nodeId)
            except KeyError:
                print("WARNING: node \'%s\' does not exist in the network, skipping..." % (nodeId))
                continue
            addNodeToPolygons(node, polygons)

    if not options.outFile:
        options.outFile = options.netFile[:options.netFile.rfind(".net.xml")] + ".dxf"
    if options.debug:
        debugOutFile = options.outFile[:options.outFile.rfind(".dxf")] + ".poly.xml"
        sumolib.files.additional.write(debugOutFile, polygons)

    writeToDxf(polygons, options)
