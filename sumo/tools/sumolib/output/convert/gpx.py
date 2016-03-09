"""
@file    gpx.py
@author  Jakob Erdmann
@author  Laura Bieker
@date    2014-02-13
@version $Id$

This module includes functions for converting SUMO's fcd-output into
GPX format (http://en.wikipedia.org/wiki/GPS_eXchange_Format)

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from collections import defaultdict


def fcd2gpx(inpFCD, outSTRM, ignored):
    tracks = defaultdict(list)
    for timestep in inpFCD:
        for v in timestep.vehicle:
            tracks[v.id].append((timestep.time, v.x, v.y))

    outSTRM.write('<?xml version="1.0" encoding="UTF-8"?>\n')
    outSTRM.write('<gpx version="1.0">\n')
    for vehicle in sorted(tracks):
        outSTRM.write("  <trk><name>%s</name><trkseg>\n" % vehicle)
        for timestamp, lon, lat in tracks[vehicle]:
            outSTRM.write('    <trkpt lon="%s" lat="%s"><time>%s</time></trkpt>\n' % (
                lon, lat, timestamp))
        outSTRM.write("  </trkseg></trk>\n")
    outSTRM.write('</gpx>\n')
