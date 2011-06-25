# -*- coding: utf-8 -*-
"""
@file    __init__.py
@author  Michael.Behrisch@dlr.de
@date    2011-06-23
@version $Id$

Python interface to SUMO especially for parsing xml input and output files.

Copyright (C) 2011-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import net, poi, output

import subprocess
from xml.sax import parseString, handler
from optparse import OptionParser, OptionGroup, Option

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
            option = Option("--" + name)
            option.help = attrs.get("help", "")
            if attrs["type"] == "BOOL":
                option.action = "store_true"
                option.default = False
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
    optParser = OptionParser()
    pullOptions(executable, optParser)
    cmd = [executable, "--save-configuration", filename]
    for option, value in options.__dict__.iteritems():
        o = "--" + option.replace("_", "-")
        opt = optParser.get_option(o)
        if value and opt.default != value:
            cmd.append(o)
            if opt.action != "store_true":
                cmd.append(str(value))
    subprocess.call(cmd)
