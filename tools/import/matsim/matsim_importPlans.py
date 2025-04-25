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

# @file    matsim_importPlans.py
# @author  Jakob Erdmann
# @author  Camillo Fillinger
# @date    2019-09-27

"""
Import person plans from MATSim
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess
from xml.sax import saxutils

try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools', 'route'))
import sumolib  # noqa
import sort_routes


def get_options(args=None):
    op = sumolib.options.ArgumentParser()
    op.add_argument("-p", "--plan-file", category="input", required=True,
                    type=op.data_file, help="MATSim plan file (mandatory)")
    op.add_argument("-o", "--output-file", category="input", required=True,
                    type=op.route_file, help="SUMO output route file (mandatory)")
    op.add_argument("-n", "--net-file", category="input", type=op.net_file,
                    help="SUMO net file (mandatory for --repair or --remove-loops)")
    op.add_argument("--vehicles-only", action="store_true", category="processing",
                    default=False, help="Import only vehicles instead of persons")
    op.add_argument("--no-bikes", action="store_true", category="processing",
                    default=False, help="do not import bike trips")
    op.add_argument("--no-rides", action="store_true", category="processing",
                    default=False, help="do not import ride trips")
    op.add_argument("--repair", action="store_true", category="processing",
                    default=False, help="Repair routes after import (needs a SUMO net)")
    op.add_argument("--remove-loops", action="store_true", category="processing",
                    default=False, help="Remove loops in routes after import (needs a SUMO net)")
    op.add_argument("--prefer-coordinate", action="store_true", category="processing",
                    default=False, help="Use coordinates instead of link ids if both are given")
    op.add_argument("--default-start", metavar="TIME", default="0:0:0", category="time",
                    help="default start time for the first activity")
    op.add_argument("--default-end", metavar="TIME", default="24:0:0", category="time",
                    help="default end time for the last activity")
    op.add_argument("--default-dur", metavar="TIME", default="1:0:0", category="time",
                    help="default duration for activities")
    op.add_argument("-v", "--verbose", action="store_true", category="processing",
                    default=False, help="tell me what you are doing")
    options = op.parse_args(args)

    if not options.net_file and (options.repair or options.remove_loops):
        op.print_help()
        sys.exit()

    return options


def getLocation(options, activity, attr, prj=None):
    if activity.link and (not options.prefer_coordinate or not activity.x):
        return '%s="%s"' % (attr, activity.link)
    elif activity.x and activity.y:
        if prj:
            lon, lat = prj(activity.x, activity.y, inverse=True)
            if attr == "edge":
                return 'lon="%s" lat="%s"' % (lon, lat)
            else:
                return '%sLonLat="%s,%s"' % (attr, lon, lat)
        else:
            if attr == "edge":
                return 'x="%s" y="%s"' % (activity.x, activity.y)
            else:
                return '%sXY="%s,%s"' % (attr, activity.x, activity.y)
    else:
        return None


def skipLeg(options, leg):
    # walk and public transport are not relevant
    if leg.mode.endswith("walk") or leg.mode == "pt":
        return True
    if options.no_rides and leg.mode == "ride":
        return True
    if options.no_bikes and leg.mode in ("bike", "bicycle"):
        return True
    return False


def writeLeg(outf, options, idveh, leg, start, end, types):
    """ Write the vehicles and trips. """
    if skipLeg(options, leg):
        return
    depart = leg.dep_time if options.vehicles_only else "triggered"
    mode = ' type="%s"' % leg.mode if leg.mode else ""
    if leg.mode:
        types.add(leg.mode)
    if leg.route is None or leg.route[0].distance == "NaN" or leg.mode in ("bike", "bicycle"):
        outf.write('    <trip id="%s" depart="%s" %s %s%s/>\n'
                   % (idveh, depart, start, end, mode))
    else:
        outf.write('    <vehicle id="%s" depart="%s"%s>\n' % (idveh, depart, mode))
        outf.write('        <route edges="%s"/>\n' % (leg.route[0].getText()))
        outf.write('    </vehicle>\n')


def main(options):

    prj = None
    for attributes in sumolib.xml.parse(options.plan_file, 'attributes'):
        for attribute in attributes.attribute:
            if attribute.attr_name == "coordinateReferenceSystem":
                projName = attribute.getText().strip()
                try:
                    import pyproj
                    try:
                        prj = pyproj.Proj("EPSG:31468" if projName == "GK4" else projName)
                    except pyproj.exceptions.CRSError as e:
                        print("Warning: Could not interpret coordinate system '%s' (%s)" % (
                            projName, e), file=sys.stderr)
                except ImportError:
                    print("Warning: install pyproj to support input with coordinates", file=sys.stderr)

    persons = []  # (depart, xmlsnippet)
    types = set()
    for index, person in enumerate(sumolib.xml.parse(options.plan_file, 'person')):
        outf = StringIO()
        vehIndex = 0
        plan = person.plan[0]
        if len(plan.getChildList()) == 0:
            continue
        firstAct = plan.getChildList()[0]
        depart = firstAct.start_time
        if depart is None:
            depart = options.default_start
        attributes = person.attributes[0] if person.attributes else None

        # write vehicles
        vehicleslist = []
        untillist = []
        durations = []
        lastAct = None
        lastLeg = None
        for item in plan.getChildList():
            leg = None
            idveh = "%s_%s" % (person.id, vehIndex)
            if "act" in item.name:  # act or activity
                if lastLeg is not None:
                    leg = lastLeg
                    leg.dep_time = lastAct.end_time
                    writeLeg(outf, options, idveh, leg,
                             getLocation(options, lastAct, "from", prj),
                             getLocation(options, item, "to", prj), types)
                    lastLeg = None
                # set missing end_time:
                if not item.end_time:
                    if item.start_time and item.max_dur:
                        item.end_time = sumolib.miscutils.humanReadableTime(
                            sumolib.miscutils.parseTime(item.start_time) +
                            sumolib.miscutils.parseTime(item.max_dur)
                        )
                    elif item.start_time:
                        item.end_time = sumolib.miscutils.humanReadableTime(
                            sumolib.miscutils.parseTime(item.start_time) +
                            sumolib.miscutils.parseTime(options.default_dur)
                        )
                    elif item.max_dur and leg:
                        item.end_time = sumolib.miscutils.humanReadableTime(
                            sumolib.miscutils.parseTime(leg.dep_time) +
                            sumolib.miscutils.parseTime(options.default_dur) +
                            sumolib.miscutils.parseTime(item.max_dur)
                        )
                lastAct = item
                durations.append(item.max_dur if item.max_dur else options.default_dur)
            if item.name == "leg":
                if item.route is None:
                    lastLeg = item
                else:
                    leg = item
                    start = 'from="%s"' % leg.route[0].start_link
                    end = 'to="%s"' % leg.route[0].end_link
                    writeLeg(outf, options, idveh, leg, start, end, types)
            if leg:
                untillist.append(leg.dep_time)
                vehicleslist.append(idveh if leg.mode != "pt" else "pt")
                vehIndex += 1
        untillist.append(lastAct.end_time if lastAct.end_time else options.default_end)

        # write person
        if not options.vehicles_only:
            vehIndex = 0
            actIndex = 0
            outf.write('    <person id="%s" depart="%s">\n' % (person.id, depart))
            if attributes is not None:
                for attr in attributes.attribute:
                    outf.write('        <param key="%s" value=%s/>\n' % (
                        attr.attr_name, saxutils.quoteattr(attr.getText())))

            lastLeg = None
            for item in plan.getChildList():
                if "act" in item.name:  # act or activity
                    end = getLocation(options, item, "to", prj)
                    if lastLeg is not None:
                        if lastLeg.mode == "non_network_walk":
                            pass
                            # outf.write('        <transship to="%s"/>\n' % item.link)
                        elif lastLeg.mode in ("walk", "transit_walk"):
                            outf.write('        <walk %s/>\n' % end)
                        elif lastLeg.mode == "pt":
                            outf.write('        <personTrip modes="public" %s/>\n' % end)
                        elif skipLeg(options, lastLeg):
                            outf.write('        <!-- ride lines="%s" %s mode="%s"/-->\n' % (
                                vehicleslist[vehIndex], end, lastLeg.mode))
                        else:
                            outf.write('        <ride lines="%s" %s/>\n' % (vehicleslist[vehIndex], end))
                        vehIndex += 1
                    until = untillist[vehIndex]
                    if until:
                        timing = 'until="%s"' % until
                    else:
                        timing = 'duration="%s"' % durations[actIndex]
                    outf.write('        <stop %s %s actType="%s"/>\n' %
                               (getLocation(options, item, "edge", prj), timing, item.type))
                    actIndex += 1
                if item.name == "leg":
                    lastLeg = item
            outf.write('    </person>\n')
        persons.append((sumolib.miscutils.parseTime(depart), index, outf.getvalue()))

    persons.sort()
    tmpout = options.output_file + ".tmp"
    with open(tmpout, 'w') as outf:
        sumolib.writeXMLHeader(outf, root="routes")
        for t in sorted(types):
            vClass = ""
            if t in ("bike", "bicycle"):
                vClass = ' vClass="bicycle"'
            outf.write('    <vType id="%s"%s/>\n' % (t, vClass))
        for depart, index, xml in persons:
            outf.write(xml)
        outf.write('</routes>\n')
    outf.close()
    # use duarouter for sorting when --vehicles-only is set
    if options.repair or options.remove_loops:
        args = ["-n", options.net_file, "-a", tmpout, "-o", options.output_file]
        args += ["--repair"] if options.repair else []
        args += ["--remove-loops"] if options.remove_loops else []
        args += ["--write-trips", "--write-trips.geo"] if options.prefer_coordinate else []
        subprocess.call([sumolib.checkBinary("duarouter")] + args)
    elif options.vehicles_only:
        sort_routes.main([tmpout, '-o', options.output_file])
    else:
        os.rename(tmpout, options.output_file)


if __name__ == "__main__":
    main(get_options())
