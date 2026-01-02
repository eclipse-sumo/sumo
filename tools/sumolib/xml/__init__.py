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
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Mirko Barthauer
# @date    2011-06-23

from __future__ import print_function
import os
import sys
import datetime
import fileinput

from .. import version
from .. import miscutils
from .parsing import *  # noqa
from . import xsd  # noqa


def buildHeader(script=None, root=None, schemaPath=None, rootAttrs="", options=None, includeXMLDeclaration=False):
    """
    Builds an XML header with schema information and a comment on how the file has been generated
    (script name, arguments and datetime).
    If script name is not given, it is determined from the command line call.
    If root is not given, no root element is printed (and thus no schema).
    If schemaPath is not given, it is derived from the root element.
    If rootAttrs is given as a string, it can be used to add further attributes to the root element.
    If rootAttrs is set to None, the schema related attributes are not printed.
    """
    if script is None or script == "$Id$":
        script = os.path.basename(sys.argv[0])
    if options is None:
        optionString = u"  options: %s\n" % (' '.join(sys.argv[1:]).replace('--', '<doubleminus>'))
    else:
        optionString = options.config_as_string
    if includeXMLDeclaration:
        header = u'<?xml version="1.0" encoding="UTF-8"?>\n\n'
    else:
        header = u''
    header += u'<!-- generated on %s by Eclipse SUMO %s %s\n%s-->\n\n' % (datetime.datetime.now(), script,
                                                                          version.gitDescribe(), optionString)
    if root is not None:
        if rootAttrs is None:
            header += u'<%s>\n' % root
        else:
            if schemaPath is None:
                schemaPath = root + "_file.xsd"
            header += (u'<%s%s xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ' +
                       u'xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/%s">\n') % (root, rootAttrs, schemaPath)
    return header


def writeHeader(outf, script=None, root=None, schemaPath=None, rootAttrs="", options=None, includeXMLDeclaration=True):
    """
    Writes an XML header with schema information and a comment on how the file has been generated
    (script name, arguments and datetime). Please use this as first call whenever you open a
    SUMO related XML file for writing from your script.
    If script name is not given, it is determined from the command line call.
    If root is not given, no root element is printed (and thus no schema).
    If schemaPath is not given, it is derived from the root element.
    If rootAttrs is given as a string, it can be used to add further attributes to the root element.
    If rootAttrs is set to None, the schema related attributes are not printed.
    """
    outf.write(buildHeader(script, root, schemaPath, rootAttrs, options, includeXMLDeclaration))


def insertOptionsHeader(filename, options):
    """
    Inserts a comment header with the options used to call the script into an existing file.
    """
    header = buildHeader(options=options)
    fpath, fbase = os.path.split(filename)
    tmpfile = os.path.join(fpath, "tmp." + fbase)
    with miscutils.openz(tmpfile, 'w') as tmpf:
        with miscutils.openz(filename) as inpf:
            for lineNbr, line in enumerate(inpf):
                if lineNbr == 2:
                    tmpf.write(header)
                tmpf.write(line)
    os.remove(filename)  # on windows, rename does not overwrite
    os.rename(tmpfile, filename)
