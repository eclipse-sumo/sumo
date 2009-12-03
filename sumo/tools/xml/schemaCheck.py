#!/usr/bin/env python
"""
@file    schemaCheck.py
@author  Daniel.Krajzewicz@dlr.de
@date    03.12.2009
@version $Id: schemaCheck.py 7559 2009-07-28 20:29:48Z behrisch $

Checks schema for files matching certain file names using SAX2Count.exe.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, os.path, sys, shutil

toCheck = [ "input_edges.edg.xml" ]
srcRoot = os.path.join(os.path.dirname(sys.argv[0]), sys.argv[1])
#os.environ['XML_CATALOG_FILES']=os.path.join(os.path.join(os.path.dirname(sys.argv[0]), "./catalog.xml"))
#print os.environ['XML_CATALOG_FILES']

for root, dirs, files in os.walk(srcRoot):
    for name in files:
        if name in toCheck:
            os.system("SAX2Count.exe -v=always -f " + os.path.join(root, name) + " > dev0")
        for ignoreDir in ['.svn', 'foreign']:
            if ignoreDir in dirs:
                dirs.remove(ignoreDir)
