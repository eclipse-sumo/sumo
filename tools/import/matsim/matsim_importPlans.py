#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2023 German Aerospace Center (DLR) and others.
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
try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    argParser = sumolib.options.ArgumentParser()
    argParser.add_argument("-p", "--plan-file", help="MATSim plan file (mandatory)")
    argParser.add_argument("-o", "--output-file", help="SUMO output route file (mandatory)")
    argParser.add_argument("-n", "--net-file", help="SUMO net file (mandatory for --repair or --remove-loops)")
    argParser.add_argument("--vehicles-only", action="store_true",
                           default=False, help="Import only vehicles instead of persons")
    argParser.add_argument("--repair", action="store_true",
                           default=False, help="Repair routes after import (needs a SUMO net)")
    argParser.add_argument("--remove-loops", action="store_true",
                           default=False, help="Remove loops in routes after import (needs a SUMO net)")
    argParser.add_argument("--default-start", metavar="TIME", default="0:0:0",
                           help="default start time for the first activity")
    argParser.add_argument("--default-end", metavar="TIME", default="24:0:0",
                           help="default end time for the last activity")
    argParser.add_argument("-v", "--verbose", action="store_true",
                           default=False, help="tell me what you are doing")
    options = argParser.parse_args(args)

    if not options.plan_file or not options.output_file:
        argParser.print_help()
        sys.exit()

    if not options.net_file and (options.repair or options.remove_loops):
        argParser.print_help()
        sys.exit()

    return options


def writeLeg(outf, options, idveh, leg, startLink, endLink):
    depart = leg.dep_time if options.vehicles_only else "triggered"
    mode = ' type="%s"' % leg.mode if leg.mode in ("car", "bicycle") else ""
    if leg.route is None or leg.route[0].distance == "NaN" or leg.mode == "bicycle":
        outf.write('    <trip id="%s" depart="%s" from="%s" to="%s"%s/>\n'
                   % (idveh, depart, startLink, endLink, mode))
    elif not leg.mode.endswith("walk") and leg.mode != "pt":
        outf.write('    <vehicle id="%s" depart="%s"%s>\n' % (idveh, depart, mode))
        outf.write('        <route edges="%s"/>\n' % (leg.route[0].getText()))
        outf.write('    </vehicle>\n')


def main(options):
    persons = []  # (depart, xmlsnippet)
    for person in sumolib.xml.parse(options.plan_file, 'person'):
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
        lastAct = None
        lastLeg = None
        for item in plan.getChildList():
            leg = None
            idveh = "%s_%s" % (person.id, vehIndex)
            if "act" in item.name:  # act or activity
                if lastLeg is not None:
                    leg = lastLeg
                    leg.dep_time = lastAct.end_time
                    writeLeg(outf, options, idveh, leg, lastAct.link, item.link)
                    lastLeg = None
                lastAct = item
            if item.name == "leg":
                if item.route is None:
                    lastLeg = item
                else:
                    leg = item
                    writeLeg(outf, options, idveh, leg, leg.route[0].start_link, leg.route[0].end_link)
            if leg:
                untillist.append(leg.dep_time)
                vehicleslist.append(idveh)
                vehIndex = vehIndex+1
        untillist.append(lastAct.end_time if lastAct.end_time else options.default_end)
        # write person
        if not options.vehicles_only:
            vehIndex = 0
            outf.write('    <person id="%s" depart="%s">\n' % (person.id, depart))
            if attributes is not None:
                for attr in attributes.attribute:
                    outf.write('        <param key="%s" value="%s"/>\n' % (attr.attr_name, attr.getText()))

            lastLeg = None
            for item in plan.getChildList():
                if "act" in item.name:  # act or activity
                    if lastLeg is not None:
                        if lastLeg.mode == "non_network_walk":
                            pass
                            # outf.write('        <transship to="%s"/>\n' % item.link)
                        elif lastLeg.mode in ("walk", "transit_walk"):
                            outf.write('        <walk to="%s"/>\n' % item.link)
                        else:
                            outf.write('        <ride lines="%s" to="%s"/>\n' % (vehicleslist[vehIndex], item.link))
                        vehIndex = vehIndex+1
                    outf.write('        <stop lane="%s_0" until="%s" actType="%s"/>\n' %
                               (item.link, untillist[vehIndex], item.type))
                if item.name == "leg":
                    lastLeg = item
            outf.write('    </person>\n')
        persons.append((sumolib.miscutils.parseTime(depart), outf.getvalue()))

    persons.sort()
    with open(options.output_file, 'w') as outf:
        sumolib.writeXMLHeader(outf, root="routes")
        outf.write('    <vType id="car" vClass="passenger"/>\n    <vType id="bicycle" vClass="bicycle"/>\n\n')
        for depart, xml in persons:
            outf.write(xml)
        outf.write('</routes>\n')
    outf.close()
    if options.repair or options.remove_loops:
        args = ["-n", options.net_file, "-r", options.output_file, "-o", options.output_file + ".repaired"]
        args += ["--repair"] if options.repair else []
        args += ["--remove-loops"] if options.remove_loops else []
        subprocess.call([sumolib.checkBinary("duarouter")] + args)


if __name__ == "__main__":
    options = get_options()
    main(options)
