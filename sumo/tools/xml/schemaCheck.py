#!/usr/bin/env python
"""
@file    schemaCheck.py
@author  Daniel.Krajzewicz@dlr.de
@date    03.12.2009
@version $Id$

Checks schema for files matching certain file names using SAX2Count.exe.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, sys

toCheck = [ "input_edges.edg.xml" ]
srcRoot = "."
if len(sys.argv) > 1:
    srcRoot = sys.argv[1]
sax2count = "SAX2Count.exe"
if 'XERCES_64' in os.environ:
    sax2count = os.path.join(os.environ['XERCES_64'], "bin", sax2count)
elif 'XERCES' in os.environ:
    sax2count = os.path.join(os.environ['XERCES'], "bin", sax2count)
#os.environ['XML_CATALOG_FILES']=os.path.join(os.path.join(os.path.dirname(sys.argv[0]), "./catalog.xml"))
#print os.environ['XML_CATALOG_FILES']

for root, dirs, files in os.walk(srcRoot):
    for name in files:
        if name in toCheck:
            os.system(sax2count + " -v=always -f " + os.path.join(root, name))
        if '.svn' in dirs:
            dirs.remove('.svn')
