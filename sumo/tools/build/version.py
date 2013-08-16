#!/usr/bin/env python
"""
@file    version.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@date    2007
@version $Id$

This script rebuilds "../../src/version.h", the file which
 lets the applications know the version of their build.
It does this by parsing the SVN revision either from .svn/entries or .svn/wc.db (depending on svn
version of the working copy).
If the version file is newer than the svn file or the revision cannot be
determined any exisitng vershion.h is kept

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import sys
import re
from subprocess import Popen, PIPE
from os.path import dirname, exists, getmtime, join, isdir

UNKNOWN_REVISION = "UNKNOWN"
SVNDIR = '.svn'
SVN16FILE = 'entries'
SVN16FILE2 = 'all-wcprops'
SVN17FILE = 'wc.db'

def find_svnDir(searchRoot):
    # we need to find the .svn folder
    # for subversion 1.7 and later, it only exists at the wc root and each externals root
    candidates = [
            join(searchRoot, SVNDIR),              # src 
            join(searchRoot, '..', SVNDIR),        # sumo 
            join(searchRoot, '..', '..', SVNDIR)]  # trunk
    for d in candidates:
        if isdir(d):
            return d
    return None


def find_svnFile(svnDir):
    candidates = [
            join(svnDir, SVN17FILE), 
            join(svnDir, SVN16FILE2), 
            join(svnDir, SVN16FILE)]
    for f in candidates:
        if exists(f):
            return f
    return None


def parseRevision(svnFile):
    if SVN17FILE in svnFile or SVN16FILE2 in svnFile:
        # new style wc.db
        svnRevision = -1
        for l in open(svnFile, 'rb'):
            m = re.search('[!]svn[/]ver[/](\d*)', l)
            if m:
                try:
                    svnRevision = max(svnRevision, int(m.group(1)))
                except ValueError:
                    pass
        if svnRevision >= 0:
            return svnRevision
        else:
            return UNKNOWN_REVISION
    else:
        # old style entries file
        for i, l in enumerate(open(svnFile)):
            if i == 3 and l.strip().isdigit():
                svnRevision = l.strip()
            revIndex = l.find('revision="')
            if revIndex >= 0:
                revIndex += 10
                svnRevision = l[revIndex:l.index('"', revIndex)]
                return svnRevision
        return UNKNOWN_REVISION


def create_version_file(versionFile, svnRevision, svnFile):
    print('generating %s from revision in %s' % (versionFile, svnFile))
    with open(versionFile, 'w') as f:
        print >>f, '#define VERSION_STRING "dev-SVN-r%s"' % svnRevision


def main():
    sumoSrc = join(dirname(__file__), '..', '..', 'src')
    # determine output file
    if len(sys.argv) > 1:
        versionDir = sys.argv[1]
    else:
        versionDir = sumoSrc
    versionFile = join(versionDir, 'version.h')

    # determine svn dir
    if len(sys.argv) > 2:
        svnDir = sys.argv[2]
    else:
        svnDir = find_svnDir(sumoSrc)
    if svnDir == None or not exists(svnDir):
        print "unknown revision - svn dir '%s' not found" % svnDir
        if not exists(versionFile):
            create_version_file(versionFile, UNKNOWN_REVISION, "<None>")
    else:
        # determine svn file
        svnFile = find_svnFile(svnDir)
        if svnFile == None:
            print "unknown revision - no svn file found in %s" % svnDir
            if not exists(versionFile):
                create_version_file(versionFile, UNKNOWN_REVISION, "<None>")
        if not exists(versionFile) or getmtime(versionFile) < getmtime(svnFile):
            # svnFile is newer. lets update the revision number
            try:
                svnRevision = int(re.search(
                        'Revision: (\d*)\n',
                        Popen(['svn', 'info', sumoSrc], stdout=PIPE).communicate()[0]).group(1))
            except:
                svnRevision = parseRevision(svnFile)
            create_version_file(versionFile, svnRevision, svnFile)


if __name__ == "__main__":
    main()
