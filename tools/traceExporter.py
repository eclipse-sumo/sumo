#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    traceExporter.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @author  Evamarie Wiessner
# @author  Mirko Barthauer
# @date    2013-01-15


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import gzip
import random
import datetime
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'tools'))
sys.path.append(os.path.join(os.environ.get(
    "SUMO_HOME", os.path.join(os.path.dirname(__file__), '..', '..')), 'tools'))

from sumolib.miscutils import getSocketStream  # noqa
import sumolib.net  # noqa
from sumolib.options import ArgumentParser  # noqa
from sumolib.output.convert import phem, omnet, shawn, ns2, gpsdat, kml, gpx, poi, ipg, fcdfilter, keplerjson, trj  # noqa


class FCDTimeEntry:

    def __init__(self, t):
        self.time = t
        self.vehicle = []


def disturb_gps(x, y, deviation):
    if deviation == 0:
        return x, y
    x += random.gauss(0, deviation)
    y += random.gauss(0, deviation)
    return x, y


def _getOutputStream(name, binary=False):
    if not name:
        return None
    if name.endswith(".gz"):
        return gzip.open(name, "wt")
    return open(name, "wb" if binary else "w")


def _closeOutputStream(strm):
    if strm:
        strm.close()


def makeEntries(movables, chosen, options):
    if options.boundary:
        xmin, ymin, xmax, ymax = [float(e)
                                  for e in options.boundary.split(",")]
    result = []
    negative = False
    for v in movables:
        if v.id not in chosen:
            chosen[v.id] = random.random() < options.penetration
        if chosen[v.id]:
            v.x, v.y = disturb_gps(float(v.x), float(v.y), options.blur)
            if v.x < 0 or v.y < 0:
                if options.shift:
                    v.x = round(v.x + float(options.shift), 2)
                    v.y = round(v.y + float(options.shift), 2)
                else:
                    negative = True
            if not v.z:
                v.z = 0
            if not options.boundary or (v.x >= xmin and v.x <= xmax and v.y >= ymin and v.y <= ymax):
                if v.lane:
                    v.edge = sumolib._laneID2edgeID(v.lane)
                result.append(v)
    return result, negative


def procFCDStream(fcdstream, options):
    pt = -1  # "prior" time step
    lt = -1  # "last" time step
    lastExported = -1
    chosen = {}
    hasWarning = False
    for q in fcdstream:
        pt = lt
        lt = sumolib.miscutils.parseTime(q.time)
        if options.begin and options.begin > lt:
            continue  # do not export steps before a set begin
        if options.end and options.end <= lt:
            continue  # do not export steps after a set end
        if lastExported >= 0 and (options.delta and options.delta + lastExported > lt):
            continue  # do not export between delta-t, if set
        lastExported = lt
        e = FCDTimeEntry(lt)
        if q.vehicle:
            result, warning = makeEntries(q.vehicle, chosen, options)
            e.vehicle += result
            hasWarning = hasWarning or warning
        if options.persons and q.person:
            result, warning = makeEntries(q.person, chosen, options)
            e.vehicle += result
            hasWarning = hasWarning or warning
        yield e
    t = lt - pt + lt
    if hasWarning:
        print("One or more coordinates are negative, some applications might need strictly positive values. " +
              "To avoid this use the option --shift.")
    yield FCDTimeEntry(t)


def runMethod(inputFile, outputFile, writer, options, further={}):
    further["app"] = os.path.split(__file__)[1]
    further["orig-ids"] = options.orig_ids
    further["ignore-gaps"] = options.ignore_gaps
    if options.base >= 0:
        further["base-date"] = datetime.datetime.fromtimestamp(options.base, tz=datetime.timezone.utc)
    else:
        further["base-date"] = datetime.datetime.now().replace(hour=0,
                                                               minute=0, second=0, microsecond=0)
    binaryOutput = True if options.trj else False
    o = _getOutputStream(outputFile, binary=binaryOutput)
    if inputFile.isdigit():
        inp = getSocketStream(int(inputFile))
    else:
        inp = inputFile
    fcdStream = sumolib.output.parse(inp, "timestep")
    ret = writer(procFCDStream(fcdStream, options), o, further)
    _closeOutputStream(o)
    options._had_output = True
    return ret


def main(args=None):
    """The main function; parses options and converts..."""
    # ---------- build and read options ----------
    description = """Converts the given fcd file (generated by sumo --fcd-output) into the selected
output format. Optionally the output can be sampled, filtered and distorted.
"""

    optParser = ArgumentParser(description=description)
    optParser.add_option("-i", "--fcd-input", dest="fcd", metavar="FILE", required=True,
                         help="Defines the FCD-output file to use as input " +
                              "(numeric value is interpreted as port to listen on)")
    optParser.add_option("-n", "--net-input", dest="net", metavar="FILE",
                         help="Defines the network file to use as input")
    optParser.add_option("-p", "--penetration", type=float, dest="penetration",
                         default=1., help="Defines the percentage (0-1) of vehicles to export")
    optParser.add_option("-b", "--begin", dest="begin",
                         type=float, help="Defines the first step to export")
    optParser.add_option("-e", "--end", dest="end",
                         type=float, help="Defines the first step not longer to export")
    optParser.add_option("-d", "--delta-t", dest="delta",
                         type=float, help="Defines the export step length")
    optParser.add_option("--gps-blur", dest="blur", default=0,
                         type=float, help="Defines the GPS blur")
    optParser.add_option(
        "--boundary", help="Defines the bounding box as 'xmin,ymin,xmax,ymax'")
    optParser.add_option("-s", "--seed", dest="seed", default=42,
                         type=float, help="Defines the randomizer seed")
    optParser.add_option(
        "--base-date", dest="base", default=-1, type=int, help="Defines the base date")
    optParser.add_option("--orig-ids", dest="orig_ids", default=False, action="store_true",
                         help="Write original vehicle IDs instead of a running index")
    optParser.add_option("--ignore-gaps", dest="ignore_gaps", default=False, action="store_true",
                         help="Ignore steps where a vehicle is not in the network")
    optParser.add_option("--persons", default=False, action="store_true",
                         help="Include person data")
    # PHEM
    optParser.add_option("--dri-output", dest="dri", metavar="FILE",
                         help="Defines the name of the PHEM .dri-file to generate")
    optParser.add_option("--str-output", dest="str", metavar="FILE",
                         help="Defines the name of the PHEM .str-file to generate")
    optParser.add_option("--fzp-output", dest="fzp", metavar="FILE",
                         help="Defines the name of the PHEM .fzp-file to generate")
    optParser.add_option("--flt-output", dest="flt", metavar="FILE",
                         help="Defines the name of the PHEM .flt-file to generate")
    # OMNET
    optParser.add_option("--omnet-output", dest="omnet", metavar="FILE",
                         help="Defines the name of the OMNET file to generate")
    # Shawn
    optParser.add_option("--shawn-output", dest="shawn", metavar="FILE",
                         help="Defines the name of the Shawn file to generate")
    # ns2
    optParser.add_option("--ns2activity-output", dest="ns2activity", metavar="FILE",
                         help="Defines the name of the ns2 file to generate")
    optParser.add_option("--ns2config-output", dest="ns2config", metavar="FILE",
                         help="Defines the name of the ns2 file to generate")
    optParser.add_option("--ns2mobility-output", dest="ns2mobility", metavar="FILE",
                         help="Defines the name of the ns2 file to generate")
    # GPSDAT
    optParser.add_option("--gpsdat-output", dest="gpsdat", metavar="FILE",
                         help="Defines the name of the gpsdat file to generate")
    # KML
    optParser.add_option("--kml-output", dest="kml", metavar="FILE",
                         help="Defines the name of the kml file to generate")
    # GPX
    optParser.add_option("--gpx-output", dest="gpx", metavar="FILE",
                         help="Defines the name of the gpx file to generate")
    # POI
    optParser.add_option("--poi-output", dest="poi", metavar="FILE",
                         help="Defines the name of the poi file to generate")
    # FCD
    optParser.add_option("--fcd-filter", dest="fcdfilter", metavar="FILE",
                         help="Defines the name of the filter definition file")
    # kepler.gl JSON
    optParser.add_option("--keplerjson-output", dest="keplerjson", metavar="FILE",
                         help="Defines the name of the kelper.gl JSON file to generate")
    optParser.add_option("--fcd-filter-comment", dest="fcdcomment",
                         help="Extra comments to include in fcd file")
    optParser.add_option("--fcd-filter-type", dest="fcdtype",
                         help="vehicle type to include in fcd file")
    optParser.add_option("--shift", dest="shift",
                         help="shift coordinates to positive values only")
    # IPG
    optParser.add_option("--ipg-output", dest="ipg", metavar="FILE",
                         help="Defines the name of the ipg trace file to generate")

    # TRJ
    optParser.add_option("--trj-output", dest="trj", metavar="FILE",
                         help="Defines the name of the trj file to generate")
    optParser.add_option("--trj-veh-width", dest="trjvehwidth", default=1.7,
                         type=float, help="Defines the assumed vehicle width")
    optParser.add_option("--trj-veh-length", "--trj-vehicle-length", dest="trjvehlength", default=4.8,
                         type=float, help="Defines the assumed vehicle length")
    optParser.add_option("--timestep", dest="timestep", default=1.0,
                         type=float, help="Used time step duration")

    # parse
    options = optParser.parse_args(args=args)

    options._had_output = False

    if options.seed:
        random.seed(options.seed)
    # ---------- process ----------
    net = None
    # ----- check needed values
    if options.delta and options.delta <= 0:
        print("delta-t must be a positive value.")
        return 1
    # phem
    if (options.dri or options.fzp or options.flt) and not options.fcd:
        print("A fcd-output from SUMO must be given using the --fcd-input.")
        return 1
    if (options.str or options.fzp or options.flt) and not options.net:
        print("A SUMO network must be given using the --net-input option.")
        return 1
    # omnet
    if options.omnet and not options.fcd:
        print("A fcd-output from SUMO must be given using the --fcd-input.")
        return 1
    # trj
    if options.trj and not options.fcd:
        print("A fcd-output from SUMO must be given using the --fcd-input.")
        return 1
    if options.trj and not options.net:
        print("A SUMO network must be given using the --net-input option.")
        return 1
    # ----- check needed values

    # ----- OMNET
    if options.omnet:
        runMethod(options.fcd, options.omnet, omnet.fcd2omnet, options)
    # ----- OMNET

    # ----- Shawn
    if options.shawn:
        runMethod(options.fcd, options.shawn, shawn.fcd2shawn, options)
    # ----- Shawn

    # ----- GPSDAT
    if options.gpsdat:
        runMethod(options.fcd, options.gpsdat, gpsdat.fcd2gpsdat, options)
    # ----- GPSDAT

    # ----- KML
    if options.kml:
        runMethod(options.fcd, options.kml, kml.fcd2kml, options)
    # ----- KML

    # ----- GPX
    if options.gpx:
        runMethod(options.fcd, options.gpx, gpx.fcd2gpx, options)
    # ----- GPX

    # ----- POI
    if options.poi:
        runMethod(options.fcd, options.poi, poi.fcd2poi, options)
    # ----- POI

    # ----- FCD
    if options.fcdfilter:
        runMethod(options.fcd, None, fcdfilter.fcdfilter, options,
                  {"filter": options.fcdfilter, "comment": options.fcdcomment, "type": options.fcdtype})
    # ----- FCD

    # ----- kepler.gl JSON
    if options.keplerjson:
        runMethod(options.fcd, options.keplerjson, keplerjson.fcd2keplerjson, options)
    # ----- kepler.gl JSON

    # ----- ns2
    if options.ns2mobility or options.ns2config or options.ns2activity:
        vIDm, vehInfo, begin, end, area = runMethod(
            options.fcd, options.ns2mobility, ns2.fcd2ns2mobility, options)
    if options.ns2activity:
        o = _getOutputStream(options.ns2activity)
        ns2.writeNS2activity(o, vehInfo)
        _closeOutputStream(o)
    if options.ns2config:
        o = _getOutputStream(options.ns2config)
        ns2.writeNS2config(
            o, vehInfo, options.ns2activity, options.ns2mobility, begin, end, area)
        _closeOutputStream(o)
    # ----- ns2

    # ----- PHEM
    # .dri
    if options.dri:
        runMethod(options.fcd, options.dri, phem.fcd2dri, options)
    # .str (we need the net for other outputs, too)
    if options.str or options.fzp or options.flt:
        if not options.net:
            print("A SUMO network must be given using the --net-input option.")
            return 1
        if not net:
            net = sumolib.net.readNet(options.net)
        o = _getOutputStream(options.str)
        sIDm = phem.net2str(net, o)
        _closeOutputStream(o)
    # .fzp
    if options.flt or options.fzp:
        vIDm, vtIDm = runMethod(
            options.fcd, options.fzp, phem.fcd2fzp, options, {"phemStreetMap": sIDm})
    # .flt
    if options.flt:
        o = _getOutputStream(options.flt)
        phem.vehicleTypes2flt(o, vtIDm)
        _closeOutputStream(o)
    # ----- PHEM

    # ----- IPG
    if options.ipg:
        runMethod(options.fcd, options.ipg, ipg.fcd2ipg, options)
    # ----- IPG

    # ----- TRJ
    if options.trj:
        if not net:
            net = sumolib.net.readNet(options.net)
        runMethod(options.fcd, options.trj, trj.fcd2trj, options,
                  {"length": options.trjvehlength, "width": options.trjvehwidth,
                   "bbox": net.getBBoxXY(), "timestep": options.timestep})
    # ----- TRJ

    if not options._had_output:
        print("Warning: No output option specified", file=sys.stderr)

    return 0


if __name__ == "__main__":
    sys.exit(main())
