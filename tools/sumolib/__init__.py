# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    __init__.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2011-06-23

from __future__ import absolute_import
import os
import sys
import subprocess
import gzip
import io
import warnings
try:
    from urllib.request import urlopen
except ImportError:
    from urllib import urlopen
from optparse import OptionParser

from . import files, net, output, sensors, shapes, statistics, fpdiff  # noqa
from . import color, geomhelper, miscutils, options, route, vehicletype, version  # noqa
from .options import pullOptions
from .xml import writeHeader as writeXMLHeader  # noqa
# the visualization submodule is not imported to avoid an explicit matplotlib dependency


def saveConfiguration(executable, configoptions, filename):
    configoptions.save_configuration = filename
    call(executable, configoptions)


def call(executable, args):
    optParser = OptionParser()
    pullOptions(executable, optParser)
    cmd = [executable]
    for option, value in args.__dict__.items():
        o = "--" + option.replace("_", "-")
        opt = optParser.get_option(o)
        if opt is not None and value is not None and opt.default != value:
            cmd.append(o)
            if opt.action != "store_true":
                cmd.append(str(value))
    return subprocess.call(cmd)


def exeExists(binary):
    if os.name == "nt" and binary[-4:] != ".exe":
        binary += ".exe"
    return os.path.exists(binary)


def checkBinary(name, bindir=None):
    """
    Checks for the given binary in the places, defined by the environment
    variables SUMO_HOME and <NAME>_BINARY.
    """
    if name == "sumo-gui":
        envName = "GUISIM_BINARY"
    else:
        envName = name.upper() + "_BINARY"
    env = os.environ
    join = os.path.join
    if envName in env and exeExists(env.get(envName)):
        return env.get(envName)
    if bindir is not None:
        binary = join(bindir, name)
        if exeExists(binary):
            return binary
    if "SUMO_HOME" in env:
        binary = join(env.get("SUMO_HOME"), "bin", name)
        if exeExists(binary):
            return binary
    if bindir is None:
        binary = os.path.abspath(join(os.path.dirname(__file__), '..', '..', 'bin', name))
        if exeExists(binary):
            return binary
    if name[-1] != "D" and name[-5:] != "D.exe":
        binaryD = (name[:-4] if name[-4:] == ".exe" else name) + "D"
        found = checkBinary(binaryD, bindir)
        if found != binaryD:
            return found
    return name


class _Running:

    """
    A generator of running, numerical IDs
    Should be enhanced by:
    - a member method for returning the size
    - a member iterator over the stored ids
    """

    def __init__(self, orig_ids=False, warn=False):
        """Contructor"""
        # whether original IDs shall be used instead of an index
        self.orig_ids = orig_ids
        # whether a warning for non-integer IDs shall be given
        self.warn = warn
        # running index of assigned numerical IDs
        self.index = 0
        # map from known IDs to assigned numerical IDs
        self._m = {}

    def g(self, id):
        """
        If the given id is known, the numerical representation is returned,
        otherwise a new running number is assigned to the id and returned"""
        if id not in self._m:
            if self.orig_ids:
                self._m[id] = id
                if self.warn:
                    try:
                        int(id)
                    except ValueError:
                        sys.stderr.write(
                            'Warning: ID "%s" is not an integer.\n' % id)
                        self.warn = False
            else:
                self._m[id] = self.index
                self.index += 1
        return self._m[id]

    def k(self, id):
        """
        Returns whether the given id is known."""
        return id in self._m

    def d(self, id):
        """
        Removed the element."""
        del self._m[id]


class TeeFile:

    """A helper class which allows simultaneous writes to several files"""

    def __init__(self, *outputfiles):
        self.files = outputfiles

    def write(self, txt):
        """Writes the text to all files"""
        for fp in self.files:
            fp.write(txt)

    def flush(self):
        """flushes all file contents to disc"""
        for fp in self.files:
            fp.flush()
            if isinstance(fp, int) or hasattr(fp, "fileno"):
                try:
                    os.fsync(fp)
                except OSError:
                    pass


def _intTime(tStr):
    """
    Converts a time given as a string containing a float into an integer representation.
    """
    return int(float(tStr))


def _laneID2edgeID(laneID):
    return laneID[:laneID.rfind("_")]


def open(fileOrURL, tryGZip=True, mode="rb"):
    warnings.warn("sumolib.open is deprecated, due to the name clash and strange signature! Use openz instead.")
    return openz(fileOrURL, mode, tryGZip=tryGZip)


def openz(fileOrURL, mode="r", **kwargs):
    """
    Opens transparently files, URLs and gzipped files for reading and writing.
    Also enforces UTF8 on text output / input.
    Should be compatible with python 2 and 3.
    """
    try:
        if fileOrURL.startswith("http://") or fileOrURL.startswith("https://"):
            return io.BytesIO(urlopen(fileOrURL).read())
        if fileOrURL.endswith(".gz") and "w" in mode:
            if "b" in mode:
                return gzip.open(fileOrURL, mode="w")
            return gzip.open(fileOrURL, mode="wt", encoding="utf8")
        if kwargs.get("tryGZip", True) and "r" in mode:
            return gzip.open(fileOrURL)
    finally:
        pass
    if "b" in mode:
        return io.open(fileOrURL, mode=mode)
    return io.open(fileOrURL, mode=mode, encoding="utf8")
