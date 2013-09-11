#!/usr/bin/env python
"""
@file    toolrunner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@version $Id$

Wrapper script for running tool tests with TextTest.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os,subprocess,sys
if len(sys.argv) < 2:
    sys.exit('required argument <tool> missing')
tool = [os.path.join(os.path.dirname(sys.argv[0]), "..", sys.argv[-1])]
if tool[0].endswith(".jar"):
    tool = ["java", "-jar"] + tool

if tool[0].endswith(".py"):
    tool = [os.environ.get('PYTHON') or 'python'] + tool

import os
from sys import version_info
subprocess.call(tool+sys.argv[1:-1], shell=(os.name=="nt"),
                stdout=sys.stdout, stderr=sys.stderr)
