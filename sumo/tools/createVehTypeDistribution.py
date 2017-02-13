#!/usr/bin/env python
"""
@file    createVehTypeDistribution.py
@author  Mirko Barthauer (Technische Universitaet Braunschweig, Institut fuer Verkehr und Stadtbauwesen)
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2016-06-09
@version $Id$


Creates a vehicle type distribution with a number of representative car-following parameter sets. Optional parameters can be viewed by using the --help switch.
Mandatory input: 
path to config file - defines the car-following model parameter distributions for one single vehicle type distribution

In the config file, one line is used per vehicle type attribute. The syntax is:
nameOfAttribute; valueOfAttribute [; limits]

ValueOfAttribute can be a string, a scalar value or a distribution definition. Available distributions and its syntax are:
"normal(mu,sd)" with mu and sd being floating numbers: Normal distribution with mean mu and standard deviation sd.
"uniform(a,b)" with limits a and b being floating numbers: Uniform distribution between a and b.
"gamma(alpha,beta)" with parameters alpha and beta: Gamma distribution. 

Limits are optional and defined as the allowed interval: e.g. "[0,1]" or "[3.5,5.0]". By default, no negative values are accepted but have to be enabled by 
a negative lower limit. 


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os
import sys
import csv
import re
import xml.dom.minidom
import random
import argparse


class FixDistribution(object):

    def __init__(self, params, isNumeric=True):
        if isNumeric:
            self._params = tuple([float(p) for p in params])
        else:
            self._params = params
        self._limits = (0, None)
        self._isNumeric = isNumeric

    def setLimits(self, limits):
        self._limits = limits

    def sampleValue(self):
        value = self._sampleValue()
        if self._isNumeric:
            if self._limits[0] is not None and value < self._limits[0]:
                value = self._limits[0]
            elif self._limits[1] is not None and value > self._limits[1]:
                value = self._limits[1]

        return value

    def sampleValueString(self, decimalPlaces):
        if self._isNumeric:
            decimalPattern = "%." + str(decimalPlaces) + "f"
            return decimalPattern % self.sampleValue()
        return self.sampleValue()

    def _sampleValue(self):
        return self._params[0]


class NormalDistribution(FixDistribution):

    def __init__(self, loc, scale):
        FixDistribution.__init__(self, (loc, scale))

    def _sampleValue(self):
        return random.normalvariate(self._params[0], self._params[1])


class UniformDistribution(FixDistribution):

    def __init__(self, lower, upper):
        FixDistribution.__init__(self, (lower, upper))

    def _sampleValue(self):
        return random.uniform(self._params[0], self._params[1])


class GammaDistribution(FixDistribution):

    def __init__(self, loc, scale):
        FixDistribution.__init__(self, (loc, 1.0 / scale))

    def _sampleValue(self):
        return random.gammavariate(self._params[0], self._params[1])


def get_options(args=None):
    argParser = argparse.ArgumentParser()
    argParser.add_argument(
        "configFile", help="file path of the config file which defines the car-following parameter distributions")
    argParser.add_argument(
        "-o", "--output-file", dest="outputFile", default="vTypeDistributions.add.xml", help="file path of the output file (if the file already exists, the script tries to insert the distribution node into it)")
    argParser.add_argument(
        "-n", "--name", dest="vehDistName", default="vehDist", help="alphanumerical ID used for the created vehicle type distribution")
    argParser.add_argument(
        "-s", "--size", type=int, default=100, dest="vehicleCount", help="number of vTypes in the distribution")
    argParser.add_argument(
        "-d", "--decimal-places", type=int, default=3, dest="decimalPlaces", help="number of decimal places for numeric attribute values")
    argParser.add_argument("--seed", type=int, help="random seed", default=42)

    options = argParser.parse_args()
    return options


def readConfigFile(filePath):
    result = {}

    distSyntaxes = {'normal': 'normal\(\s*(-?[0-9]+(\.[0-9]+)?)\s*,\s*([0-9]+(\.[0-9]+)?)\s*\)',
                    'uniform': 'uniform\(\s*(-?[0-9]+(\.[0-9]+)?)\s*,\s*(-?[0-9]+(\.[0-9]+)?)\s*\)',
                    'gamma': 'gamma\(\s*([0-9]+(\.[0-9]+)?)\s*,\s*([0-9]+(\.[0-9]+)?)\s*\)'}

    with open(filePath, 'rb') as f:
        reader = csv.reader(f, delimiter=';')
        for row in reader:
            parName = None
            lowerLimit = 0
            upperLimit = None
            value = None

            if len(row) >= 2:
                if len(row[0].strip()) > 0:
                    parName = row[0].strip()
                    # check if attribute value matches given distribution
                    # syntax
                    attValue = row[1].strip()
                    distFound = False
                    for distName, distSyntax in distSyntaxes.items():
                        items = re.findall(distSyntax, attValue)
                        distFound = len(items) > 0
                        if distFound:  # found distribution
                            distPar1 = float(items[0][0])
                            distPar2 = float(items[0][2])

                            if distName == 'normal':
                                value = NormalDistribution(distPar1, distPar2)
                            elif distName == 'uniform':
                                value = UniformDistribution(distPar1, distPar2)
                            elif distName == 'gamma':
                                value = GammaDistribution(distPar1, distPar2)
                            break

                    if not distFound:
                        isNumeric = len(re.findall(
                            '(-?[0-9]+(\.[0-9]+)?)', attValue)) > 0
                        value = FixDistribution((attValue,), isNumeric)

                    # get optional limits
                    if len(row) == 3:
                        limitValue = row[2].strip()
                        items = re.findall(
                            '\[\s*(-?[0-9]+(\.[0-9]+)?)\s*,\s*(-?[0-9]+(\.[0-9]+)?)\s*\]', limitValue)
                        if len(items) > 0:
                            lowerLimit = float(items[0][0])
                            upperLimit = float(items[0][2])
                    value.setLimits((lowerLimit, upperLimit))
                    result[parName] = value
    return result


def main(options):
    if options.seed:
        random.seed(options.seed)
    vTypeParameters = readConfigFile(options.configFile)
    useExistingFile = False
    if os.path.exists(options.outputFile):
        try:
            domTree = xml.dom.minidom.parse(options.outputFile)
        except Exception as e:
            sys.exit("Cannot parse existing %s. Error: %s" %
                     (options.outputFile, str(e)))
        useExistingFile = True
    else:
        domTree = xml.dom.minidom.Document()
    vTypeDistNode = domTree.createElement("vTypeDistribution")
    vTypeDistNode.setAttribute("id", options.vehDistName)

    for i in range(0, options.vehicleCount):
        vTypeNode = domTree.createElement("vType")
        vTypeNode.setAttribute("id", options.vehDistName + str(i))
        for attName, attValue in vTypeParameters.items():
            vTypeNode.setAttribute(
                attName, attValue.sampleValueString(options.decimalPlaces))
        vTypeDistNode.appendChild(vTypeNode)

    existingDistNodes = domTree.getElementsByTagName("vTypeDistribution")
    replaceNode = None
    for existingDistNode in existingDistNodes:
        if existingDistNode.hasAttribute("id") and existingDistNode.getAttribute("id") == options.vehDistName:
            replaceNode = existingDistNode
            break
    if useExistingFile:
        if replaceNode is not None:
            replaceNode.parentNode.replaceChild(vTypeDistNode, replaceNode)
        else:
            domTree.documentElement.appendChild(vTypeDistNode)
    else:
        additionalNode = domTree.createElement("additional")
        additionalNode.setAttribute(
            "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")
        additionalNode.setAttribute(
            "xsi:noNamespaceSchemaLocation", "http://sumo.dlr.de/xsd/additional_file.xsd")
        additionalNode.appendChild(vTypeDistNode)
        domTree.appendChild(additionalNode)
    try:
        fileHandle = open(options.outputFile, "wb")
        domTree.documentElement.writexml(
            fileHandle, addindent="    ", newl="\n")
        fileHandle.close()
    except Exception as e:
        sys.exit(str(e))
    sys.stdout.write("Output written to %s" % options.outputFile)


if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)
