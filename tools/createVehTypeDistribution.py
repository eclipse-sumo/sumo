#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    createVehTypeDistribution.py
# @author  Mirko Barthauer (Technische Universitaet Braunschweig, Institut fuer Verkehr und Stadtbauwesen)
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2016-06-09

"""
Creates a vehicle type distribution with a number of representative car-following parameter sets.
Optional parameters can be viewed by using the --help switch.
Mandatory input:
path to config file - defines the car-following model parameter distributions for one single vehicle type distribution

In the config file, one line is used per vehicle type attribute. The syntax is:
nameOfAttribute; valueOfAttribute [; limits]

ValueOfAttribute can be a string, a scalar value or a distribution definition.
All parameters are floating point numbers.
Available distributions and their syntax are:
"normal(mu,sd)": Normal distribution with mean mu and standard deviation sd.
"lognormal(mu,sd)": Log-Normal distribution with mean mu and standard deviation sd.
"uniform(a,b)": Uniform distribution between a and b.
"gamma(alpha,beta)": Gamma distribution.

Limits are optional and defined as the allowed interval: e.g. "[0,1]" or "[3.5,5.0]".
By default, no negative values are accepted but have to be enabled by
a negative lower limit.
"""

import sys
import csv
import re
import argparse
from sumolib.vehicletype import CreateVehTypeDistribution


def get_options(args=None):
    argParser = argparse.ArgumentParser()
    argParser.add_argument(
        "configFile", help="file path of the config file which defines the car-following parameter distributions")
    argParser.add_argument(
        "-o", "--output-file", dest="outputFile", default="vTypeDistributions.add.xml", help="file path of the " +
        "output file (if the file already exists, the script tries to insert the distribution node into it)")
    argParser.add_argument(
        "-n", "--name", dest="vehDistName", default="vehDist", help="alphanumerical ID used for the created " +
        "vehicle type distribution")
    argParser.add_argument(
        "-s", "--size", type=int, default=100, dest="vehicleCount", help="number of vTypes in the distribution")
    argParser.add_argument(
        "-d", "--decimal-places", type=int, default=3, dest="decimalPlaces", help="number of decimal places for " +
        "numeric attribute values")
    argParser.add_argument(
        "--resampling", type=int, default=100, dest="nrSamplingAttempts", help="number of attempts to resample a " +
        "value until it lies in the specified bounds")
    argParser.add_argument("--seed", type=int, help="random seed", default=42)

    options = argParser.parse_args()
    return options


def readConfigFile(options):
    filePath = options.configFile
    result = []
    floatRegex = [r'\s*(-?[0-9]+(\.[0-9]+)?)\s*']
    distSyntaxes = {'normal': r'normal\(%s\)' % (",".join(2 * floatRegex)),
                    'lognormal': r'lognormal\(%s\)' % (",".join(2 * floatRegex)),
                    'normalCapped': r'normalCapped\(%s\)' % (",".join(4 * floatRegex)),
                    'uniform': r'uniform\(%s\)' % (",".join(2 * floatRegex)),
                    'gamma': r'gamma\(%s\)' % (",".join(2 * floatRegex))}

    with open(filePath) as f:
        reader = csv.reader(f, delimiter=';')
        for row in reader:
            attName = None
            lowerLimit = 0
            upperLimit = None
            if len(row) >= 2:
                if len(row[0].strip()) > 0:
                    attName = row[0].strip()
                    if attName == "param":
                        # this indicates that a parameter child-element is to be created for the vTypes
                        isParam = True
                        del row[0]
                        if len(row) < 2:
                            # a parameter needs a name and a value specification
                            continue
                        attName = row[0].strip()
                    else:
                        isParam = False
                    # check if attribute value matches given distribution
                    # syntax
                    attValue = row[1].strip()
                    distFound = False
                    distAttr = None
                    for distName, distSyntax in distSyntaxes.items():
                        items = re.findall(distSyntax, attValue)
                        distFound = len(items) > 0
                        if distFound:  # found distribution
                            distPar1 = float(items[0][0])
                            distPar2 = float(items[0][2])
                            if distName == 'normal':
                                distAttr = {"mu": distPar1, "sd": distPar2}
                            if distName == 'lognormal':
                                distAttr = {"mu": distPar1, "sd": distPar2}
                            elif distName == 'normalCapped':
                                cutLow = float(items[0][4])
                                cutHigh = float(items[0][6])
                                distAttr = {
                                    "mu": distPar1, "sd": distPar2, 'min': cutLow, 'max': cutHigh}
                            elif distName == 'uniform':
                                distAttr = {"a": distPar1, "b": distPar2}
                            elif distName == 'gamma':
                                distAttr = {
                                    "alpha": distPar1, "beta": distPar2}
                            # can only have attValue if no distribution
                            attValue = None
                            break

                    if not distFound:
                        distName = None
                        distAttr = None

                    # get optional limits
                    limits = None
                    if len(row) == 3:
                        limitValue = row[2].strip()
                        items = re.findall(
                            r'\[\s*(-?[0-9]+(\.[0-9]+)?)\s*,\s*(-?[0-9]+(\.[0-9]+)?)\s*\]', limitValue)
                        if len(items) > 0:
                            lowerLimit = float(items[0][0])
                            upperLimit = float(items[0][2])
                            limits = (lowerLimit, upperLimit)

                    result.append({
                        "name": attName,
                        "is_param": isParam,
                        "distribution": distName,
                        "distribution_params": distAttr,
                        "bounds": limits,
                        "attribute_value": attValue
                    })
    return result


def main(options):

    dist_creator = CreateVehTypeDistribution(seed=options.seed,
                                             size=options.vehicleCount,
                                             name=options.vehDistName,
                                             resampling=options.nrSamplingAttempts,
                                             decimal_places=options.decimalPlaces)

    for param_dict in readConfigFile(options):
        dist_creator.add_attribute(param_dict)

    dist_creator.to_xml(options.outputFile)


if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)
