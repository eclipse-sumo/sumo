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
from glob import glob

# list of folders and tools
TOOLS = [

    # detector
    # "detector/aggregateFlows.py",   NO CONFIG
    # "detector/detector.py",         NO CONFIG
    "detector/edgeDataFromFlow.py",
    # "detector/filterFlows.py",      NO CONFIG
    "detector/flowFromEdgeData.py",
    # "detector/flowFromRoutes.py",   NO CONFIG
    # "detector/flowrouter.py",       NO CONFIG
    "detector/mapDetectors.py",
    # "detector/plotFlows.py",        NO CONFIG
    # "detector/routeUsage.py",       NO CONFIG
    # "detector/validate.py",         NO CONFIG

    # district
    # "district/aggregateAndSplitMatrices.py",    NO CONFIG
    # "district/countConnectionsInDistricts.py",  NO CONFIG
    # "district/districtMapper.py",               NO CONFIG
    "district/filterDistricts.py",
    "district/gridDistricts.py",
    "district/stationDistricts.py",

    # drt
    # "drt/darpSolvers.py",   NO CONFIG
    # "drt/drtOnline.py",     NO CONFIG
    "drt/drtOrtools.py",
    # "drt/ortools_pdp.py",   NO CONFIG

    # emissions
    # "emissions/findMinDiffModel.py",    NO CONFIG
    # "emissions/nefz.py",                NO CONFIG

    # import
    "import/citybrain/citybrain_flow.py",
    "import/citybrain/citybrain_infostep.py",
    "import/citybrain/citybrain_road.py",
    "import/gtfs/gtfs2fcd.py",
    # "import/gtfs/gtfs2osm.py",                                NO CONFIG
    "import/gtfs/gtfs2pt.py",
    # "import/vissim/convert_detectors2SUMO.py",                NO CONFIG
    # "import/vissim/convert_vissimXML_flows_statRoutes.py",    NO CONFIG
    # "import/vissim/tls_vissimXML2SUMOnet_update.py",          NO CONFIG
    # "import/vissim/vissim_parseBusStops.py",                  NO CONFIG
    "import/vissim/vissim_parseRoutes.py",
    # "import/vissim/vissim2poly.py",                           NO CONFIG
    "import/visum/visum_convertEdgeTypes.py",
    # "import/visum/visum_convertRoutes.py",                    NO CONFIG
    # "import/visum/visum_convertTurnPercentages.py",           NO CONFIG
    # "import/visum/visum_parseZaehlstelle.py",                 NO CONFIG
    # "import/matsim_importPlans.py",                         NO CONFIG
    # "import/signal_POIs_from_xodr.py",                      NO CONFIG
    # "import/osmTaxiStop.py",                                NO CONFIG

    # net
    "net/abstractRail.py",
    # "net/createRoundaboutConnections.py",   NO CONFIG
    "net/net2geojson.py",
    "net/net2jpsgeometry.py",
    "net/net2kml.py",
    "net/net2poly.py",
    # "net/netcheck.py",                      NO CONFIG
    "net/netdiff.py",
    # "net/netduplicate.py",                  NO CONFIG
    # "net/netmatch.py",                      NO CONFIG
    "net/network_statistics.py",
    # "net/odConnectionsCheck.py",            NO CONFIG
    # "net/prepareVISUM.py",                  NO CONFIG
    "net/reduceLanes.py",
    # "net/reprojectpolys.py",                NO CONFIG
    "net/split_at_stops.py",
    # "net/visum_mapDistricts.py",            NO CONFIG
    # "net/xmlconnections_mapEdges.py",       NO CONFIG
    # "net/xmledges_applyOffset.py",          NO CONFIG
    # "net/xmlnodes_applyOffset.py",          NO CONFIG

    # route
    # "route/addParkingAreaStops2Routes.py",  NO CONFIG
    # "route/addParkingAreaStops2Trips.py",   NO CONFIG
    "route/addStopDelay.py",
    "route/addStops2Routes.py",
    "route/addTAZ.py",
    # "route/analyzePersonPlans.py",          NO CONFIG
    "route/checkStopOrder.py",
    # "route/cutRoutes.py",                   NO CONFIG
    # "route/cutTrips.py",
    "route/implausibleRoutes.py",
    # "route/route_1htoDay.py",               NO CONFIG
    # "route/route_departOffset.py",          NO CONFIG
    # "route/route2alts.py",                  NO CONFIG
    "route/route2OD.py",
    # "route/route2poly.py",                  NO CONFIG
    # "route/route2sel.py",                   NO CONFIG
    # "route/routecheck.py",                  NO CONFIG
    # "route/routecompare.py",                NO CONFIG
    "route/routeStats.py",
    "route/scaleTimeLine.py",
    # "route/sort_routes.py",                 NO CONFIG
    # "route/splitRandom.py",                 NO CONFIG
    # "route/splitRouteFiles.py",             NO CONFIG
    # "route/tracegenerator.py",              NO CONFIG
    "route/tracemapper.py",
    # "route/vehicle2flow.py",                NO CONFIG

    # output
    # "output/accelerations.py",                      NO CONFIG
    # "output/aggregateBatteryOutput.py",             NO CONFIG
    # "output/analyze_pedestrian_jam.py",             NO CONFIG
    "output/analyze_teleports.py",
    "output/attributeStats.py",
    "output/attributeDiff.py",
    "output/attributeCompare.py",
    "output/computeCoordination.py",
    "output/computePassengerCounts.py",
    "output/computeStoppingPlaceUsage.py",
    "output/countLaneChanges.py",
    "output/edgeDataDiff.py",
    # "output/edgeDataStatistics.py",                 NO CONFIG
    "output/fcdDiff.py",
    # "output/generateITetrisIntersectionMetrics.py", NO CONFIG
    # "output/generateITetrisNetworkMetrics.py",      NO CONFIG
    # "output/generateMeanDataDefinitions.py",        NO CONFIG
    # "output/generateTLSE1Detectors.py",             NO CONFIG
    "output/generateTLSE2Detectors.py",
    # "output/generateTLSE3Detectors.py",             NO CONFIG
    # "output/netdumpdiff.py",                        NO CONFIG
    # "output/netdumpmean.py",                        NO CONFIG
    "output/parkingSearchTraffic.py",
    "output/scheduleStats.py",
    # "output/statisticsElements.py",                 NO CONFIG
    "output/stopOrder.py",
    # "output/tables.py",                             NO CONFIG
    # "output/timingStats.py",                        NO CONFIG
    # "output/tripinfoByTAZ.py",                      NO CONFIG
    "output/tripinfoByType.py",
    # "output/tripinfoDiff.py",                       NO CONFIG
    # "output/tripStatistics.py",                     NO CONFIG
    "output/vehLanes.py",
    # "output/vehroute2amitranOD.py",                 NO CONFIG
    "output/vehrouteCountValidation.py",
    "output/vehrouteDiff.py",
    "output/walkFactor.py",

    # shapes
    # "shapes/circlePolygon.py",      NO CONFIG
    # "shapes/CSV2polyconvertXML.py", NO CONFIG
    # "shapes/poi_alongRoads.py",     NO CONFIG
    # "shapes/poi_at_stops.py",       NO CONFIG
    # "shapes/poi_atTLS.py",          NO CONFIG

    # tls
    # "tls/buildTransitions.py",      NO CONFIG
    # "tls/createTlsCsv.py",          NO CONFIG
    # "tls/tls_check.py",             NO CONFIG
    # "tls/tls_csv2SUMO.py",          NO CONFIG
    "tls/tls_csvSignalGroups.py",

    # turn-defs
    # "turn-defs/collectinghandler.py",       NO CONFIG
    # "turn-defs/connections.py",             NO CONFIG
    # "turn-defs/generateTurnDefs.py",        NO CONFIG
    "turn-defs/generateTurnRatios.py",
    "turn-defs/turnCount2EdgeCount.py",
    # "turn-defs/turndefinitions.py",         NO CONFIG
    "turn-defs/turnFile2EdgeRelations.py",

    # visualization
    # "visualization/macrOutput.py",                  NO CONFIG
    # "visualization/mpl_dump_onNet.py",              NO CONFIG
    # "visualization/mpl_dump_timeline.py",           NO CONFIG
    # "visualization/mpl_dump_twoAgainst.py",         NO CONFIG
    # "visualization/mpl_tripinfos_twoAgainst.py",    NO CONFIG
    # "visualization/plot_csv_bars.py",               NO CONFIG
    # "visualization/plot_csv_pie.py",                NO CONFIG
    # "visualization/plot_csv_timeline.py",           NO CONFIG
    "visualization/plot_net_dump.py",
    # "visualization/plot_net_selection.py",          NO CONFIG
    # "visualization/plot_net_speeds.py",             NO CONFIG
    # "visualization/plot_net_trafficLights.py",      NO CONFIG
    # "visualization/plot_summary.py",                NO CONFIG
    # "visualization/plot_tripinfo_distributions.py", NO CONFIG
    "visualization/plotXMLAttributes.py",

    # xml
    # "xml/addSchema.py",         NO CONFIG
    # "xml/binary2plain.py",      NO CONFIG
    "xml/csv2xml.py",
    "xml/xml2csv.py",
    "xml/changeAttribute.py",
    "xml/filterElements.py",
    # "xml/protobuf2xml.py",      NO CONFIG
    # "xml/xml2protobuf.py",      NO CONFIG
    # "xml/xsd.py",               NO CONFIG

    # other
    # "averageTripStatistics.py",       NO CONFIG
    "countEdgeUsage.py",
    # "createVehTypeDistribution.py",   NO CONFIG
    "edgesInDistricts.py",
    # "evacuateAreas.py",               NOT_SUITABLE
    "extractTest.py",
    "fcdReplay.py",
    "findAllRoutes.py",
    "generateBidiDistricts.py",
    # "generateContinuousRerouters.py", NO CONFIG
    "generateParkingAreaRerouters.py",
    "generateParkingAreas.py",
    # "generateParkingLots.py",         NO CONFIG
    "generateRailSignalConstraints.py",
    "generateRerouters.py",
    # "jtcrouter.py",                   NO CONFIG
    # "osmBuild.py",                    NOT_SUITABLE
    "osmGet.py",
    # "osmWebWizard.py",                NOT_WORKING_YET
    "plot_trajectories.py",
    # "ptlines2flows.py",               NO CONFIG
    "randomTrips.py",
    "routeSampler.py",
    "runSeeds.py",
    "stateReplay.py",
    "tileGet.py",
    "tlsCoordinator.py",
    "tlsCycleAdaptation.py",
    # "traceExporter.py",               NO CONFIG
]


SOURCE_DEPS = [
    "microsim/MSFrame.cpp", "microsim/devices/*.cpp",
    "utils/common/RandHelper.cpp", "utils/common/SystemFrame.cpp",
    "utils/geom/GeoConvHelper.cpp",
]


def buildTemplateToolHeader(templateHeaderFile):
    print(
        "#pragma once\n"
        "#include <string>\n"
        "#include <vector>\n"
        "\n"
        "// @brief template tool\n"
        "struct TemplateTool {\n"
        "\n"
        "    // @brief constructor\n"
        "    TemplateTool(const std::string name_, const std::string pythonPath_,\n"
        "        const std::string subfolder_, const std::string templateStr_) :\n"
        "        name(name_),\n"
        "        pythonPath(pythonPath_),\n"
        "        subfolder(subfolder_),\n"
        "        templateStr(templateStr_) {\n"
        "    }\n"
        "\n"
        "    // @brief tool name\n"
        "    const std::string name;\n"
        "\n"
        "    // @brief python path\n"
        "    const std::string pythonPath;\n"
        "\n"
        "    // @brief subfolder path\n"
        "    const std::string subfolder;\n"
        "\n"
        "    // @brief tool template\n"
        "    const std::string templateStr;\n"
        "};\n",
        file=templateHeaderFile)


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
        return '""'
    # replace all current directory values (src/netedit)
    templateStr = re.sub("(?<=value)(.*)(?=netedit)", "", templateStr)
    templateStr = templateStr.replace('netedit', '="')
    # replace " with \"
    templateStr = templateStr.replace('"', '\\"')
    # add quotes and end lines
    return templateStr.replace("<", '"<').replace("\n", '"\n')


def generateTemplate(app, appBin):
    """
    @brief generate template for the given app
    """
    if os.path.exists(appBin):
        # show info
        print("Obtaining " + app + " template")
        # obtain template piping stdout using check_output
        try:
            template = check_output([appBin, "--save-template", "stdout"], universal_newlines=True)
        except CalledProcessError as e:
            sys.stderr.write("Error when generating template for " + app + ": '%s'" % e)
            return 'const std::string ' + app + 'Template = "";'
        # join variable and formated template
        return 'const std::string ' + app + 'Template = "' + formatBinTemplate(template)
    # if binary wasn't found, then raise exception
    raise Exception(app + "template cannot be generated. " + app + " binary not found. "
                    "Make sure that " + app + " or " + app + "D was generated in bin folder")


def generateToolTemplate(toolDir, toolPath):
    """
    @brief generate tool template
    """
    toolName = os.path.basename(toolPath)[:-3]
    # create templateTool
    templateTool = 'TemplateTool("%s", "tools/%s", "%s",\n' % (toolName, toolPath, os.path.dirname(toolPath))
    # check if exists
    if os.path.exists(join(toolDir, toolPath)):
        # show info
        print("Obtaining '" + toolName + "' tool template.")
        # obtain template piping stdout using check_output
        try:
            with open(os.devnull, "w") as null:
                template = check_output([sys.executable, join(toolDir, toolPath), "--save-template", "stdout"],
                                        stderr=null, universal_newlines=True)
            # join variable and formated template
            return templateTool + formatToolTemplate(template) + '),\n'
        except CalledProcessError as e:
            print("Error when generating tool template for %s: '%s'." % (toolName, e), file=sys.stderr)
    else:
        print(toolName + "Template cannot be generated. '" + toolPath + "' not found.", file=sys.stderr)
    return templateTool + '""),\n'


def checkMod(toolDir, reference):
    mtime = os.path.getmtime(reference)
    for root, _, files in os.walk(toolDir):
        for f in files:
            if f[-3:] == ".py" and os.path.getmtime(join(root, f)) > mtime:
                return True
    srcDir = join(dirname(toolDir), 'src')
    for p in SOURCE_DEPS:
        for f in glob(join(srcDir, p)):
            if os.path.getmtime(f) > mtime:
                return True
    return False


def main():
    if len(sys.argv) != 3:
        sys.exit("Arguments: <pathToSumo> <pathToNetgenerate>")
    # get tool dir path (SUMO/tools)
    toolDir = join(dirname(__file__), '..')
    if not os.path.exists("templates.h") or checkMod(toolDir, "templates.h"):
        # write templates.h
        with open("templates.h", 'w') as templateHeaderFile:
            # generate templateTool header
            buildTemplateToolHeader(templateHeaderFile)
            # generate Tool templates
            print("const std::vector<TemplateTool> templateTools {\n", file=templateHeaderFile)
            for tool in TOOLS:
                print(generateToolTemplate(toolDir, tool), file=templateHeaderFile)
            print("};\n", file=templateHeaderFile)
            # generate sumo Template
            print(generateTemplate("sumo", sys.argv[1]), file=templateHeaderFile)
            # generate netgenerate Template
            print(generateTemplate("netgenerate", sys.argv[2]), file=templateHeaderFile)


if __name__ == "__main__":
    main()
