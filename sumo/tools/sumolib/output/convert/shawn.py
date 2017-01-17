"""
@file    shawn.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2013-01-15
@version $Id$

This module includes functions for converting SUMO's fcd-output into
data files read by Shawn.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
from __future__ import absolute_import
import datetime
import sumolib.output
import sumolib.net


def fcd2shawn(inpFCD, outSTRM, further):
    print('<?xml version="1.0" encoding="utf-8"?>', file=outSTRM)
    print('<!-- generated on %s by %s -->\n' %
          (datetime.datetime.now(), further["app"]), file=outSTRM)
    print('<scenario>', file=outSTRM)
    # is it necessary to convert the ids?
    vIDm = sumolib._Running(further["orig-ids"])
    for timestep in inpFCD:
        print('   <snapshot id="%s">' % timestep.time, file=outSTRM)
        if timestep.vehicle:
            for v in timestep.vehicle:
                nid = vIDm.g(v.id)
                print('     <node id="%s"> <location x="%s" y="%s" z="%s"/> </node>' %
                      (nid, v.x, v.y, v.z), file=outSTRM)
        print('   </snapshot>', file=outSTRM)
    print('</scenario>', file=outSTRM)
