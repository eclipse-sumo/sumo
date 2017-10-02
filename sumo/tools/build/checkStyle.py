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
SUMO is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
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
    import flake8  # noqa
    HAVE_FLAKE = True
except ImportError:
    HAVE_FLAKE = False
try:
    import autopep8  # noqa
    HAVE_AUTOPEP = True
except ImportError:
    HAVE_AUTOPEP = False

_SOURCE_EXT = [".h", ".cpp", ".py", ".pyw", ".pl", ".java", ".am", ".cs"]
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
GPL_HEADER = """/****************************************************************************/
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
EPL_HEADER = """/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
"""


class PropertyReader(xml.sax.handler.ContentHandler):

    """Reads the svn properties of files as written by svn pl -v --xml"""

    def __init__(self, doFix, doPep, doEPL):
        self._fix = doFix
        self._pep = doPep
        self._epl = doEPL
        self._file = ""
        self._property = None
        self._value = ""
        self._hadEOL = False
        self._hadKeywords = False

    def checkFileHeader(self, ext):
        lines = open(self._file).readlines()
        if len(lines) == 0:
            print(self._file, "is empty")
            return
        haveFixed = False
        if ext in (".cpp", ".h"):
            if lines[0] == SEPARATOR:
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
                license = GPL_HEADER.replace("2001", year)
                if "module" in lines[idx + 3]:
                    fileLicense = "".join(lines[idx:idx + 3]) + "".join(lines[idx + 5:idx + 14])
                else:
                    fileLicense = "".join(lines[idx:idx + 12])
                if fileLicense != license:
                    print(self._file, "invalid license")
                    if options.verbose:
                        print(fileLicense)
                        print(license)
                elif self._epl:
                    newLicense = EPL_HEADER.replace("2001", year).splitlines(True)
                    end = idx + 12
                    if "module" in lines[idx + 3]:
                        newLicense[3:3] = lines[idx+3:idx+5]
                        end += 2
                    lines[idx:end] = newLicense
                    lines[end-2:end-2] = lines[:idx]
                    lines[:idx] = []
                    haveFixed = True
            else:
                print(self._file, "header does not start")
        if ext in (".py", ".pyw"):
            idx = 0
            if lines[0][:2] == '#!':
                idx += 1
                if lines[0] != '#!/usr/bin/env python\n':
                    print(self._file, "wrong shebang")
                    if self._fix:
                        lines[0] = '#!/usr/bin/env python\n'
                        haveFixed = True
            if lines[idx][:5] == '# -*-':
                idx += 1
            if lines[idx] != '"""\n':
                print(self._file, "header does not start")
            else:
                idx += 1
                pre = idx
                fileRef = "@file    %s\n" % os.path.basename(self._file)
                if lines[idx] != fileRef:
                    print(self._file, "broken @file reference", lines[idx].rstrip())
                    if self._fix and lines[idx].startswith("@file"):
                        lines[idx] = fileRef
                        haveFixed = True
                idx += 1
                if not lines[idx].startswith("@author "):
                    print(self._file, "broken @author reference", lines[idx].rstrip())
                idx += 1
                while lines[idx].startswith("@author "):
                    idx += 1
                if not lines[idx].startswith("@date "):
                    print(self._file, "broken @date reference", lines[idx].rstrip())
                idx += 1
                if not lines[idx].startswith("@version "):
                    print(self._file, "broken @version reference", lines[idx].rstrip())
                idx += 1
                if lines[idx] != "\n":
                    print(self._file, "missing empty line", idx, lines[idx].rstrip())
                atlines = idx
                idx += 1
                while idx < len(lines) and lines[idx][:4] != "SUMO":
                    idx += 1
                if idx == len(lines):
                    print(self._file, "license not found")
                else:
                    year = lines[idx + 1][14:18]
                    license = GPL_HEADER.replace("2001", year).replace(SEPARATOR, "")
                    license = license.replace("//   ", "").replace("// ", "").replace("\n//", "\n")[:-1]
                    if "module" in lines[idx + 2]:
                        fileLicense = "".join(lines[idx:idx + 2]) + "".join(lines[idx + 4:idx + 10])
                    else:
                        fileLicense = "".join(lines[idx:idx + 8])
                    if fileLicense != license:
                        print(self._file, "invalid license")
                        if options.verbose:
                            print("!!%s!!" % os.path.commonprefix([fileLicense, license]))
                            print(fileLicense)
                            print(license)
                    elif self._epl:
                        newLicense = EPL_HEADER.replace("2001", year).replace(SEPARATOR, "")
                        newLicense = newLicense.replace("//   ", "# ").replace("// ", "# ").replace("\n//", "").splitlines(True)
                        end = idx + 8
                        if "module" in lines[idx + 2]:
                            newLicense[2:2] = ["# " + l for l in lines[idx+2:idx+4]]
                            end += 2
                        newLines = lines[:pre-1] + newLicense + ["\n"] + ["# " + l for l in lines[pre:atlines]]
                        if atlines < idx - 3:
                            newLines += ["\n", '"""\n'] + lines[atlines+1:idx-1] + lines[end:]
                        else:
                            newLines += ["\n"] + lines[end+1:]
                        lines = newLines
                        haveFixed = True
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
                if (name == 'property' and self._property == "svn:executable" and
                        ext not in (".py", ".pyw", ".pl", ".bat")):
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
                if ((name == 'property' and self._property == "svn:eol-style" and self._value != "LF") or
                        (name == "target" and not self._hadEOL)):
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
                if ((name == 'property' and self._property == "svn:keywords" and self._value != _KEYWORDS) or
                        (name == "target" and not self._hadKeywords)):
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
                if ((name == 'property' and self._property == "svn:eol-style" and self._value != "CRLF") or
                        (name == "target" and not self._hadEOL)):
                    print(self._file, "svn:eol-style", self._value)
                    if self._fix:
                        subprocess.call(
                            ["svn", "ps", "svn:eol-style", "CRLF", self._file])
            if self._pep and name == 'target' and ext == ".py" and "/contributed/" not in self._file:
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
optParser.add_option("-s", "--skip-pep", action="store_true",
                     default=False, help="skip autopep8 and flake8 tests")
optParser.add_option("-r", "--rewrite-license", action="store_true",
                     default=False, help="change GPL to EPL license")
(options, args) = optParser.parse_args()
seen = set()
if len(args) > 0:
    svnRoots = [os.path.abspath(a) for a in args]
for svnRoot in svnRoots:
    if options.verbose:
        print("checking", svnRoot)
    output = subprocess.check_output(["svn", "pl", "-v", "-R", "--xml", svnRoot])
    xml.sax.parseString(output, PropertyReader(options.fix, not options.skip_pep, options.rewrite_license))
    if options.verbose:
        print("re-checking tree at", svnRoot)
    for root, dirs, files in os.walk(svnRoot):
        for name in files:
            ext = os.path.splitext(name)[1]
            if name not in _IGNORE:
                if ext in _SOURCE_EXT or ext in _TESTDATA_EXT or ext in _VS_EXT:
                    fullName = os.path.join(root, name)
                    if fullName in seen or subprocess.call(["svn", "ls", fullName],
                                                           stdout=open(os.devnull, 'w'), stderr=subprocess.STDOUT):
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
