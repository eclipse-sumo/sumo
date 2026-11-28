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

import plistlib
import subprocess
from os.path import dirname, exists, join


_UNKNOWN_VERSION = ".".join(("0", "0", "0"))
# this is the fallback version, it gets replaced with the current version on "make install" or "make dist"
_FALLBACK_VERSION = "0.0.0"


def _detect_metadata_version():
    try:
        import importlib.metadata  # noqa
        for pkg in ("sumolib", "eclipse-sumo"):
            try:
                return importlib.metadata.version(pkg)
            except importlib.metadata.PackageNotFoundError:
                continue
    except ImportError:
        pass
    return _FALLBACK_VERSION


_version = _detect_metadata_version()
_cached_bin_version = {}

GITDIR = join(dirname(__file__), '..', '..', '.git')


def _parse_version_header(versionFile):
    if exists(versionFile):
        with open(versionFile) as f:
            version = f.read().split()
        if len(version) > 2:
            return version[2][1:-1]
    return None


def _version_from_plist(baseDir):
    for rel in (
        join(baseDir, '..', '..', '..', 'Resources', 'Info.plist'),
        join(baseDir, '..', 'Resources', 'Info.plist'),
        join(baseDir, 'Resources', 'Info.plist'),
    ):
        if exists(rel):
            try:
                with open(rel, 'rb') as pf:
                    data = plistlib.load(pf)
                ver = data.get("CFBundleShortVersionString") or data.get("CFBundleVersion")
                if ver:
                    return ver if ver.startswith("v") else ("v" + ver)
            except Exception:
                continue
    return None


def _version_from_binary(baseDir):
    if baseDir in _cached_bin_version:
        return _cached_bin_version[baseDir]
    exeNames = ("sumo", "sumo.exe")
    for relDir in (join(baseDir, 'bin'), join(baseDir, '..', '..', 'bin')):
        for exe in exeNames:
            cand = join(relDir, exe)
            if exists(cand):
                try:
                    out = subprocess.check_output([cand, "--version"], universal_newlines=True)
                    for line in out.splitlines():
                        if "Version " in line:
                            tok = line.split("Version ", 1)[1].split()[0].strip()
                            if tok and tok != "UNKNOWN":
                                res = tok if tok.startswith("v") else ("v" + tok)
                                _cached_bin_version[baseDir] = res
                                return res
                except (subprocess.CalledProcessError, EnvironmentError):
                    continue
    _cached_bin_version[baseDir] = None
    return None


def fromVersionHeader(rootDir=None, curVersion=None):
    """
    Returns the version as defined in "include/version.h" (or build "src/version.h"),
    "src/config.h.cmake", installed wheel metadata, macOS framework "Info.plist",
    or sibling "bin/sumo --version".
    """
    baseDir = rootDir if rootDir is not None else join(dirname(__file__), '..', '..')
    effVer = _version if curVersion is None else curVersion

    vfiles = [join(baseDir, 'include', 'version.h'), join(baseDir, 'src', 'version.h')]
    if rootDir is None:
        vfiles.append(join('src', 'version.h'))
    for vfile in vfiles:
        parsed = _parse_version_header(vfile)
        if parsed:
            return parsed

    configFile = join(baseDir, 'src', 'config.h.cmake')
    if exists(configFile):
        with open(configFile) as f:
            config = f.read()
        idx = config.find("VERSION_STRING") + 16
        if idx > 16:
            return "v" + config[idx:config.find('"\n', idx)] + "-" + (10 * "0")

    if effVer and effVer != _UNKNOWN_VERSION:
        return effVer if effVer.startswith("v") else ("v" + effVer)

    plistVer = _version_from_plist(baseDir)
    if plistVer:
        return plistVer

    binVer = _version_from_binary(baseDir)
    if binVer:
        return binVer

    return "v" + (effVer or _UNKNOWN_VERSION)


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
