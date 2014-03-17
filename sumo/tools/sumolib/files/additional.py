"""
@file    additional.py
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2013-05-06
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

def write(name, elements):
    fdo = open(name, "w")
    fdo.write('<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/additional_file.xsd">\n')
    for e in elements:
        fdo.write(e.toXML())
        fdo.write("\n")
    fdo.write("</additional>\n")
    fdo.close()
    