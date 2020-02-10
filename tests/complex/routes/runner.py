#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2015-02-03


from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
from optparse import OptionParser, BadOptionError, AmbiguousOptionError
sys.path.append(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', "tools"))
from sumolib import checkBinary  # noqa


class PassThroughOptionParser(OptionParser):

    """
    An unknown option pass-through implementation of OptionParser.
    see http://stackoverflow.com/questions/1885161/how-can-i-get-optparses-optionparser-to-ignore-invalid-options

    When unknown arguments are encountered, bundle with largs and try again,
    until rargs is depleted.

    sys.exit(status) will still be called if a known argument is passed
    incorrectly (e.g. missing arguments or bad argument types, etc.)
    """

    def _process_args(self, largs, rargs, values):
        while rargs:
            try:
                OptionParser._process_args(self, largs, rargs, values)
            except (BadOptionError, AmbiguousOptionError) as e:
                largs.append(e.opt_str)


def runInstance(elem, attrSet, childSet, depart):
    print(elem, attrSet, childSet)
    sys.stdout.flush()
    with open("routes.xml", "w") as routes:
        routes.write('''<routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" \
xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/routes_file.xsd">
    <route id="a" edges="1fi 1si 2o 2fi 2si"/>
    <%s id="v" %s''' % (elem, depart))
        for idx, attr in enumerate(attrs):
            if attrSet & (2 ** idx):
                routes.write(attr)
        routes.write('>\n')
        for idx, child in enumerate(childs):
            if childSet & (2 ** idx):
                routes.write((8 * " ") + child)
        routes.write('    </%s>\n</routes>\n' % elem)
    retCode = subprocess.call(
        call + ["-n", "input_net.net.xml", "-r", routes.name], stdout=sys.stdout, stderr=sys.stdout)
    retCodeTaz = subprocess.call(
        call + ["-n", "input_net.net.xml", "-r", routes.name, "--with-taz"], stdout=sys.stdout, stderr=sys.stdout)
    if retCode < 0 or retCodeTaz < 0:
        sys.stdout.write(open(routes.name).read())
        sys.exit()


optParser = PassThroughOptionParser()
optParser.add_option("-e", "--element", help="xml element to choose")
optParser.add_option(
    "-a", "--attr", type="int", default=0, help="attribute set to use")
optParser.add_option(
    "-c", "--child", type="int", default=0, help="child set to use")
options, args = optParser.parse_args()

if len(args) == 0 or args[0] == "sumo":
    call = [checkBinary('sumo'), "--no-step-log", "--no-duration-log",
            "-a", "input_additional.add.xml"]
elif args[0] == "dfrouter":
    call = [checkBinary('dfrouter'),
            "--detector-files", "input_additional.add.xml"]
elif args[0] == "duarouter":
    call = [checkBinary('duarouter'), "--no-step-log",
            "-o", "dummy.xml", "-a", "input_additional.add.xml"]
elif args[0] == "jtrrouter":
    call = [checkBinary('jtrrouter'), "--no-step-log",
            "-o", "dummy.xml", "-a", "input_additional.add.xml"]
else:
    print("Unsupported application defined", file=sys.stderr)
call += args[1:]

elements = {'vehicle': 'depart="0"',
            'flow': 'begin="0" end="1" number="1"', 'trip': 'depart="0"'}
attrs = [' from="1fi"', ' to="2si"',
         ' fromTaz="1"', ' toTaz="2"', ' route="a"']
childs = ['<route edges="1fi 1si 2o 2fi 2si"/>\n', '<stop lane="1fi_0" duration="10"/>\n',
          '<stop lane="1si_0" duration="10"/>\n', '<stop lane="2si_0" duration="10"/>\n']

# check route processing
if options.element:
    runInstance(
        options.element, options.attr, options.child, elements[options.element])
else:
    for elem, depart in sorted(elements.iteritems()):
        for attrSet in range(2 ** len(attrs)):
            for childSet in range(2 ** len(childs)):
                runInstance(elem, attrSet, childSet, depart)
