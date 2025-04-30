#!/usr/bin/env python3
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
import pandas as pd
import zipfile

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
from sumolib.miscutils import humanReadableTime  # noqa
import traceExporter  # noqa
import gtfs2osm  # noqa


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
                    (", ".join(gtfs2osm.OSM2SUMO_MODES.keys())))
    op.add_argument("--vtype-output", default="vtypes.xml", category="output", type=op.file,
                    help="file to write the generated vehicle types to")
    op.add_argument("--write-terminals", action="store_true", default=False,
                    dest="writeTerminals", category="processing",
                    help="Write vehicle parameters that describe terminal stops and times")
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
        options.modes = ",".join(gtfs2osm.OSM2SUMO_MODES.keys())
    if options.gtfs and not options.date:
        raise ValueError("When option --gtfs is set, option --date must be set as well")

    return options


def time2sec(s):
    t = s.split(":")
    return int(t[0]) * 3600 + int(t[1]) * 60 + int(t[2])


def get_merged_data(options):
    gtfsZip = zipfile.ZipFile(sumolib.openz(options.gtfs, mode="rb", tryGZip=False, printErrors=True))
    routes, trips_on_day, shapes, stops, stop_times = gtfs2osm.import_gtfs(options, gtfsZip)
    gtfsZip.fp.close()

    if options.bbox:
        stops['stop_lat'] = stops['stop_lat'].astype(float)
        stops['stop_lon'] = stops['stop_lon'].astype(float)
        stops = stops[(options.bbox[1] <= stops['stop_lat']) & (stops['stop_lat'] <= options.bbox[3]) &
                      (options.bbox[0] <= stops['stop_lon']) & (stops['stop_lon'] <= options.bbox[2])]
    stop_times['arrival_time'] = stop_times['arrival_time'].map(time2sec)
    stop_times['departure_time'] = stop_times['departure_time'].map(time2sec)

    if 'fare_stops.txt' in gtfsZip.namelist():
        zones = pd.read_csv(gtfsZip.open('fare_stops.txt'), dtype=str)
        stops_merged = pd.merge(pd.merge(stops, stop_times, on='stop_id'), zones, on='stop_id')
    else:
        stops_merged = pd.merge(stops, stop_times, on='stop_id')
        stops_merged['fare_zone'] = ''
        stops_merged['fare_token'] = ''
        stops_merged['start_char'] = ''

    trips_routes_merged = pd.merge(trips_on_day, routes, on='route_id')
    merged = pd.merge(stops_merged, trips_routes_merged,
                      on='trip_id')[['trip_id', 'route_id', 'route_short_name', 'route_type',
                                     'stop_id', 'stop_name', 'stop_lat', 'stop_lon', 'stop_sequence',
                                     'fare_zone', 'fare_token', 'start_char', 'trip_headsign',
                                     'arrival_time', 'departure_time']].drop_duplicates()
    return merged


def dataAvailable(options):
    for mode in options.modes.split(","):
        if os.path.exists(os.path.join(options.fcd, "%s.fcd.xml" % mode)):
            return True
    return False


def main(options):
    ft = humanReadableTime if options.hrtime else lambda x: x
    if options.mergedCSV:
        full_data_merged = pd.read_csv(options.mergedCSV, sep=";",
                                       keep_default_na=False,
                                       dtype={"route_type": str})
    else:
        full_data_merged = get_merged_data(options)
    if options.mergedCSVOutput:
        full_data_merged.sort_values(by=['trip_id', 'stop_sequence'], inplace=True)
        full_data_merged.to_csv(options.mergedCSVOutput, sep=";", index=False)
    if full_data_merged.empty:
        return False
    fcdFile = {}
    tripFile = {}
    if not os.path.exists(options.fcd):
        os.makedirs(options.fcd)
    seenModes = set()
    modes = set(options.modes.split(","))
    for mode in modes:
        filePrefix = os.path.join(options.fcd, mode)
        fcdFile[mode] = io.open(filePrefix + '.fcd.xml', 'w', encoding="utf8")
        sumolib.writeXMLHeader(fcdFile[mode], "gtfs2fcd.py", options=options)
        fcdFile[mode].write(u'<fcd-export>\n')
        if options.verbose:
            print('Writing fcd file "%s"' % fcdFile[mode].name)
        tripFile[mode] = io.open(filePrefix + '.rou.xml', 'w', encoding="utf8")
        tripFile[mode].write(u"<routes>\n")
    timeIndex = 0
    for _, trip_data in full_data_merged.groupby('route_id'):
        seqs = {}
        for trip_id, data in trip_data.groupby('trip_id'):
            stopSeq = []
            buf = u""
            offset = 0
            firstDep = None
            firstStop = None
            lastIndex = None
            lastArrival = None
            lastStop = None
            for idx, d in data.sort_values(by=['stop_sequence']).iterrows():
                if d.stop_sequence == lastIndex:
                    print("Invalid stop_sequence in input for trip %s" % trip_id, file=sys.stderr)
                if lastArrival is not None:
                    if d.arrival_time < lastArrival:
                        print("Warning! Stop %s for vehicle %s starts earlier (%s) than previous stop (%s)" % (
                            idx, trip_id, d.arrival_time, lastArrival), file=sys.stderr)
                lastArrival = d.arrival_time
                lastStop = d.stop_name

                arrivalSec = d.arrival_time + timeIndex
                stopSeq.append(d.stop_id)
                departureSec = d.departure_time + timeIndex
                until = 0 if firstDep is None else departureSec - timeIndex - firstDep
                buf += ((u'    <timestep time="%s"><vehicle id="%s" x="%s" y="%s" until="%s" ' +
                         u'name=%s fareZone="%s" fareSymbol="%s" startFare="%s" speed="20"/></timestep>\n') %
                        (arrivalSec - offset, trip_id, d.stop_lon, d.stop_lat, until,
                         sumolib.xml.quoteattr(d.stop_name, True), d.fare_zone, d.fare_token, d.start_char))
                if firstDep is None:
                    firstDep = departureSec - timeIndex
                    firstStop = d.stop_name
                offset += departureSec - arrivalSec
                lastIndex = d.stop_sequence
            mode = gtfs2osm.GTFS2OSM_MODES[d.route_type]
            if mode in modes:
                s = tuple(stopSeq)
                if s not in seqs:
                    seqs[s] = trip_id
                    fcdFile[mode].write(buf)
                    timeIndex = arrivalSec
                tripFile[mode].write(u'    <vehicle id="%s" route="%s" type="%s" depart="%s" line="%s">\n' %
                                     (trip_id, seqs[s], mode, firstDep, seqs[s]))
                params = [("gtfs.route_name", d.route_short_name)]
                if d.trip_headsign:
                    params.append(("gtfs.trip_headsign", d.trip_headsign))
                if options.writeTerminals:
                    params += [("gtfs.origin_stop", firstStop),
                               ("gtfs.origin_depart", ft(firstDep)),
                               ("gtfs.destination_stop", lastStop),
                               ("gtfs.destination_arrrival", ft(lastArrival))]
                for k, v in params:
                    tripFile[mode].write(u'        <param key="%s" value=%s/>\n' % (
                        k, sumolib.xml.quoteattr(str(v), True)))
                tripFile[mode].write(u'    </vehicle>\n')
                seenModes.add(mode)
    for mode in modes:
        fcdFile[mode].write(u'</fcd-export>\n')
        fcdFile[mode].close()
        tripFile[mode].write(u"</routes>\n")
        tripFile[mode].close()
        if mode not in seenModes:
            os.remove(fcdFile[mode].name)
            os.remove(tripFile[mode].name)
    if options.gpsdat:
        if not os.path.exists(options.gpsdat):
            os.makedirs(options.gpsdat)
        for mode in modes:
            if mode in seenModes:
                traceExporter.main(['--base-date', '0', '-i', fcdFile[mode].name,
                                    '--gpsdat-output', os.path.join(options.gpsdat, "gpsdat_%s.csv" % mode)])
    if dataAvailable(options):
        gtfs2osm.write_vtypes(options, seenModes)
    return True


if __name__ == "__main__":
    main(check_options(add_options().parse_args()))
