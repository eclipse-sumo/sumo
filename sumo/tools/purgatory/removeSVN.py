#!/usr/bin/python
"""
@file    removeSVN.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    28-08-2008
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import

import os
import sys
import stat
import shutil


path = "./"
if len(sys.argv) > 1:
    path = sys.argv[1]

# remove files in ".svn"
for root, dirs, files in os.walk(path):
    if root.find(".svn") >= 0:
        for file in files:
            os.chmod(os.path.join(root, file), stat.S_IWRITE | stat.S_IREAD)
            os.remove(os.path.join(root, file))
        for dir in dirs:
            os.chmod(os.path.join(root, dir), stat.S_IWRITE | stat.S_IREAD)

# remove dirs in ".svn"
for root, dirs, files in os.walk(path):
    if ".svn" in dirs:
        dirs.remove(".svn")
        os.chmod(os.path.join(root, ".svn"), stat.S_IWRITE | stat.S_IREAD)
        shutil.rmtree(os.path.join(root, ".svn"))
