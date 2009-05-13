#!/usr/bin/python
"""
@file    runGuisimTests.py
@author  Michael.Behrisch@dlr.de
@date    2009-02-09
@version $Id$

Copies the sumo tests and tries to run them with guisim.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, shutil, sys, subprocess
sumoRoot = os.path.join(os.path.dirname(sys.argv[0]), "sumo")
guisimRoot = os.path.join(os.path.dirname(sys.argv[0]), "guisim")

if len(sys.argv) > 1 and sys.argv[1] == "-b" and os.path.exists(guisimRoot):
    shutil.rmtree(guisimRoot)
if not os.path.exists(guisimRoot):
    os.mkdir(guisimRoot)
for root, dirs, files in os.walk(sumoRoot):
    for folder in ['.svn', 'meta']:
        if folder in dirs:
            dirs.remove(folder)
    newRoot = root.replace(sumoRoot, guisimRoot)
    for folder in dirs:
        newDir = os.path.join(newRoot, folder)
        if not os.path.exists(newDir):
            os.mkdir(newDir)
    for name in files:
        oldPath = os.path.join(root, name)
        newPath = os.path.join(newRoot, name)
        if name.endswith('sumo'):
            newPath = newPath[:-4] + "guisim"
        if name == "config.sumo":
            src = open(oldPath)
            dest = open(newPath, "w")
	    print >> dest, "use_case_record_mode:GUI"
            for l in src:
                dest.write(l.replace('SUMO_BINARY', 'TEXTTEST_HOME/guisimrunner.py'))
            src.close()
            dest.close()
        elif not os.path.exists(newPath) or os.path.getmtime(oldPath) > os.path.getmtime(newPath):
            shutil.copy2(oldPath, newPath)

if len(sys.argv) == 1:
    os.environ["TEXTTEST_HOME"] = os.path.dirname(sys.argv[0])
    subprocess.call("texttest.py -a guisim -gx", 
                    shell=True, stdout=sys.stdout, stderr=sys.stderr)
