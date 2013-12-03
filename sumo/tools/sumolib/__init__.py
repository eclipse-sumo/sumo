# -*- coding: utf-8 -*-
"""
@file    __init__.py
@author  Michael Behrisch
@date    2011-06-23
@version $Id$

Python interface to SUMO especially for parsing xml input and output files.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2011-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os, subprocess
from xml.sax import parseString, handler
from optparse import OptionParser, OptionGroup, Option

from . import net, shapes, output, files, sensors, color, geomhelper, miscutils, options, route

class ConfigurationReader(handler.ContentHandler):
    """Reads a configuration template, storing the options in an OptionParser"""

    def __init__(self, optParse, groups, options):
        self._opts = optParse
        self._groups = groups
        self._options = options
        self._group = self._opts

    def startElement(self, name, attrs):
        if len(attrs) == 0:
            self._group = OptionGroup(self._opts, name)
        if self._group != self._opts and self._groups and self._group.title not in self._groups:
            return
        if "type" in attrs and name != "help":
            if self._options and name not in self._options:
                return
            help = attrs.get("help", "")
            option = Option("--" + name, help=help)
            if attrs["type"] == "BOOL":
                option = Option("--" + name, action="store_true", default=False, help=help)
            elif attrs["type"] in ["FLOAT", "TIME"]:
                option.type = "float"
                if attrs["value"]:
                    option.default = float(attrs["value"])
            elif attrs["type"] == "INT":
                option.type = "int"
                if attrs["value"]:
                    option.default = int(attrs["value"])
            else:
                option.default = attrs["value"]
            self._group.add_option(option)

    def endElement(self, name):
        if self._group != self._opts and name == self._group.title:
            self._opts.add_option_group(self._group)
            self._group = self._opts


def pullOptions(executable, optParse, groups=None, options=None):
    output = subprocess.Popen([executable, "--save-template", "-"], stdout=subprocess.PIPE).communicate()[0]
    parseString(output, ConfigurationReader(optParse, groups, options))

def saveConfiguration(executable, options, filename):
    options.save_configuration = filename
    call(executable, options)

def call(executable, options):
    optParser = OptionParser()
    pullOptions(executable, optParser)
    cmd = [executable]
    for option, value in options.__dict__.iteritems():
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
    """Checks for the given binary in the places, defined by the environment variables SUMO_HOME and SUMO_BINDIR."""
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
    if "SUMO_BINDIR" in env:
        binary = join(env.get("SUMO_BINDIR"), name)
        if exeExists(binary):
            return binary
    if "SUMO_HOME" in env:
        binary = join(env.get("SUMO_HOME"), "bin", name)
        if exeExists(binary):
            return binary
    binary = os.path.abspath(join(os.path.dirname(__file__), '..', '..', 'bin', name))
    if exeExists(binary):
        return binary
    return name

class _Running:
  """
  A generator of running, numerical IDs
  Should be enhanced by:
  - a member method for returning the size
  - a member iterator over the stored ids
  """
  def __init__(self):
    """Contructor"""
    # running index of assigned numerical IDs
    self.index = 0 
    # map from known IDs to assigned numerical IDs
    self._m = {}
    
  def g(self, id):
    """
    If the given id is known, the numerical representation is returned,
    otherwise a new running number is assigned to the id and returned"""
    if id not in self._m:
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



def _intTime(tStr):
  """
  Converts a time given as a string containing a float into an integer representation.
  """
  return int(float(tStr))


def _laneID2edgeID(laneID):
  return laneID[:laneID.rfind("_")]
