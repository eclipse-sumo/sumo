#!/usr/bin/env python
"""
@file    apply_astyle.py
@author  Michael.Behrisch@dlr.de
@date    2007
@version $Id: apply_astyle.py 7559 2009-07-28 20:29:48Z behrisch $

Applies astyle with the proper settings used in SUMO on all
 files in src.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, os.path, sys, shutil

proc = {
"input_edges.edg.xml": "<edges xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='http://sumo.sf.net/xsd/edges_file.xsd'",
"input_nodes.nod.xml": "<nodes xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='http://sumo.sf.net/xsd/nodes_file.xsd'",
"input_types.typ.xml": "<types xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='http://sumo.sf.net/xsd/types_file.xsd'",
"input_connections.con.xml": "<connections xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='http://sumo.sf.net/xsd/connections_file.xsd'"
}

srcRoot = os.path.join(os.path.dirname(sys.argv[0]), "../")

for root, dirs, files in os.walk(srcRoot):
    for name in files:
        if name in proc:
            repTo = proc[name]
            repFrom = repTo[:repTo.find(' ')]
            print "Patching '%s'" % (os.path.join(root, name))
            shutil.copy(os.path.join(root, name), "totest.xml")
            fdi = open("totest.xml")
            fdo = open("totest.patch", "w")
            for line in fdi:
                if line.find(repTo)<0 and line.find(repFrom)>=0:
                    line = line.replace(repFrom, repTo)
                fdo.write(line)
            fdo.close()
            fdi.close()
            shutil.copy("totest.patch", os.path.join(root, name))
        for ignoreDir in ['.svn', 'foreign']:
            if ignoreDir in dirs:
                dirs.remove(ignoreDir)
