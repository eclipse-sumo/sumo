#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    matsim_importPlans.py
# @author  Jakob Erdmann
# @author  Camillo Fillinger
# @date    2019-09-27
# @version $Id: matsim_importPlans.py

"""
Import person plans from MATSim
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import optparse

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("-p", "--plan-file", dest="planfile",
                         help="define the route file (mandatory)")
    optParser.add_option("-o", "--out-file", dest="outfile",
                         help="Output file (mandatory)")
    optParser.add_option("--vehicles-only", dest="carsOnly", action="store_true",
                         default=False, help="Import only vehicles instead of persons")
    optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="tell me what you are doing")

    (options, args) = optParser.parse_args(args=args)

    if not options.planfile or not options.outfile:
        optParser.print_help()
        sys.exit()

    return options
   

def main(options):
    with open(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "routes")
        for person in sumolib.output.parse(options.planfile, 'person'):
            vehIndex = 0
            plan = person.plan[0]
            attributes = person.attributes[0]
            # write vehicles
            vehicleslist = []
            untillist = []
            for leg in plan.leg:                
                depart = leg.dep_time if options.carsOnly else "triggered"
                idveh = "%s_%s" % (person.id, vehIndex)
                if leg.route[0].distance == "NaN":
                    outf.write('   <trip id="%s" depart="%s" from="%s" to="%s"/>\n'
                               % (idveh, depart, leg.route[0].start_link,leg.route[0].end_link))
                else:
                    outf.write('   <vehicle id="%s" depart="%s" >\n' % (idveh, depart))
                    outf.write('        <route edges="%s"/>\n' % (leg.route[0].getText()))
                    outf.write('   </vehicle>\n')
                untillist.append (leg.dep_time)                   
                vehicleslist.append (idveh)
                vehIndex=vehIndex+1
            untillist.append (plan.activity[-1].end_time)
            # write person
            if not options.carsOnly:
                vehIndex = 0
                outf.write('   <person id="%s" depart="%s">\n' % (person.id, plan.activity[0].start_time))
                for attr in attributes.attribute:
                    outf.write('       <param key="%s" value="%s"/>\n' % (attr.attr_name, attr.getText()))                      
                for item in plan.getChildList():
                    if item.name == "activity":
                       outf.write('       <stop lane="%s_0" until="%s" actType="%s" />\n' %(item.link, untillist[vehIndex], item.type))
                    elif item.name == "leg":
                        outf.write('       <ride lines="%s" to="%s"  />\n'
                                   %(vehicleslist[vehIndex],item.route[0].end_link))
                        vehIndex=vehIndex+1
                outf.write('   </person>\n')
        outf.write('</routes>\n')
    outf.close()

if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)
