#!/usr/bin/env python
"""
@file    sikulixrunner.py
@author  Jakob Erdmann
@date    2016-07-29
@version $Id$

Wrapper script for running gui tests with SikuliX and TextTest.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2016 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os
import subprocess
import sys

# we do not need sikulix log output
subprocess.call([os.environ.get('SIKULIX', 'runsikulix')] + sys.argv[1:], env=os.environ,
                stdout=open(os.devnull, 'wb'), stderr=sys.stderr)
