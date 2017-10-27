#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    removeSVN.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    28-08-2008
# @version $Id$

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
