#!/usr/bin/env python
"""
@file    addSchema.py
@author  Daniel.Krajzewicz@dlr.de
@date    2010
@version $Id$

Inserts XML schema references.

Copyright (C) 2010-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import os, shutil

schema = 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'

proc = {
"input_routes.rou.xml": '<routes %s xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/routes_file.xsd"' % schema,
"input_routes2.rou.xml": '<routes %s xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/routes_file.xsd"' % schema,
"input_edges.edg.xml": '<edges %s xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/edges_file.xsd"' % schema,
"input_nodes.nod.xml": '<nodes %s xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/nodes_file.xsd"' % schema,
"input_types.typ.xml": '<types %s xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/types_file.xsd"' % schema,
"input_connections.con.xml": '<connections %s xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/connections_file.xsd"' % schema
}

srcRoot = os.path.join(os.path.dirname(__file__), "..", "..")

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
                if repFrom in line and schema not in line:
                    line = line.replace(repFrom, repTo)
                fdo.write(line)
            fdo.close()
            fdi.close()
            shutil.copy("totest.patch", os.path.join(root, name))
        for ignoreDir in ['.svn', 'foreign']:
            if ignoreDir in dirs:
                dirs.remove(ignoreDir)
