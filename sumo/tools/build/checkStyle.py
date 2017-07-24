#!/usr/bin/env python
"""
@file    checkStyle.py
@author  Michael Behrisch
@date    2010-08-29
@version $Id$

Checks svn property settings for all files and pep8 for python
as well as file headers.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import xml.sax
import codecs
from optparse import OptionParser
try:
    import flake8
    HAVE_FLAKE = True
except ImportError:
    HAVE_FLAKE = False
try:
    import autopep8
    HAVE_AUTOPEP = True
except ImportError:
    HAVE_AUTOPEP = False

_SOURCE_EXT = [".h", ".cpp", ".py", ".pl", ".java", ".am", ".cs"]
_TESTDATA_EXT = [".xml", ".prog", ".csv",
                 ".complex", ".dfrouter", ".duarouter", ".jtrrouter", ".marouter",
                 ".astar", ".chrouter", ".internal", ".tcl", ".txt",
                 ".netconvert", ".netedit", ".netgen",
                 ".od2trips", ".polyconvert", ".sumo",
                 ".meso", ".tools", ".traci", ".activitygen",
                 ".scenario", ".tapasVEU",
                 ".sumocfg", ".netccfg", ".netgcfg"]
_VS_EXT = [".vsprops", ".sln", ".vcproj",
           ".bat", ".props", ".vcxproj", ".filters"]
_IGNORE = set(["binstate.sumo", "binstate.sumo.meso", "image.tools"])
_KEYWORDS = "HeadURL Id LastChangedBy LastChangedDate LastChangedRevision"

SEPARATOR = "/****************************************************************************/\n"
LICENSE_HEADER = """/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
"""



class PropertyReader(xml.sax.handler.ContentHandler):

    """Reads the svn properties of files as written by svn pl -v --xml"""

    def __init__(self, doFix):
        self._fix = doFix
        self._file = ""
        self._property = None
        self._value = ""
        self._hadEOL = False
        self._hadKeywords = False

    def checkFileHeader(self, ext):
        haveFixed = False
        if ext in (".cpp", ".h"):
            lines = open(self._file).readlines()
            if lines and lines[0] == SEPARATOR:
                fileRef = "/// @file    %s\n" % os.path.basename(self._file)
                if lines[1] != fileRef:
                    print(self._file, "broken @file reference", lines[1].rstrip())
                    if self._fix and lines[1].startswith("/// @file"):
                        lines[1] = fileRef
                        haveFixed = True
                s = lines[2].split()
                if s[:2] != ["///", "@author"]:
                    print(self._file, "broken @author reference", s)
                idx = 3
                while lines[idx].split()[:2] == ["///", "@author"]:
                    idx += 1
                s = lines[idx].split()
                if s[:2] != ["///", "@date"]:
                    print(self._file, "broken @date reference", s)
                idx += 1
                s = lines[idx].split()
                if s[:2] != ["///", "@version"]:
                    print(self._file, "broken @version reference", s)
                idx += 1
                if lines[idx] != "///\n":
                    print(self._file, "missing empty line", idx, lines[idx].rstrip())
                idx += 1
                while lines[idx].split()[:1] == ["//"]:
                    idx += 1
                if lines[idx] != SEPARATOR:
                    print(self._file, "missing license start", idx, lines[idx].rstrip())
                year = lines[idx + 2][17:21]
                license = LICENSE_HEADER.replace("2001", year)
                if "module" in lines[idx + 3]:
                    fileLicense = "".join(lines[idx:idx+3]) + "".join(lines[idx+5:idx+14])
                else:
                    fileLicense = "".join(lines[idx:idx+12])
                if fileLicense != license:
                    print(self._file, "invalid license")
                    if options.verbose:
                        print("".join(lines[idx:idx+12]))
                        print(license)
            else:
                if len(lines) == 0:
                    print(self._file, "is empty")
                else:
                    print(self._file, "header does not start")
            if haveFixed:
                open(self._file, "w").write("".join(lines))

    def startElement(self, name, attrs):
        if name == 'target':
            self._file = attrs['path']
            seen.add(os.path.join(svnRoot, self._file))
        if name == 'property':
            self._property = attrs['name']

    def characters(self, content):
        if self._property:
            self._value += content

    def endElement(self, name):
        ext = os.path.splitext(self._file)[1]
        if name == 'property' and self._property == "svn:eol-style":
            self._hadEOL = True
        if name == 'property' and self._property == "svn:keywords":
            self._hadKeywords = True
        if os.path.basename(self._file) not in _IGNORE:
            if ext in _SOURCE_EXT or ext in _TESTDATA_EXT or ext in _VS_EXT:
                if name == 'property' and self._property == "svn:executable" and ext not in [".py", ".pl", ".bat"]:
                    print(self._file, self._property, self._value)
                    if self._fix:
                        subprocess.call(
                            ["svn", "pd", "svn:executable", self._file])
                if name == 'property' and self._property == "svn:mime-type":
                    print(self._file, self._property, self._value)
                    if self._fix:
                        subprocess.call(
                            ["svn", "pd", "svn:mime-type", self._file])
            if ext in _SOURCE_EXT or ext in _TESTDATA_EXT:
                if name == 'property' and self._property == "svn:eol-style" and self._value != "LF"\
                   or name == "target" and not self._hadEOL:
                    print(self._file, "svn:eol-style", self._value)
                    if self._fix:
                        if os.name == "posix":
                            subprocess.call(
                                ["sed", "-i", r's/\r$//', self._file])
                            subprocess.call(
                                ["sed", "-i", r's/\r/\n/g', self._file])
                        subprocess.call(
                            ["svn", "ps", "svn:eol-style", "LF", self._file])
            if ext in _SOURCE_EXT:
                if name == 'property' and self._property == "svn:keywords" and self._value != _KEYWORDS\
                   or name == "target" and not self._hadKeywords:
                    print(self._file, "svn:keywords", self._value)
                    if self._fix:
                        subprocess.call(
                            ["svn", "ps", "svn:keywords", _KEYWORDS, self._file])
                if name == 'target':
                    try:
                        codecs.open(self._file, 'r', 'utf8').read()
                    except UnicodeDecodeError as e:
                        print(self._file, e)
                    self.checkFileHeader(ext)
            if ext in _VS_EXT:
                if name == 'property' and self._property == "svn:eol-style" and self._value != "CRLF"\
                   or name == "target" and not self._hadEOL:
                    print(self._file, "svn:eol-style", self._value)
                    if self._fix:
                        subprocess.call(
                            ["svn", "ps", "svn:eol-style", "CRLF", self._file])
            if name == 'target' and ext == ".py"and "/contributed/" not in self._file:
                if HAVE_FLAKE and os.path.getsize(self._file) < 1000000:  # flake hangs on very large files
                    subprocess.call(["flake8", "--max-line-length", "120", self._file])
                if HAVE_AUTOPEP and self._fix:
                    subprocess.call(["autopep8", "--max-line-length", "120", "--in-place", self._file])
        if name == 'property':
            self._value = ""
            self._property = None
        if name == 'target':
            self._hadEOL = False
            self._hadKeywords = False


sumoRoot = os.path.dirname(
    os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
svnRoots = [sumoRoot]
optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true",
                     default=False, help="tell me what you are doing")
optParser.add_option("-f", "--fix", action="store_true",
                     default=False, help="fix invalid svn properties")
(options, args) = optParser.parse_args()
seen = set()
if len(args) > 0:
    svnRoots = [os.path.abspath(a) for a in args]
else:
    upDir = os.path.dirname(sumoRoot)
    for l in subprocess.Popen(["svn", "pg", "svn:externals", upDir], stdout=subprocess.PIPE, stderr=open(os.devnull, 'w')).communicate()[0].splitlines():
        if l[:5] == "sumo/":
            svnRoots.append(os.path.join(upDir, l.split()[0]))
for svnRoot in svnRoots:
    if options.verbose:
        print("checking", svnRoot)
    output = subprocess.check_output(["svn", "pl", "-v", "-R", "--xml", svnRoot])
    xml.sax.parseString(output, PropertyReader(options.fix))
    if options.verbose:
        print("re-checking tree at", svnRoot)
    for root, dirs, files in os.walk(svnRoot):
        for name in files:
            ext = os.path.splitext(name)[1]
            if name not in _IGNORE:
                if ext in _SOURCE_EXT or ext in _TESTDATA_EXT or ext in _VS_EXT:
                    fullName = os.path.join(root, name)
                    if fullName in seen or subprocess.call(["svn", "ls", fullName], stdout=open(os.devnull, 'w'), stderr=subprocess.STDOUT):
                        continue
                    print(fullName, "svn:eol-style")
                    if options.fix:
                        if ext in _VS_EXT:
                            subprocess.call(
                                ["svn", "ps", "svn:eol-style", "CRLF", fullName])
                        else:
                            if os.name == "posix":
                                subprocess.call(["sed", "-i", 's/\r$//', fullName])
                            subprocess.call(
                                ["svn", "ps", "svn:eol-style", "LF", fullName])
                if ext in _SOURCE_EXT:
                    print(fullName, "svn:keywords")
                    if options.fix:
                        subprocess.call(
                            ["svn", "ps", "svn:keywords", _KEYWORDS, fullName])
        for ignoreDir in ['.svn', 'foreign', 'contributed', 'texttesttmp']:
            if ignoreDir in dirs:
                dirs.remove(ignoreDir)
