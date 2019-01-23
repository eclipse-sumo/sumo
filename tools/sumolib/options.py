# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    options.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2012-03-15
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import subprocess
from collections import namedtuple
import re
from xml.sax import parse, handler


def get_long_option_names(application):
    # @todo using option "--save-template stdout" and parsing xml would be prettier
    output = subprocess.check_output([application, '--help'])
    reprog = re.compile(b'(--\S*)\s')
    result = []
    for line in output.splitlines():
        m = reprog.search(line)
        if m:
            result.append(m.group(1).decode('utf-8'))
    return result


Option = namedtuple("Option", ["name", "value", "type", "help"])


class OptionReader(handler.ContentHandler):

    """Reads an option file"""

    def __init__(self):
        self.opts = []

    def startElement(self, name, attrs):
        if 'value' in attrs:
            self.opts.append(
                Option(name, attrs['value'], attrs.get('type'), attrs.get('help')))


def readOptions(filename):
    optionReader = OptionReader()
    try:
        if not os.path.isfile(filename):
            print("Option file '%s' not found" % filename, file=sys.stderr)
            sys.exit(1)
        parse(filename, optionReader)
    except None:
        print("Invalid option file '%s'" % filename, file=sys.stderr)
        sys.exit(1)
    return optionReader.opts
