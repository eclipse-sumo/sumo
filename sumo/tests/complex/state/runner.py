#!/usr/bin/env python
"""
@file    runner.py
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2009-11-04
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
import subprocess
import sys

idx = sys.argv.index(":")
saveParams = sys.argv[1:idx]
loadParams = sys.argv[idx + 1:]
if '--mesosim' in loadParams:
    saveParams.append('--mesosim')

# need to add runner.py again in options.complex.meso to ensure it is the
# last  entry
saveParams = [p for p in saveParams if not 'runner.py' in p]
loadParams = [p for p in loadParams if not 'runner.py' in p]

# print "save:", saveParams
# print "load:", loadParams

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', 'bin', 'sumo'))
# print "sumoBinary", sumoBinary
subprocess.call([sumoBinary] + saveParams,
                shell=(os.name == "nt"), stdout=sys.stdout, stderr=sys.stderr)
subprocess.call([sumoBinary] + loadParams,
                shell=(os.name == "nt"), stdout=sys.stdout, stderr=sys.stderr)
