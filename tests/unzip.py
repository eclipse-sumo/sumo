#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    unzip.py
# @author  Jakob Erdmann
# @date    2022-09-14

"""platform-independent collate_script to compare .xml.gz files"""

import sys
import gzip
with gzip.open(sys.argv[1], 'r') as f:
    if sys.version_info[0] < 3:
        sys.stdout.write(f.read())
    else:
        sys.stdout.buffer.write(f.read())
