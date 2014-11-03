#!/usr/bin/env python
"""
@file    generateTLSE3Detectors.py 
@author  Daniel Krajzewicz
@author  Karol Stosiek
@author  Michael Behrisch
@date    2007-10-25
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import logging
import optparse
import os
import sys
import xml.dom.minidom

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net

def get_net_file_directory(net_file):
    """ Returns the directory containing the net file given. """

    dirname = os.path.split(net_file)[0]
    return dirname

def open_detector_file(destination_dir, detector_file_name):
    """ Opens a new detector file in given directory. """

    return open(os.path.join(destination_dir, detector_file_name), "w")

if __name__ == "__main__":
    # pylint: disable-msg=C0103

    logging.basicConfig(level="INFO")

    option_parser = optparse.OptionParser()
    option_parser.add_option("-n", "--net-file",
                         dest="net_file",
                         help="Network file to work with. Mandatory.",
                         type="string")
    option_parser.add_option("-l", "--detector-length",
                         dest="requested_detector_length",
                         help="Length of the detector in meters "
                              "(-1 for maximal length).",
                         type="int",
                         default=250)
    option_parser.add_option("-d", "--distance-to-TLS",
                         dest="requested_distance_to_tls",
                         help="Distance of the detector to the traffic "
                              "light in meters. Defaults to 0.1m.",
                         type="float",
                         default=.1)
    option_parser.add_option("-f", "--frequency",
                         dest="frequency",
                         help="Detector's frequency. Defaults to 60.",
                         type="int",
                         default=60)
    option_parser.add_option("-o", "--output",
                         dest="output",
                         help="The name of the file to write the detector "
                              "definitions into. Defaults to e1.add.xml.",
                         type="string",
                         default="e3.add.xml")
    option_parser.add_option("-r", "--results-file",
                         dest="results",
                         help="The name of the file the detectors write "
                              "their output into. Defaults to e1output.xml.",
                         type="string",
                         default="e3output.xml")
    option_parser.set_usage("generateTLSE3Detectors.py -n example.net.xml "
                        "-l 250 -d .1 -f 60")

    (options, args) = option_parser.parse_args()
    if not options.net_file:
        print "Missing arguments"
        option_parser.print_help()
        exit()

    logging.info("Reading net...")
    network = sumolib.net.readNet(options.net_file)

    logging.info("Generating detectors...")
    detectors_xml = xml.dom.minidom.Element("additional")
    generated_detectors = 0
    for tls in network._tlss:
        for edge in sorted(tls.getEdges(), key=sumolib.net.edge.Edge.getID):
            detector_xml = xml.dom.minidom.Element("e3Detector")
            detector_xml.setAttribute("id", "e3_" + str(tls._id) + "_" + str(edge._id))
            detector_xml.setAttribute("freq", str(options.frequency))
            detector_xml.setAttribute("file", options.results)

            input_edges = network.getDownstreamEdges(edge, options.requested_detector_length, True)
            for input_edge in input_edges:
                position = input_edge[1]
                if input_edge[3]:
                    position = .1
                for lane in input_edge[0]._lanes:
                    detector_entry_xml = xml.dom.minidom.Element("detEntry")
                    detector_entry_xml.setAttribute("lane", str(lane.getID()))
                    detector_entry_xml.setAttribute("pos", str(position))
                    detector_xml.appendChild(detector_entry_xml)

            for lane in edge._lanes:
                detector_exit_xml = xml.dom.minidom.Element("detExit")
                detector_exit_xml.setAttribute("lane", str(lane.getID()))
                detector_exit_xml.setAttribute("pos", "-.1")
                detector_xml.appendChild(detector_exit_xml)

            detectors_xml.appendChild(detector_xml)
            generated_detectors += 1

    detector_file = open_detector_file(
        get_net_file_directory(options.net_file),
        options.output)
    detector_file.write(detectors_xml.toprettyxml())
    detector_file.close()

    logging.info("%d e3 detectors generated!" % (generated_detectors))

