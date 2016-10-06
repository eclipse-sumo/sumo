#!/usr/bin/env python
"""
@file    route_departOffset.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    11.09.2009
@version $Id$

Applies a given offset to the given route's departure time

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2016 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function
import os, sys
import codecs
import optparse

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    from sumolib.output import parse
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def intIfPossible(val):
    if int(val) == val:
        return int(val)
    else:
        return val

def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("-r", "--input-file", dest="infile", help="the input route file (mandatory)")
    optParser.add_option("-o", "--output-file", dest="outfile", help="the output route file (mandatory)")
    optParser.add_option("-d", "--depart-offset", dest="offset", type="float", help="the depart offset to apply (mandatory)")
    optParser.add_option("--modify-ids", dest="modify_ids", action="store_true", default=False, help="whether ids should be modified as well")
    optParser.add_option("--heterogeneous", dest="heterogeneous",
            action="store_true", default=False, help="whether heterogeneous objects shall be parsed (i.e. vehicles with embeded and referenced routes)")
    optParser.add_option("--depart-edges", dest="depart_edges", 
            help="only modify departure times of vehicles departing on the given edges")
    optParser.add_option("--depart-edges.file", dest="depart_edges_file", 
            help="only modify departure times of vehicles departing on edges or lanes in the given selection file")

    (options, args) = optParser.parse_args(args=args)
    if options.infile is None or options.outfile is None or options.offset is None:
        optParser.print_help()
        sys.exit()

    options.offset = intIfPossible(options.offset)

    if options.depart_edges is not None:
        options.depart_edges = options.depart_edges.split(',')

    if options.depart_edges_file is not None:
        if options.depart_edges is None:
            options.depart_edges = []
        for line in open(options.depart_edges_file):
            line = line.strip()
            if line.startswith("edge:"):
                options.depart_edges.append(line[5:])
            elif line.startswith("lane:"):
                options.depart_edges.append(line[5:-2])
            else: 
                options.depart_edges.append(line)

    return options


def main(options):
    # cache stand-alone routes
    routesDepart = {} # first edge for each route

    with codecs.open(options.outfile, 'w', encoding='utf8') as out:
        out.write("<routes>\n")
        for route in parse(options.infile, "route"):
            if route.hasAttribute('id') and route.id is not None:
                routesDepart[route.id] = route.edges.split()[0]
                out.write(route.toXML('    '))

        for obj in parse(options.infile, ['vehicle', 'trip', 'flow', 'vType'],
                heterogeneous=options.heterogeneous, warn=False):
            if obj.name == 'vType':
                # copy
                pass
            else:
                if options.modify_ids:
                    obj.id += "_%s" % options.offset
                departEdge = None
                if options.depart_edges is not None:
                    # determine the departEdge of the current vehicle
                    if obj.name == 'trip':
                        departEdge = obj.attr_from
                    elif obj.name == 'vehicle':
                        if obj.hasAttribute('route') and obj.route is not None:
                            departEdge = routesDepart[obj.route]
                        else:
                            # route child element
                            departEdge = obj.route[0].edges.split()[0]
                    elif obj.name == 'flow':
                        if obj.hasAttribute('attr_from') and obj.attr_from is not None:
                            departEdge = obj.attr_from
                        elif obj.hasAttribute('route') and obj.route is not None:
                            departEdge = routesDepart[obj.route]
                        else:
                            # route child element
                            departEdge = obj.route[0].edges.split()[0]
                if departEdge is None or departEdge in options.depart_edges:
                    if obj.name in ['trip', 'vehicle']:
                        obj.depart = str(intIfPossible(float(obj.depart) + options.offset))
                    else:
                        obj.begin = str(intIfPossible(float(obj.begin) + options.offset))
                        obj.end = str(intIfPossible(float(obj.end) + options.offset))

            out.write(obj.toXML('    '))
        out.write("</routes>\n")


if __name__ == "__main__":
    main(get_options(sys.argv))

