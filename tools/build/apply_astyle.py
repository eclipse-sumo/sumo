#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    apply_astyle.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2007
# @version $Id$

"""
Applies astyle with the proper settings used in SUMO on all
 files in src (except for foreign) and autopep8 on all .py files.
 Also calls sed to fix missing eol at the end of files.
"""
from __future__ import absolute_import

import os
import subprocess

srcRoot = os.path.join(os.path.dirname(__file__), "../../src/")
for root, dirs, files in os.walk(srcRoot):
    for name in files:
        if name.endswith(".h") or name.endswith(".cpp"):
            subprocess.call(["astyle", "--style=java", "--unpad-paren", "--pad-header", "--pad-oper",
                             "--add-brackets", "--indent-switches", "--align-pointer=type",
                             "-n", os.path.join(root, name)])
            subprocess.call(["sed", "-i", "-e", '$a\\',
                             os.path.join(root, name)])
    for ignoreDir in ['.svn', 'foreign']:
        if ignoreDir in dirs:
            dirs.remove(ignoreDir)
