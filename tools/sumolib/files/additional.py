# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    additional.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2013-05-06
# @version $Id$


def write(name, elements):
    fdo = open(name, "w")
    fdo.write(
        '<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ' +
        'xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/additional_file.xsd">\n')
    for e in elements:
        fdo.write("%s\n" % e.toXML())
    fdo.write("</additional>\n")
    fdo.close()
