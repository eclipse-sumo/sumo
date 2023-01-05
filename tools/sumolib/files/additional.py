# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    additional.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2013-05-06


def write(name, elements):
    fdo = open(name, "w")
    fdo.write(
        '<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ' +
        'xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/additional_file.xsd">\n')
    for e in elements:
        fdo.write("%s\n" % e.toXML())
    fdo.write("</additional>\n")
    fdo.close()


def write_additional_minidom(xml_dom, children_nodes, file_path, ):
    additionalNode = xml_dom.createElement("additional")
    additionalNode.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")
    additionalNode.setAttribute("xsi:noNamespaceSchemaLocation", "http://sumo.dlr.de/xsd/additional_file.xsd")
    if isinstance(children_nodes, list):
        for node in children_nodes:
            additionalNode.appendChild(node)
    else:
        additionalNode.appendChild(children_nodes)
    xml_dom.appendChild(additionalNode)
    with open(file_path, "w") as f:
        f.write(xml_dom.toprettyxml("   "))
