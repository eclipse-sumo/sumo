"""
@file    gpx.py
@author  Jakob Erdmann
@date    2014-02-13
@version $Id: gpx.py 15337 2013-12-20 11:17:02Z namdre $

This module includes functions for converting SUMO's fcd-output into
GPX format (http://en.wikipedia.org/wiki/GPS_eXchange_Format)

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from collections import defaultdict

def fcd2gpx(inpFCD, outSTRM, ignored):
    tracks = defaultdict(list)
    for timestep in inpFCD:
        for v in timestep.vehicle:
            tracks[v.id].append((timestep.time, v.x, v.y))

    outSTRM.write('<?xml version="1.0" encoding="UTF-8"?>\n')
    outSTRM.write('<gpx version="1.0">\n')
    for vehicle, trackpoints in tracks.iteritems():
        outSTRM.write("  <trk><name>%s</name><trkseg>\n" % vehicle)
        for timestamp, lon, lat in trackpoints:
            outSTRM.write('    <trkpt lon="%s" lat="%s"><time>%s</time></trkpt>\n' % (
                lon, lat, timestamp))
        outSTRM.write("  </trkseg></trk>\n")
    outSTRM.write('</gpx>\n')

