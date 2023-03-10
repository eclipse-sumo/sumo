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
import re
from os.path import dirname, join
from subprocess import check_output, CalledProcessError

# list of folders and tools
tools = [

# detector
    #["detector", ".", "aggregateFlows"],   NO CONFIG
    #["detector", ".", "detector"],         NO CONFIG
    ["detector", ".", "edgeDataFromFlow"],
    #["detector", ".", "filterFlows"],      NO CONFIG
    #["detector", ".", "flowFromEdgeData"], NO CONFIG
    #["detector", ".", "flowFromRoutes"],   NO CONFIG    
    #["detector", ".", "flowrouter"],       NO CONFIG
    ["detector", ".", "mapDetectors"],
    #["detector", ".", "plotFlows"],        NO CONFIG
    #["detector", ".", "routeUsage"],       NO CONFIG
    #["detector", ".", "validate"],         NO CONFIG

# district
    #["district", ".", "aggregateAndSplitMatrices"],    NO CONFIG    
    #["district", ".", "countConnectionsInDistricts"],  NO CONFIG
    #["district", ".", "districtMapper"],               NO CONFIG
    #["district", ".", "filterDistricts"],              NO CONFIG
    #["district", ".", "gridDistricts"],                NO CONFIG
    ["district", ".", "stationDistricts"],
    
# drt
    #["drt", ".", "darpSolvers"],   NO CONFIG
    #["drt", ".", "drtOnline"],     NO CONFIG
    ["drt", ".", "drtOrtools"],
    #["drt", ".", "ortools_pdp"],   NO CONFIG

# emissions
    #["emissions", ".", "findMinDiffModel"],    NO CONFIG
    #["emissions", ".", "nefz"],                NO CONFIG
    
# import
    ["import", "citybrain", "citybrain_flow"],
    ["import", "citybrain", "citybrain_infostep"],
    ["import", "citybrain", "citybrain_road"],
    ["import", "gtfs", "gtfs2fcd"],
    #["import", "gtfs", "gtfs2osm"],                                NO CONFIG
    ["import", "gtfs", "gtfs2pt"],
    #["import", "vissim", "convert_detectors2SUMO"],                NO CONFIG
    #["import", "vissim", "convert_vissimXML_flows_statRoutes"],    NO CONFIG
    #["import", "vissim", "tls_vissimXML2SUMOnet_update"],          NO CONFIG
    #["import", "vissim", "vissim_parseBusStops"],                  NO CONFIG
    #["import", "vissim", "vissim_parseRoutes"],                    NO CONFIG    
    #["import", "vissim", "vissim2poly"],                           NO CONFIG    
    #["import", "visum", "visum_convertEdgeTypes"],                 NO CONFIG    
    #["import", "visum", "visum_convertRoutes"],                    NO CONFIG
    #["import", "visum", "visum_convertTurnPercentages"],           NO CONFIG
    #["import", "visum", "visum_parseZaehlstelle"],                 NO CONFIG
    #["import", ".", "matsim_importPlans"],                         NO CONFIG
    #["import", ".", "signal_POIs_from_xodr"],                      NO CONFIG    
    #["import", ".", "osmTaxiStop"],                                NO CONFIG

# net
    ["net", ".", "abstractRail"],
    #["net", ".", "createRoundaboutConnections"],   NO CONFIG
    ["net", ".", "net2geojson"],
    ["net", ".", "net2jpsgeometry"],
    ["net", ".", "net2kml"],
    ["net", ".", "net2poly"],
    #["net", ".", "netcheck"],                      NO CONFIG
    ["net", ".", "netdiff"],
    #["net", ".", "netduplicate"],                  NO CONFIG
    #["net", ".", "netmatch"],                      NO CONFIG
    ["net", ".", "network_statistics"],
    #["net", ".", "odConnectionsCheck"],            NO CONFIG
    #["net", ".", "prepareVISUM"],                  NO CONFIG
    ["net", ".", "reduceLanes"],
    #["net", ".", "reprojectpolys"],                NO CONFIG
    ["net", ".", "split_at_stops"],
    #["net", ".", "visum_mapDistricts"],            NO CONFIG
    #["net", ".", "xmlconnections_mapEdges"],       NO CONFIG
    #["net", ".", "xmledges_applyOffset"],          NO CONFIG
    #["net", ".", "xmlnodes_applyOffset"],          NO CONFIG    
    
# route
    #["route", ".", "addParkingAreaStops2Routes"],  NO CONFIG
    #["route", ".", "addParkingAreaStops2Trips"],   NO CONFIG
    ["route", ".", "addStopDelay"],
    ["route", ".", "addStops2Routes"],
    ["route", ".", "addTAZ"],
    #["route", ".", "analyzePersonPlans"],          NO CONFIG
    ["route", ".", "checkStopOrder"],
    #["route", ".", "cutRoutes"],                   NO CONFIG
    #["route", ".", "cutTrips"],
    ["route", ".", "implausibleRoutes"],
    #["route", ".", "route_1htoDay"],               NO CONFIG
    #["route", ".", "route_departOffset"],          NO CONFIG
    #["route", ".", "route2alts"],                  NO CONFIG
    ["route", ".", "route2OD"],
    #["route", ".", "route2poly"],                  NO CONFIG
    #["route", ".", "route2sel"],                   NO CONFIG    
    #["route", ".", "routecheck"],                  NO CONFIG
    #["route", ".", "routecompare"],                NO CONFIG
    ["route", ".", "routeStats"],
    ["route", ".", "scaleTimeLine"],
    #["route", ".", "sort_routes"],                 NO CONFIG
    #["route", ".", "splitRandom"],                 NO CONFIG
    #["route", ".", "splitRouteFiles"],             NO CONFIG
    #["route", ".", "tracegenerator"],              NO CONFIG    
    ["route", ".", "tracemapper"],
    #["route", ".", "vehicle2flow"],                NO CONFIG
    
# output
    #["output", ".", "accelerations"],                      NO CONFIG
    #["output", ".", "aggregateBatteryOutput"],             NO CONFIG
    #["output", ".", "analyze_pedestrian_jam"],             NO CONFIG
    ["output", ".", "analyze_teleports"],
    ["output", ".", "attributeDiff"],
    ["output", ".", "attributeStats"],
    ["output", ".", "computeCoordination"],
    ["output", ".", "computePassengerCounts"],
    ["output", ".", "computeStoppingPlaceUsage"],
    ["output", ".", "countLaneChanges"],
    ["output", ".", "edgeDataDiff"],
    #["output", ".", "edgeDataStatistics"],                 NO CONFIG
    ["output", ".", "fcdDiff"],
    #["output", ".", "generateITetrisIntersectionMetrics"], NO CONFIG
    #["output", ".", "generateITetrisNetworkMetrics"],      NO CONFIG
    #["output", ".", "generateMeanDataDefinitions"],        NO CONFIG
    #["output", ".", "generateTLSE1Detectors"],             NO CONFIG
    ["output", ".", "generateTLSE2Detectors"],
    #["output", ".", "generateTLSE3Detectors"],             NO CONFIG
    #["output", ".", "netdumpdiff"],                        NO CONFIG
    #["output", ".", "netdumpmean"],                        NO CONFIG
    ["output", ".", "parkingSearchTraffic"],
    ["output", ".", "scheduleStats"],
    #["output", ".", "statisticsElements"],                 NO CONFIG
    ["output", ".", "stopOrder"],
    #["output", ".", "tables"],                             NO CONFIG
    #["output", ".", "timingStats"],                        NO CONFIG
    #["output", ".", "tripinfoByTAZ"],                      NO CONFIG
    ["output", ".", "tripinfoByType"],
    #["output", ".", "tripinfoDiff"],                       NO CONFIG
    #["output", ".", "tripStatistics"],                     NO CONFIG
    ["output", ".", "vehLanes"],
    #["output", ".", "vehroute2amitranOD"],                 NO CONFIG
    ["output", ".", "vehrouteCountValidation"],
    ["output", ".", "vehrouteDiff"],
    ["output", ".", "walkFactor"],

# shapes
    #["shapes", ".", "circlePolygon"],      NO CONFIG
    #["shapes", ".", "CSV2polyconvertXML"], NO CONFIG
    #["shapes", ".", "poi_alongRoads"],     NO CONFIG
    #["shapes", ".", "poi_at_stops"],       NO CONFIG
    #["shapes", ".", "poi_atTLS"],          NO CONFIG

# tls
    #["tls", ".", "buildTransitions"],      NO CONFIG
    #["tls", ".", "createTlsCsv"],          NO CONFIG
    #["tls", ".", "tls_check"],             NO CONFIG
    #["tls", ".", "tls_csv2SUMO"],          NO CONFIG
    #["tls", ".", "tls_csvSignalGroups"],   NO CONFIG

# turn-defs
    #["turn-defs", ".", "collectinghandler"],       NO CONFIG
    #["turn-defs", ".", "connections"],             NO CONFIG
    #["turn-defs", ".", "generateTurnDefs"],        NO CONFIG
    #["turn-defs", ".", "generateTurnRatios"],      NO CONFIG
    #["turn-defs", ".", "turnCount2EdgeCount"],     NO CONFIG
    #["turn-defs", ".", "turndefinitions"],         NO CONFIG
    #["turn-defs", ".", "turnFile2EdgeRelations"],  NO CONFIG

# visualization
    #["visualization", ".", "macrOutput"],                  NO CONFIG
    #["visualization", ".", "mpl_dump_onNet"],              NO CONFIG
    #["visualization", ".", "mpl_dump_timeline"],           NO CONFIG
    #["visualization", ".", "mpl_dump_twoAgainst"],         NO CONFIG
    #["visualization", ".", "mpl_tripinfos_twoAgainst"],    NO CONFIG
    #["visualization", ".", "plot_csv_bars"],               NO CONFIG
    #["visualization", ".", "plot_csv_pie"],                NO CONFIG
    #["visualization", ".", "plot_csv_timeline"],           NO CONFIG
    #["visualization", ".", "plot_net_dump"],               NO CONFIG
    #["visualization", ".", "plot_net_selection"],          NO CONFIG
    #["visualization", ".", "plot_net_speeds"],             NO CONFIG
    #["visualization", ".", "plot_net_trafficLights"],      NO CONFIG
    #["visualization", ".", "plot_summary"],                NO CONFIG
    #["visualization", ".", "plot_tripinfo_distributions"], NO CONFIG
    ["visualization", ".", "plotXMLAttributes"],

# xml
    #["xml", ".", "addSchema"],         NO CONFIG
    #["xml", ".", "binary2plain"],      NO CONFIG
    #["xml", ".", "changeAttribute"],   NO CONFIG
    #["xml", ".", "csv2xml"],           NO CONFIG
    #["xml", ".", "filterElements"],    NO CONFIG
    #["xml", ".", "protobuf2xml"],      NO CONFIG
    #["xml", ".", "xml2csv"],           NO CONFIG
    #["xml", ".", "xml2protobuf"],      NO CONFIG
    #["xml", ".", "xsd"],               NO CONFIG

# other
    #[".", ".", "averageTripStatistics"],       NO CONFIG         
    [".", ".", "countEdgeUsage"],
    #[".", ".", "createVehTypeDistribution"],   NO CONFIG
    [".", ".", "edgesInDistricts"],
    [".", ".", "evacuateAreas"],
    [".", ".", "extractTest"],
    [".", ".", "fcdReplay"],
    [".", ".", "findAllRoutes"],
    [".", ".", "generateBidiDistricts"],
    #[".", ".", "generateContinuousRerouters"], NO CONFIG
    [".", ".", "generateParkingAreaRerouters"],
    [".", ".", "generateParkingAreas"],
    #[".", ".", "generateParkingLots"],         NO CONFIG
    [".", ".", "generateRailSignalConstraints"],
    [".", ".", "generateRerouters"],
    #[".", ".", "jtcrouter"],                   NO CONFIG
    [".", ".", "osmBuild"],
    [".", ".", "osmGet"],
    [".", ".", "osmWebWizard"],
    [".", ".", "plot_trajectories"],
    #[".", ".", "ptlines2flows"],               NO CONFIG
    [".", ".", "randomTrips"],
    [".", ".", "routeSampler"],
    [".", ".", "runSeeds"],
    [".", ".", "stateReplay"],
    [".", ".", "tileGet"],
    [".", ".", "tlsCoordinator"],
    [".", ".", "tlsCycleAdaptation"],
    #[".", ".", "traceExporter"]                NO CONFIG
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
    if templateStr == '' or templateStr[0] != "<":
        return '"";\n'
    # replace all current directory values (src/netedit)
    templateStr = re.sub("(?<=value)(.*)(?=netedit)", "", templateStr)
    templateStr = templateStr.replace('netedit', '="')
    # replace " with \"
    templateStr = templateStr.replace('"', '\\"')
    # add quotes and end lines
    templateStr = templateStr.replace("<", '"<')
    templateStr = templateStr.replace("\n", '"\n')
    templateStr = templateStr[:-1]
    # update last line
    if templateStr == '':
        templateStr = '""'
    templateStr += '\n'
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
    toolPath = os.path.join(subDirA, subDirB, toolName + ".py")
    # get path
    path = os.path.join(toolDir, toolPath)
    # create pair
    pair = 'const std::pair<std::string, std::string> ' + toolName + 'Template = std::make_pair("' + toolPath + '",\n'
    # check if exists
    if os.path.exists(path):
        # show info
        print("Obtaining '" + toolName + "' tool template")
        # obtain template piping stdout using check_output
        try:
            template = check_output([sys.executable, path, "--save-template", "stdout"], universal_newlines=True)
        except CalledProcessError as e:
            sys.stderr.write("Error when generating tool template for %s: '%s'" % (toolName, e))
            return pair + ');\n'
        # join variable and formated template
        return pair + formatToolTemplate(template) + ');\n'
    # if tool wasn't found, then raise exception
    raise Exception(toolName + "Template cannot be generated. '" + path + "' not found.")


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
