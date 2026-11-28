#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
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

"""platform-independent collate_script to compare .xml.gz and .zip/.fmu files"""

import sys
import gzip
import zipfile

if zipfile.is_zipfile(sys.argv[1]):
    with zipfile.ZipFile(sys.argv[1], 'r') as zf:
        for info in zf.infolist():
            header = '=== %s (%o) ===\n' % (info.filename, info.external_attr >> 16)
            data = zf.read(info.filename)
            if sys.version_info[0] < 3:
                sys.stdout.write(header)
                sys.stdout.write(data)
                if not data.endswith('\n'):
                    sys.stdout.write('\n')
            else:
                sys.stdout.buffer.write(header.encode('utf-8'))
                sys.stdout.buffer.write(data)
                if not data.endswith(b'\n'):
                    sys.stdout.buffer.write(b'\n')
else:
    with gzip.open(sys.argv[1], 'r') as f:
        if sys.version_info[0] < 3:
            sys.stdout.write(f.read())
        else:
            sys.stdout.buffer.write(f.read())
