#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2015-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    templates.py
# @author  Pablo Alvarez Lopez
# @date    Dec 2022

"""
This script rebuilds "src/netedit/templates.h" the files
representing the templates.
It does this by parsing the data from the sumo data dir.
"""

from __future__ import print_function
from __future__ import absolute_import

import sys
import os
from os.path import dirname, join
from subprocess import check_output, CalledProcessError

# list of folders and tools
tools = [

# detector
    #["detector", ".", "aggregateFlows"],
    #["detector", ".", "detector"],
    ["detector", ".", "edgeDataFromFlow"],
    #["detector", ".", "filterFlows"],
    #["detector", ".", "flowFromEdgeData"],
    #["detector", ".", "flowFromRoutes"],
    #["detector", ".", "flowrouter"],
    ["detector", ".", "mapDetectors"],
    #["detector", ".", "plotFlows"],
    #["detector", ".", "routeUsage"],
    #["detector", ".", "validate"],

# district
    #["district", ".", "aggregateAndSplitMatrices"],
    #["district", ".", "countConnectionsInDistricts"],
    #["district", ".", "districtMapper"],
    #["district", ".", "filterDistricts"],
    #["district", ".", "gridDistricts"],
    ["district", ".", "stationDistricts"],
    
# drt
    #["drt", ".", "darpSolvers"],
    #["drt", ".", "drtOnline"],
    ["drt", ".", "drtOrtools"],
    #["drt", ".", "ortools_pdp"],

# emissions
    #["emissions", ".", "findMinDiffModel"],
    #["emissions", ".", "nefz"],
    
# import
    ["import", "citybrain", "citybrain_flow"],
    ["import", "citybrain", "citybrain_infostep"],
    ["import", "citybrain", "citybrain_road"],
    ["import", "gtfs", "gtfs2fcd"],
    #["import", "gtfs", "gtfs2osm"],
    ["import", "gtfs", "gtfs2pt"],
    #["import", "vissim", "convert_detectors2SUMO"],
    #["import", "vissim", "convert_vissimXML_flows_statRoutes"],
    #["import", "vissim", "tls_vissimXML2SUMOnet_update"],
    #["import", "vissim", "vissim_parseBusStops"],
    #["import", "vissim", "vissim_parseRoutes"],
    #["import", "vissim", "vissim2poly"],
    #["import", "visum", "visum_convertEdgeTypes"],
    #["import", "visum", "visum_convertRoutes"],
    #["import", "visum", "visum_convertTurnPercentages"],
    #["import", "visum", "visum_parseZaehlstelle"],
    #["import", ".", "matsim_importPlans"],
    #["import", ".", "signal_POIs_from_xodr"],
    #["import", ".", "osmTaxiStop"],

# net
    ["net", ".", "abstractRail"],
    #["net", ".", "createRoundaboutConnections"],
    ["net", ".", "net2geojson"],
    ["net", ".", "net2jpsgeometry"],
    ["net", ".", "net2kml"],
    ["net", ".", "net2poly"],
    #["net", ".", "netcheck"],
    ["net", ".", "netdiff"],
    #["net", ".", "netduplicate"],
    #["net", ".", "netmatch"],
    ["net", ".", "network_statistics"],
    #["net", ".", "odConnectionsCheck"],
    #["net", ".", "prepareVISUM"],
    ["net", ".", "reduceLanes"],
    #["net", ".", "reprojectpolys"],
    ["net", ".", "split_at_stops"],
    #["net", ".", "visum_mapDistricts"],
    #["net", ".", "xmlconnections_mapEdges"],
    #["net", ".", "xmledges_applyOffset"],
    #["net", ".", "xmlnodes_applyOffset"],
    
# route
    #["route", ".", "addParkingAreaStops2Routes"],
    #["route", ".", "addParkingAreaStops2Trips"],
    ["route", ".", "addStopDelay"],
    ["route", ".", "addStops2Routes"],
    ["route", ".", "addTAZ"],
    #["route", ".", "analyzePersonPlans"],
    ["route", ".", "checkStopOrder"],
    #["route", ".", "cutRoutes"],
    #["route", ".", "cutTrips"],
    ["route", ".", "implausibleRoutes"],
    #["route", ".", "route_1htoDay"],
    #["route", ".", "route_departOffset"],
    #["route", ".", "route2alts"],
    ["route", ".", "route2OD"],
    #["route", ".", "route2poly"],
    #["route", ".", "route2sel"],
    #["route", ".", "routecheck"],
    #["route", ".", "routecompare"],
    ["route", ".", "routeStats"],
    ["route", ".", "scaleTimeLine"],
    #["route", ".", "sort_routes"],
    #["route", ".", "splitRandom"],
    #["route", ".", "splitRouteFiles"],
    #["route", ".", "tracegenerator"],
    ["route", ".", "tracemapper"],
    #["route", ".", "vehicle2flow"],
    
# output
    #["output", ".", "accelerations"],
    #["output", ".", "aggregateBatteryOutput"],
    #["output", ".", "analyze_pedestrian_jam"],
    ["output", ".", "analyze_teleports"],
    ["output", ".", "attributeDiff"],
    ["output", ".", "attributeStats"],
    ["output", ".", "computeCoordination"],
    ["output", ".", "computePassengerCounts"],
    ["output", ".", "computeStoppingPlaceUsage"],
    ["output", ".", "countLaneChanges"],
    ["output", ".", "edgeDataDiff"],
    #["output", ".", "edgeDataStatistics"],
    ["output", ".", "fcdDiff"],
    #["output", ".", "generateITetrisIntersectionMetrics"],
    #["output", ".", "generateITetrisNetworkMetrics"],
    #["output", ".", "generateMeanDataDefinitions"],
    #["output", ".", "generateTLSE1Detectors"],
    ["output", ".", "generateTLSE2Detectors"],
    #["output", ".", "generateTLSE3Detectors"],
    #["output", ".", "netdumpdiff"],
    #["output", ".", "netdumpmean"],
    ["output", ".", "parkingSearchTraffic"],
    ["output", ".", "scheduleStats"],
    #["output", ".", "statisticsElements"],
    ["output", ".", "stopOrder"],
    #["output", ".", "tables"],
    #["output", ".", "timingStats"],
    #["output", ".", "tripinfoByTAZ"],
    ["output", ".", "tripinfoByType"],
    #["output", ".", "tripinfoDiff"],
    #["output", ".", "tripStatistics"],
    ["output", ".", "vehLanes"],
    #["output", ".", "vehroute2amitranOD"],
    ["output", ".", "vehrouteCountValidation"],
    ["output", ".", "vehrouteDiff"],
    ["output", ".", "walkFactor"],

# shapes
    #["shapes", ".", "circlePolygon"],
    #["shapes", ".", "CSV2polyconvertXML"],
    #["shapes", ".", "poi_alongRoads"],
    #["shapes", ".", "poi_at_stops"],
    #["shapes", ".", "poi_atTLS"],

# tls
    #["tls", ".", "buildTransitions"],
    #["tls", ".", "createTlsCsv"],
    #["tls", ".", "tls_check"],
    #["tls", ".", "tls_csv2SUMO"],
    #["tls", ".", "tls_csvSignalGroups"],

# turn-defs
    #["turn-defs", ".", "collectinghandler"],
    #["turn-defs", ".", "connections"],
    #["turn-defs", ".", "generateTurnDefs"],
    #["turn-defs", ".", "generateTurnRatios"],
    #["turn-defs", ".", "turnCount2EdgeCount"],
    #["turn-defs", ".", "turndefinitions"],
    #["turn-defs", ".", "turnFile2EdgeRelations"],

# visualization
    #["visualization", ".", "macrOutput"],
    #["visualization", ".", "mpl_dump_onNet"],
    #["visualization", ".", "mpl_dump_timeline"],
    #["visualization", ".", "mpl_dump_twoAgainst"],
    #["visualization", ".", "mpl_tripinfos_twoAgainst"],
    #["visualization", ".", "plot_csv_bars"],
    #["visualization", ".", "plot_csv_pie"],
    #["visualization", ".", "plot_csv_timeline"],
    #["visualization", ".", "plot_net_dump"],
    #["visualization", ".", "plot_net_selection"],
    #["visualization", ".", "plot_net_speeds"],
    #["visualization", ".", "plot_net_trafficLights"],
    #["visualization", ".", "plot_summary"],
    #["visualization", ".", "plot_tripinfo_distributions"],
    ["visualization", ".", "plotXMLAttributes"],

# xml
    #["xml", ".", "addSchema"],
    #["xml", ".", "binary2plain"],
    #["xml", ".", "changeAttribute"],
    #["xml", ".", "csv2xml"],
    #["xml", ".", "filterElements"],
    #["xml", ".", "protobuf2xml"],
    #["xml", ".", "xml2csv"],
    #["xml", ".", "xml2protobuf"],
    #["xml", ".", "xsd"],

# other
    #[".", ".", "averageTripStatistics"],
    [".", ".", "countEdgeUsage"],
    #[".", ".", "createVehTypeDistribution"],
    [".", ".", "edgesInDistricts"],
    [".", ".", "evacuateAreas"],
    [".", ".", "extractTest"],
    [".", ".", "fcdReplay"],
    [".", ".", "findAllRoutes"],
    [".", ".", "generateBidiDistricts"],
    #[".", ".", "generateContinuousRerouters"],
    [".", ".", "generateParkingAreaRerouters"],
    [".", ".", "generateParkingAreas"],
    #[".", ".", "generateParkingLots"],
    [".", ".", "generateRailSignalConstraints"],
    [".", ".", "generateRerouters"],
    #[".", ".", "jtcrouter"],
    [".", ".", "osmBuild"],
    [".", ".", "osmGet"],
    [".", ".", "osmWebWizard"],
    [".", ".", "plot_trajectories"],
    #[".", ".", "ptlines2flows"],
    [".", ".", "randomTrips"],
    [".", ".", "routeSampler"],
    [".", ".", "runSeeds"],
    [".", ".", "stateReplay"],
    [".", ".", "tileGet"],
    [".", ".", "tlsCoordinator"],
    [".", ".", "tlsCycleAdaptation"],
    #[".", ".", "traceExporter"]
]


def formatBinTemplate(templateStr):
    """
    @brief parse binary of a bin template (sumo, neconvert, etc.)
    """
    # remove endlines in Windows
    templateStr = templateStr.replace("\\r", '')
    # replace " with \"
    templateStr = templateStr.replace('"', '\\"')
    # split lines
    templateStr = templateStr.replace("\n", '"\n    "')
    # remove empty lines
    templateStr = templateStr.replace('    ""\n', '')
    # replace first backspace
    templateStr = templateStr.replace("\\b'", '"')
    # add last "
    templateStr += '"'
    # remove last line
    templateStr = templateStr.replace('\n    ""', '')
    # add ending
    templateStr += ';\n'
    return templateStr


def formatToolTemplate(templateStr):
    """
    @brief format python tool template
    """
    # replace " with \"
    templateStr = templateStr.replace('"', '\\"')
    # add quotes and end lines
    templateStr = templateStr.replace("<", '"<')
    templateStr = templateStr.replace("\n", '"\n')
    templateStr = templateStr[:-1]
    # update last line
    if templateStr == '':
        templateStr = '""'
    templateStr += ';\n'
    return templateStr


def generateSumoTemplate(sumoBin):
    """
    @brief generate template para sumo
    """
    if os.path.exists(sumoBin):
        # show info
        print("Obtaining sumo template")
        # obtain template piping stdout using check_output
        try:
            template = check_output([sumoBin, "--save-template", "stdout"], universal_newlines=True)
        except CalledProcessError as e:
            sys.stderr.write("Error when generating template for sumocfg: '%s'" % e)
            return 'const std::string sumoTemplate = "";'
        # join variable and formated template
        return 'const std::string sumoTemplate = "' + formatBinTemplate(template)
    # if binary wasn't found, then raise exception
    raise Exception("SUMO Template cannot be generated. SUMO binary not found. "
                    "Make sure that sumo or sumoD was generated in bin folder")


def generateToolTemplate(toolDir, subDirA, subDirB, toolName):
    """
    @brief generate tool template
    """
    # get toolPath
    toolPath = os.path.join(toolDir, subDirA, subDirB, toolName + ".py")
    # check if exists
    if os.path.exists(toolPath):
        # show info
        print("Obtaining '" + toolName + "' tool template")
        # obtain template piping stdout using check_output
        try:
            template = check_output([sys.executable, toolPath, "--save-template", "stdout"], universal_newlines=True)
        except CalledProcessError as e:
            sys.stderr.write("Error when generating tool template for %s: '%s'" % (toolName, e))
            return "const std::string " + toolName + 'Template = "";'
        # join variable and formated template
        return "const std::string " + toolName + 'Template = ' + formatToolTemplate(template)
    # if tool wasn't found, then raise exception
    raise Exception(toolName + "Template cannot be generated. '" + toolPath + "' not found.")


if __name__ == "__main__":
    if len(sys.argv) == 1:
        sys.exit("Arguments: <pathToSUMO>")
    # get tool dir path (SUMO/tools)
    toolDir = join(dirname(__file__), '..')
    # write templates.h
    with open("templates.h", 'w') as templateHeaderFile:
        print("#include <string>\n", file=templateHeaderFile)
        print(generateSumoTemplate(sys.argv[1]), file=templateHeaderFile)
        # generate Tool templates
        for tool in tools:
            print(generateToolTemplate(toolDir, tool[0], tool[1], tool[2]), file=templateHeaderFile)
