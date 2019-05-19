#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

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
import subprocess
from os.path import dirname, exists, getmtime, join

UNKNOWN_REVISION = "UNKNOWN"
GITDIR = '.git'
GITFILE = "index"


def gitDescribe(commit="HEAD", gitDir=None, commitPrefix="+", padZero=True):
    command = ["git", "describe", "--long", "--always", commit]
    if gitDir:
        command[1:1] = ["--git-dir=" + gitDir]
    try:
        d = subprocess.check_output(command, universal_newlines=True).strip()
    except subprocess.CalledProcessError:
        # try to find the version in the config.h
        configFile = join(dirname(__file__), '..', '..', 'src', 'config.h.cmake')
        if exists(configFile):
            config = open(configFile).read()
            if "//#define HAVE_VERSION_H" in config:
                version = config.find("VERSION_STRING") + 16
                if version > 16:
                    return "v" + config[version:config.find('"\n', version)] + "-" + (10 * "0")
        return UNKNOWN_REVISION
    if "-" in d:
        # remove the "g" in describe output
        d = d.replace("-g", "-")
        m1 = d.find("-") + 1
        m2 = d.find("-", m1)
        diff = max(0, 4 - (m2 - m1)) if padZero else 0
        # prefix the number of commits with a "+" and pad with 0
        d = d[:m1].replace("-", commitPrefix) + (diff * "0") + d[m1:]
    return d


def create_version_file(versionFile, revision, vcsFile):
    print('generating %s from revision in %s' % (versionFile, vcsFile))
    with open(versionFile, 'w') as f:
        print('#define VERSION_STRING "%s"' % revision, file=f)


def main():
    sumoRoot = join(dirname(__file__), '..', '..')
    vcsDir = join(sumoRoot, GITDIR)
    # determine output file
    if len(sys.argv) > 1:
        versionDir = sys.argv[1]
        if sys.argv[1] == "-":
            sys.stdout.write(gitDescribe(gitDir=vcsDir))
            return
    else:
        versionDir = join(sumoRoot, "src")
    versionFile = join(versionDir, 'version.h')

    vcsFile = join(vcsDir, GITFILE)
    if exists(vcsFile):
        if not exists(versionFile) or getmtime(versionFile) < getmtime(vcsFile):
            # vcsFile is newer. lets update the revision number
            try:
                create_version_file(versionFile, gitDescribe(gitDir=vcsDir), vcsFile)
            except Exception as e:
                print("Error creating", versionFile, e)
    else:
        print("unknown revision - version control file '%s' not found" % vcsFile)
    if not exists(versionFile):
        create_version_file(versionFile, UNKNOWN_REVISION, "<None>")


if __name__ == "__main__":
    main()
