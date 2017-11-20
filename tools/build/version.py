#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    version.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2007
# @version $Id$

"""
This script rebuilds "../../src/version.h", the file which
 lets the applications know the version of their build.
It does this by parsing the SVN revision either from .svn/entries or .svn/wc.db (depending on svn
version of the working copy).
If the version file is newer than the svn file or the revision cannot be
determined any exisitng vershion.h is kept
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import re
import subprocess
from os.path import dirname, exists, getmtime, join, isdir

UNKNOWN_REVISION = "UNKNOWN"
GITDIR = '.git'
SVNDIR = '.svn'
SVN16FILE = 'entries'
SVN16FILE2 = 'all-wcprops'
SVN17FILE = 'wc.db'
GITFILE = "index"


def findRootDir(searchRoot):
    # we need to find the .svn folder
    # for subversion 1.7 and later, it only exists at the wc root and each
    # externals root
    candidates = [
        join(searchRoot, SVNDIR),              # src
        join(searchRoot, '..', SVNDIR),        # sumo
        join(searchRoot, '..', '..', SVNDIR),  # trunk
        join(searchRoot, '..', GITDIR),        # sumo
        join(searchRoot, '..', '..', GITDIR)]  # trunk
    for d in candidates:
        if isdir(d):
            return d
    return None


def findVCSFile(rootDir):
    candidates = [
        join(rootDir, SVN17FILE),
        join(rootDir, SVN16FILE2),
        join(rootDir, SVN16FILE),
        join(rootDir, GITFILE)]
    for f in candidates:
        if exists(f):
            return f
    return None


def parseRevision(svnFile):
    if GITDIR in svnFile:
        return UNKNOWN_REVISION
    if SVN17FILE in svnFile or SVN16FILE2 in svnFile:
        # new style wc.db
        svnRevision = -1
        for l in open(svnFile, 'rb'):
            m = re.search('[!]svn[/]ver[/](\d*)[/]', l)
            if m:
                try:
                    svnRevision = max(svnRevision, int(m.group(1)))
                except ValueError:
                    pass
        if svnRevision >= 0:
            return "dev-SVN-r%s" % svnRevision
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
                return "dev-SVN-r" + svnRevision
        return UNKNOWN_REVISION


def gitDescribe(commit="HEAD"):
    d = subprocess.check_output(["git", "describe", "--long", "--always", commit]).strip()
    if "-" in d:
        d = d.replace("-g", "-")
        m1 = d.find("-") + 1
        m2 = d.find("-", m1)
        diff = max(0, 4 - (m2 - m1))
        d = d[:m1].replace("-", "+") + (diff * "0") + d[m1:]
    return d


def create_version_file(versionFile, revision, vcsFile):
    print('generating %s from revision in %s' % (versionFile, vcsFile))
    with open(versionFile, 'w') as f:
        print('#define VERSION_STRING "%s"' % revision, file=f)


def main():
    sumoSrc = join(dirname(__file__), '..', '..', 'src')
    # determine output file
    if len(sys.argv) > 1:
        versionDir = sys.argv[1]
        if sys.argv[1] == "-":
            print(gitDescribe())
            return
    else:
        versionDir = sumoSrc
    versionFile = join(versionDir, 'version.h')

    # determine dir
    if len(sys.argv) > 2:
        vcsDir = sys.argv[2]
    else:
        vcsDir = findRootDir(sumoSrc)
    if vcsDir is None or not isdir(vcsDir):
        print("unknown revision - version control dir '%s' not found" % vcsDir)
        if not exists(versionFile):
            create_version_file(versionFile, UNKNOWN_REVISION, "<None>")
    else:
        # determine file
        vcsFile = findVCSFile(vcsDir)
        if vcsFile is None:
            print("unknown revision - no version control file found in %s" % vcsDir)
            if not exists(versionFile):
                create_version_file(versionFile, UNKNOWN_REVISION, "<None>")
        elif not exists(versionFile) or getmtime(versionFile) < getmtime(vcsFile):
            # vcsFile is newer. lets update the revision number
            try:
                if GITDIR in vcsDir:
                    revision = gitDescribe()
                else:
                    svnInfo = subprocess.check_output(['svn', 'info', sumoSrc])
                    revision = "dev-SVN-r" + re.search('Revision: (\d*)', svnInfo).group(1)
            except:
                revision = parseRevision(vcsFile)
            create_version_file(versionFile, revision, vcsFile)


if __name__ == "__main__":
    main()
