#!/usr/bin/env python
"""
@file    osmGet.py
@author  Daniel.Krajzewicz@dlr.de
@date    2009-08-01
@version $Id$

Retrieves an area from OpenStreetMap.

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""

import os,sys,optparse,subprocess
from os import path

netconvert = "netconvert"
polyconvert = "polyconvert"
if "SUMO" in os.environ:
    netconvert = path.join(os.environ["SUMO"], netconvert)
    polyconvert = path.join(os.environ["SUMO"], polyconvert)

vclassRemove = {"passenger" : " --remove-edges.by-vclass hov,taxi,bus,delivery,transport,lightrail,cityrail,rail_slow,rail_fast,motorcycle,bicycle,pedestrian",
                "road" : " --remove-edges.by-vclass rail_slow,rail_fast,bicycle,pedestrian",
                "all" : "" }
possibleVClassOptions = '|'.join(vclassRemove.keys())


optParser = optparse.OptionParser()
optParser.add_option("-p", "--prefix", default="osm", help="for output file")
optParser.add_option("-f", "--osm-file", help="full name of the osm file to import") # don't know whether area or bbox call was used
optParser.add_option("-m", "--typemap", default=None, help="typemap file for the extraction of colored areas (optional)")
optParser.add_option("-o", "--oldapi-prefix", default=None, help="prefix that was used for retrieval with the old API")
optParser.add_option("-t", "--tiles", type="int", default=1, help="number of tiles used for retrieving OSM-data via the old api")
optParser.add_option("-c", "--vehicle-classes", default='all',help="[(%s)]extract network for a reduced set of vehicle classes" % possibleVClassOptions)  
optParser.add_option("-d", "--output-directory", default='.', help="directory in which to put the output files")
optParser.add_option("-n", "--netconvert-options", default="-R,--guess-ramps,--guess-tls,--tls-guess.joining,--try-join-tls,-v", help="comma-separated options for netconvert")


def build(args=None):
    (options, args) = optParser.parse_args(args=args)

    if ((options.oldapi_prefix and options.osm_file) or
            not (options.oldapi_prefix or options.osm_file)):
        optParser.error("exactly one of the options --osm-file and --oldapi-prefix must be supplied")
    if options.typemap and not path.isfile(options.typemap):
        # fail early because netconvert may take a long time
        optParser.error('typemap file "%s" not found' % options.typemap) 
    if not (options.vehicle_classes in vclassRemove):
        optParser.error('invalid vehicle class "%s" given' % options.vehicle_classes) 
    if not path.isdir(options.output_directory):
        optParser.error('output directory "%s" does not exist' % options.output_directory) 


    netconvertOpts = ' ' + ' '.join(options.netconvert_options.split(',')) + ' --osm-files '
    polyconvertOpts = " -v --typemap %s --osm.keep-full-type --osm-files " % (options.typemap)

    prefix = options.oldapi_prefix
    if prefix: # used old API
        num = options.tiles
        for i in range(num):
            if i != 0:
                netconvertOpts += ","
                polyconvertOpts += ","
            netconvertOpts += "%s%s_%s.osm.xml" % (prefix, i, num)
            polyconvertOpts += "%s%s_%s.osm.xml" % (prefix, i, num)

    else: # used new API
        netconvertOpts += options.osm_file
        polyconvertOpts += options.osm_file
        prefix = path.basename(options.osm_file).replace('.osm.xml','')

    remove = vclassRemove[options.vehicle_classes]
    netfile = path.join(options.output_directory, prefix+'.net.xml')
    polyfile = path.join(options.output_directory, prefix+'.poly.xml')

    call(netconvert + netconvertOpts + remove + " -o %s" % netfile)
    if options.typemap:
        call(polyconvert + polyconvertOpts + " -n %s -o %s" % (netfile, polyfile))

def call(cmd):
    # ensure unix compatibility
    print(cmd)
    if isinstance(cmd, str):
        cmd = filter(lambda a: a!='', cmd.split(' '))
    subprocess.call(cmd)


if __name__ == "__main__":
    build()
