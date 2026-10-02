#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    gtfs2fcd.py
# @author  Michael Behrisch
# @author  Robert Hilbrich
# @author  Mirko Barthauer
# @date    2018-06-13

"""
Converts GTFS data into separate fcd traces for every distinct trip
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import io
from xml.sax import saxutils
from collections import defaultdict, namedtuple
import pandas as pd

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
from sumolib.miscutils import humanReadableTime  # noqa
import traceExporter  # noqa
import gtfsutils  # noqa
from gtfsutils import OSM2SUMO_MODES, GTFS2OSM_MODES  # noqa


def add_options():
    op = sumolib.options.ArgumentParser(
        description="converts GTFS data into separate fcd traces for every distinct trip")
    op.add_argument("-r", "--region", default="gtfs", category="input",
                    help="define the region to process")
    gp = op.add_mutually_exclusive_group(required=True)
    gp.add_argument("--gtfs", category="input", type=op.data_file, fix_path=True,
                    help="define gtfs zip file to load (mandatory)")
    gp.add_argument("--merged-csv", category="input", type=op.data_file, dest="mergedCSV", fix_path=True,
                    help="define csv file for loading merged data (instead of gtfs data)")
    op.add_argument("--merged-csv-output", category="output", type=op.data_file, dest="mergedCSVOutput",
                    help="define csv file for saving merged GTFS data")
    op.add_argument("--date", category="input", required=False, help="define the day to import, format: 'YYYYMMDD'")
    op.add_argument("--fcd", category="input", type=op.data_file,
                    help="directory to write / read the generated FCD files to / from")
    op.add_argument("--gpsdat", category="input", type=op.data_file,
                    help="directory to write / read the generated gpsdat files to / from")
    op.add_argument("--modes", category="input", help="comma separated list of modes to import (%s)" %
                    (", ".join(OSM2SUMO_MODES.keys())))
    op.add_argument("--sbahn-is-light-rail", action="store_true", default=False, dest="sbahnLR", category="input",
                    help="interpret GTFS mode 109 (S-Bahn) as light_rail instead of train (Berlin, Hamburg)")
    op.add_argument("--vtype-output", default="vtypes.xml", category="output", type=op.file,
                    help="file to write the generated vehicle types to")
    op.add_argument("--write-terminals", action="store_true", default=False,
                    dest="writeTerminals", category="processing",
                    help="Write vehicle parameters that describe terminal stops and times")
    op.add_argument("--original-lines", action="store_true", default=False,
                    dest="origLines", category="processing",
                    help="Do not distinguish line ids that have distinct stop sequences")
    op.add_argument("--join-blocks", action="store_true", default=False, dest="joinBlocks",
                    help="Do not concatenate trips by block_id")
    op.add_argument("-H", "--human-readable-time", category="output", dest="hrtime", default=False, action="store_true",
                    help="write times as h:m:s")
    op.add_argument("-v", "--verbose", action="store_true", default=False,
                    category="processing", help="tell me what you are doing")
    op.add_argument("-b", "--begin", default=0, category="time", type=op.time,
                    help="Defines the begin time to export")
    op.add_argument("-e", "--end", default=86400, category="time", type=op.time,
                    help="Defines the end time for the export")
    op.add_argument("--bbox", category="input", help="define the bounding box to filter the gtfs data, format: W,S,E,N")
    return op


def check_options(options):
    if options.fcd is None:
        options.fcd = os.path.join('fcd', options.region)
    if options.gpsdat is None:
        options.gpsdat = os.path.join('input', options.region)
    if options.modes is None:
        options.modes = ",".join(OSM2SUMO_MODES.keys())
    if options.gtfs and not options.date:
        raise ValueError("When option --gtfs is set, option --date must be set as well")
    options.ft = humanReadableTime if options.hrtime else lambda x: x

    return options


def dataAvailable(options):
    for mode in options.modes.split(","):
        if os.path.exists(os.path.join(options.fcd, "%s.fcd.xml" % mode)):
            return True
    return False


def groupRoutes(options, full_data_merged):
    Stop = namedtuple("Stop", ["lon", "lat", "arrival", "until", "name", "gtfsid",
                               "block", "fareZone", "fareSymbol", "startFare", "fcdtime"])
    vehicles = defaultdict(list)  # mode -> [(trip_id, route, type, depart, line, params), ...]
    routes = defaultdict(lambda: defaultdict(list))  # mode -> trip_id -> [Stop, ...]

    modes = options.modes.split(",")
    timeIndex = 0
    lines = set()  # unique line ids
    for _, trip_data in full_data_merged.groupby('route_id'):
        seqs = {}  # stop sequence -> routeID, lineID
        for trip_id, data in trip_data.groupby('trip_id'):
            stopSeq = []
            currentRoute = []
            offset = 0
            firstDep = None
            firstStop = None
            lastIndex = None
            lastArrival = None
            lastStop = None
            for idx, d in data.sort_values(by=['stop_sequence']).iterrows():
                mode = GTFS2OSM_MODES[d.route_type]
                if mode not in modes:
                    continue

                if d.stop_sequence == lastIndex:
                    print("Invalid stop_sequence in input for trip %s" % trip_id, file=sys.stderr)
                if lastArrival is not None:
                    if d.arrival_time < lastArrival:
                        print("Warning! Stop %s for vehicle %s starts earlier (%s) than previous stop (%s)" % (
                            idx, trip_id, d.arrival_time, lastArrival), file=sys.stderr)
                lastArrival = d.arrival_time
                lastStop = d.stop_name

                arrivalSec = d.arrival_time + timeIndex
                departureSec = d.departure_time + timeIndex
                until = 0 if firstDep is None else departureSec - timeIndex - firstDep
                arrival = 0 if firstDep is None else arrivalSec - timeIndex - firstDep
                stopSeq.append((d.stop_id, until))
                currentRoute.append(Stop(
                    lon=d.stop_lon,
                    lat=d.stop_lat,
                    arrival=arrival,
                    until=until,
                    name=saxutils.escape(d.stop_name),
                    gtfsid=saxutils.escape(d.stop_id),
                    block="" if not options.joinBlocks or pd.isna(d.block_id) else d.block_id,
                    fareZone=d.fare_zone,
                    fareSymbol=d.fare_token,
                    startFare=d.start_char,
                    fcdtime=arrivalSec - offset))
                if firstDep is None:
                    firstDep = departureSec - timeIndex
                    firstStop = d.stop_name
                offset += departureSec - arrivalSec
                lastIndex = d.stop_sequence
            mode = GTFS2OSM_MODES[d.route_type]
            if mode in modes:
                stopSeq = tuple(stopSeq)
                if stopSeq not in seqs:
                    lineID = d.route_short_name.replace(" ", "_")
                    if not options.origLines:
                        baseLine = lineID
                        i = 0
                        while lineID in lines:
                            i += 1
                            lineID = "%s#%s" % (baseLine, i)
                    lines.add(lineID)
                    seqs[stopSeq] = trip_id, lineID
                    routes[mode][trip_id] = currentRoute
                    timeIndex = arrivalSec
                # The `line` attribute shall hold the line short name that can be used to determine person rides
                # as per https://sumo.dlr.de/docs/Specification/Persons.html#rides
                # The spaces in the route name are replaced by underscores to allow for space-separated lists of lines.
                routeID, lineID = seqs[stopSeq]
                params = [("gtfs.route_name", d.route_short_name)]
                if d.trip_headsign:
                    params.append(("gtfs.trip_headsign", d.trip_headsign))
                if options.writeTerminals:
                    params += [("gtfs.origin_stop", firstStop),
                               ("gtfs.origin_depart", options.ft(firstDep)),
                               ("gtfs.destination_stop", lastStop),
                               ("gtfs.destination_arrrival", options.ft(lastArrival))]
                vehicles[mode].append((trip_id, routeID, firstDep, lineID, params))

    return vehicles, routes


def writeFCD(options, routes):
    for mode in routes.keys():
        filePrefix = os.path.join(options.fcd, mode)
        fcdFile = io.open(filePrefix + '.fcd.xml', 'w', encoding="utf8")
        sumolib.writeXMLHeader(fcdFile, "gtfs2fcd.py", options=options)
        fcdFile.write(u'<fcd-export>\n')
        if options.verbose:
            print('Writing fcd file "%s"' % fcdFile.name)
        for trip_id, locations in routes[mode].items():
            for s in locations:
                fcdFile.write((u'    <timestep time="%s"><vehicle id="%s" x="%s" y="%s" until="%s" name="%s" ' +
                               u'gtfsid="%s" block="%s" fareZone="%s" fareSymbol="%s" startFare="%s" speed="20"/>' +
                               u'</timestep>\n') % (s.fcdtime, trip_id, s.lon, s.lat, s.until, s.name,
                                                    s.gtfsid, s.block, s.fareZone, s.fareSymbol, s.startFare))
        fcdFile.write(u'</fcd-export>\n')
        fcdFile.close()

        if options.gpsdat:
            if not os.path.exists(options.gpsdat):
                os.makedirs(options.gpsdat)
            traceExporter.main(['--base-date', '0', '-i', fcdFile.name,
                                '--gpsdat-output', os.path.join(options.gpsdat, "gpsdat_%s.csv" % mode)])


def writeVehicles(options, vehicles):
    for mode in vehicles.keys():
        filePrefix = os.path.join(options.fcd, mode)
        tripFile = io.open(filePrefix + '.rou.xml', 'w', encoding="utf8")
        tripFile.write(u"<routes>\n")
        for trip_id, routeID, depart, lineID, params in vehicles[mode]:
            tripFile.write(u'    <vehicle id="%s" route="%s" type="%s" depart="%s" line="%s">\n' % (
                trip_id, routeID, mode, depart, lineID))
            for k, v in params:
                tripFile.write(u'        <param key="%s" value=%s/>\n' % (k, sumolib.xml.quoteattr(str(v), True)))
            tripFile.write(u'    </vehicle>\n')
        tripFile.write(u"</routes>\n")
        tripFile.close()


def main(options):
    full_data_merged = gtfsutils.loadGTFS(options)
    if full_data_merged.empty:
        return False
    vehicles, routes = groupRoutes(options, full_data_merged)
    if routes:
        if not os.path.exists(options.fcd):
            os.makedirs(options.fcd)
        writeFCD(options, routes)
        writeVehicles(options, vehicles)
        gtfsutils.write_vtypes(options, sorted(vehicles.keys()))
    return True


if __name__ == "__main__":
    main(check_options(add_options().parse_args()))
