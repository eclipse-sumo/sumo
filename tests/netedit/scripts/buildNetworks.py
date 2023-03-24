# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    buildNetworks.py
# @author  Pablo Alvarez Lopez
# @date    2022-03-21

import os
import scandir


# folder lists
folderList = []

# get all net.netccfg
for paths, dirs, files in scandir.walk('D:/SUMO/tests/netedit'):
    for file in files:
        if file.endswith("net.netccfg"):
            folderList.append(os.path.join(paths))

for folder in folderList:
    os.system('D:/SUMO/bin/netconvert.exe -c ' + folder + '/net.netccfg')
