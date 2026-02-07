# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
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
This module contains functions to determine the current SUMO version.
"""
from __future__ import absolute_import
from __future__ import print_function

import subprocess
from os.path import dirname, exists, join

try:
    # this tries to determine the version number of an installed wheel
    import importlib.metadata  # noqa
    _version = importlib.metadata.version("sumolib")
except ImportError:
    # this is the fallback version, it gets replaced with the current version on "make install" or "make dist"
    _version = "0.0.0"

GITDIR = join(dirname(__file__), '..', '..', '.git')


def fromVersionHeader():
    """
    Returns the version as defined in "include/version.h" or as a fallback
    "src/config.h.cmake". Since the latter only contains the last release info it is extended
    with "-0000000000" to mark that it may not be a release and the commit hash is unknown.
    """
    versionFile = join(dirname(__file__), '..', '..', 'include', 'version.h')
    if exists(versionFile):
        with open(versionFile) as f:
            version = f.read().split()
        if len(version) > 2:
            return version[2][1:-1]
    # try to find the version in the config.h
    configFile = join(dirname(__file__), '..', '..', 'src', 'config.h.cmake')
    if exists(configFile):
        with open(configFile) as f:
            config = f.read()
        version = config.find("VERSION_STRING") + 16
        if version > 16:
            return "v" + config[version:config.find('"\n', version)] + "-" + (10 * "0")
    return "v" + _version


def gitDescribe(commit="HEAD", gitDir=GITDIR, padZero=True):
    """
    The original git describe format is "<tag>-<commit_count>-g<hash>".
    This function converts it to "<tag>+<commit_count>-<hash>".
    If padZero is true (the default), the commit count is padded to four digits.
    If the git describe call fails or cannot find tags (because it is a shallow clone),
    the result of fromVersionHeader is returned.
    """
    command = ["git", "describe", "--long", "--always", commit]
    if gitDir:
        command[1:1] = ["--git-dir=" + gitDir]
        if not exists(gitDir):
            return fromVersionHeader()
    try:
        d = subprocess.check_output(command, universal_newlines=True).strip()
    except (subprocess.CalledProcessError, EnvironmentError):
        return fromVersionHeader()
    if "-" in d:
        # remove the "g" in describe output
        d = d.replace("-g", "-")
        m1 = d.find("-") + 1
        m2 = d.find("-", m1)
        diff = max(0, 4 - (m2 - m1)) if padZero else 0
        # prefix the number of commits with a "+" and pad with 0
        return d[:m1].replace("-", "+") + (diff * "0") + d[m1:]
    return fromVersionHeader()
