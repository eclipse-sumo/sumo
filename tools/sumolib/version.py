#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    version.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2007

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

import subprocess
from os.path import dirname, exists, join

UNKNOWN_REVISION = "UNKNOWN"
GITDIR = join(dirname(__file__), '..', '..', '.git')


def _findVersion():
    # try to find the version in the config.h
    versionFile = join(dirname(__file__), '..', '..', 'include', 'version.h')
    if not exists(versionFile):
        versionFile = join('src', 'version.h')
    if exists(versionFile):
        version = open(versionFile).read().split()
        if len(version) > 2:
            return version[2][1:-1]
    configFile = join(dirname(__file__), '..', '..', 'src', 'config.h.cmake')
    if exists(configFile):
        config = open(configFile).read()
        if "//#define HAVE_VERSION_H" in config:
            version = config.find("VERSION_STRING") + 16
            if version > 16:
                return "v" + config[version:config.find('"\n', version)] + "-" + (10 * "0")
    return UNKNOWN_REVISION


def gitDescribe(commit="HEAD", gitDir=GITDIR, padZero=True):
    command = ["git", "describe", "--long", "--always", commit]
    if gitDir:
        command[1:1] = ["--git-dir=" + gitDir]
        if not exists(gitDir):
            return _findVersion()
    try:
        d = subprocess.check_output(command, universal_newlines=True).strip()
    except subprocess.CalledProcessError:
        return _findVersion()
    if "-" in d:
        # remove the "g" in describe output
        d = d.replace("-g", "-")
        m1 = d.find("-") + 1
        m2 = d.find("-", m1)
        diff = max(0, 4 - (m2 - m1)) if padZero else 0
        # prefix the number of commits with a "+" and pad with 0
        d = d[:m1].replace("-", "+") + (diff * "0") + d[m1:]
    return d
