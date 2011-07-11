#!/usr/bin/env python
"""
@file    schemaCheck.py
@author  Daniel.Krajzewicz@dlr.de
@date    03.12.2009
@version $Id$

Checks schema for files matching certain file names using either
lxml or SAX2Count.exe depending on availability.

Copyright (C) 2009-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import os, sys, subprocess, glob, traceback
try:
    from lxml import etree
    haveLxml = True
    schemes = {}
except ImportError:
    haveLxml = False

def validate(f):
    try:
        doc = etree.parse(f)
        schemaLoc = doc.getroot().get('{http://www.w3.org/2001/XMLSchema-instance}noNamespaceSchemaLocation')
        if schemaLoc:
            if schemaLoc not in schemes:
                schemes[schemaLoc] = etree.XMLSchema(etree.parse(schemaLoc))
            schemes[schemaLoc].validate(doc)
            sys.stderr.write(str(schemes[schemaLoc].error_log))
    except:
        print >> sys.stderr, "Error on parsing '%s'!" %f
        traceback.print_exc()

def main(srcRoot, err):
    toCheck = [ "*.edg.xml", "*.nod.xml", "*.con.xml", "*.typ.xml", "*.net.xml", "*.cfg", "net.netgen", "net.netconvert" ]
    sax2count = "SAX2Count.exe"
    if 'XERCES_64' in os.environ:
        sax2count = os.path.join(os.environ['XERCES_64'], "bin", sax2count)
    elif 'XERCES' in os.environ:
        sax2count = os.path.join(os.environ['XERCES'], "bin", sax2count)

    if os.path.exists(srcRoot):
        if os.path.isdir(srcRoot):
            for root, dirs, files in os.walk(srcRoot):
                for pattern in toCheck:
                    for name in glob.glob(os.path.join(root, pattern)):
                        if haveLxml:
                            validate(name)
                        elif os.name != "posix":
                            subprocess.call(sax2count + " -v=always -f " + name, stdout=open(os.devnull), stderr=err)
                    if '.svn' in dirs:
                        dirs.remove('.svn')
        else:
            if haveLxml:
                validate(srcRoot)
            elif os.name != "posix":
                subprocess.call(sax2count + " -v=always -f " + srcRoot, stdout=open(os.devnull), stderr=err)
    fail = 0
    if haveLxml:
        for scheme in schemes.itervalues():
            if scheme.error_log:
                fail = 1
                print >> err, scheme.error_log
    return fail

if __name__ == "__main__":
    if os.name == "posix" and not haveLxml:
        print >> sys.stderr, "neither SAX2Count nor lxml available, exiting"
        sys.exit(1)
    srcRoot = "."
    if len(sys.argv) > 1:
        srcRoot = sys.argv[1]
    sys.exit(main(srcRoot, sys.stderr))
