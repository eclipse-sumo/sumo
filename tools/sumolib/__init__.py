# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2025 German Aerospace Center (DLR) and others.
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
import warnings
from optparse import OptionParser

from . import files, net, output, sensors, shapes, statistics, fpdiff  # noqa
from . import color, geomhelper, miscutils, options, route, vehicletype, version  # noqa
# the visualization submodule is not imported to avoid an explicit matplotlib dependency
from .miscutils import openz
from .options import pullOptions
from .xml import writeHeader as writeXMLHeader  # noqa


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


def checkBinary(name, bindir=None):
    """
    Checks for the given binary in the places, defined by the environment
    variables SUMO_HOME and <NAME>_BINARY.
    """

    def exe(binary):
        return binary + ".exe" if os.name == "nt" and binary[-4:] != ".exe" else binary

    envName = "GUISIM_BINARY" if name == "sumo-gui" else name.upper() + "_BINARY"
    env = os.environ
    if envName in env and os.path.exists(exe(env[envName])):
        return exe(env[envName])
    if bindir is not None:
        binary = exe(os.path.join(bindir, name))
        if os.path.exists(binary):
            return binary
    if "SUMO_HOME" in env:
        binary = exe(os.path.join(env.get("SUMO_HOME"), "bin", name))
        if os.path.exists(binary):
            return binary
    try:
        import sumo
        # If there is a directory "sumo" in the current path, the import will succeed, so we need to double check.
        if hasattr(sumo, "SUMO_HOME"):
            binary = exe(os.path.join(sumo.SUMO_HOME, "bin", name))
            if os.path.exists(binary):
                return binary
    except ImportError:
        pass
    if bindir is None:
        binary = exe(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'bin', name)))
        if os.path.exists(binary):
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

    def close(self):
        """closes all closable outputs"""
        for fp in self.files:
            if fp not in (sys.__stdout__, sys.__stderr__) and hasattr(fp, "close"):
                fp.close()


def _intTime(tStr):
    """
    Converts a time given as a string containing a float into an integer representation.
    """
    return int(float(tStr))


def _laneID2edgeID(laneID):
    return laneID[:laneID.rfind("_")]


def open(fileOrURL, tryGZip=True, mode="rb"):
    warnings.warn("sumolib.open is deprecated, due to the name clash and strange signature! "
                  "Use sumolib.miscutils.openz instead.")
    return openz(fileOrURL, mode, tryGZip=tryGZip)
