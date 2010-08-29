#!/usr/bin/env python
"""
@file    checkSvnProps.py
@author  Michael.Behrisch@dlr.de
@date    2010
@version $Id: apply_astyle.py 8385 2010-03-05 20:51:30Z behrisch $

Checks svn property settings for all files in src, tests and tools.

Copyright (C) 2010 DLR/TS, Germany
All rights reserved
"""

import os, subprocess, sys

doFix = len(sys.argv) == 2 and sys.argv[1] in ["-f" , "--fix"]
srcRoot = os.path.normpath(os.path.join(os.path.dirname(sys.argv[0]), "../../src/"))
testsRoot = os.path.normpath(os.path.join(os.path.dirname(sys.argv[0]), "../../tests/"))
toolsRoot = os.path.normpath(os.path.join(os.path.dirname(sys.argv[0]), "../../tools/"))
for rootDir in [srcRoot, testsRoot, toolsRoot]:
    for root, dirs, files in os.walk(rootDir):
        for name in files:
            ext = os.path.splitext(name)[1]
            fullName = os.path.join(root, name)
            if ext in [".h", ".cpp", ".py", ".pl", ".java"]:
                output = subprocess.Popen(["svn", "pl", "-v", fullName], stdout=subprocess.PIPE).communicate()[0]
                for line in output.splitlines():
                    l = line.split()
                    if l[0] == "svn:eol-style" and l[2] != "LF":
                        print fullName, line
                        if doFix:#it might be necessary to call: sed -i 's/\r$//'
                            subprocess.call(["svn", "ps", "svn:eol-style", "LF", fullName])
                    if l[0] == "svn:keywords" and sorted(l[2:]) != ["HeadURL", "Id", "LastChangedBy",
                                                                    "LastChangedDate", "LastChangedRevision"]:
                        print fullName, line
                        if doFix:
                            subprocess.call(["svn", "ps", "svn:keywords", "HeadURL Id LastChangedBy LastChangedDate LastChangedRevision", fullName])
                    if l[0] == "svn:executable" and ext in [".h", ".cpp", ".java"]:
                        print fullName, line
                        if doFix:
                            subprocess.call(["svn", "pd", "svn:executable", fullName])
            for ignoreDir in ['.svn', 'foreign']:
                if ignoreDir in dirs:
                    dirs.remove(ignoreDir)

for root, dirs, files in os.walk(testsRoot):
    for name in files:
        ext = os.path.splitext(name)[1]
        fullName = os.path.join(root, name)
        if ext in [".xml", ".complex", ".dfrouter", ".duarouter", ".jtrrouter", ".netconvert", ".netgen", ".od2trips", ".polyconvert", ".sumo", ".meso", ".tools", ".traci", ".activitygen"]:
            output = subprocess.Popen(["svn", "pl", "-v", fullName], stdout=subprocess.PIPE).communicate()[0]
            for line in output.splitlines():
                l = line.split()
                if l[0] == "svn:eol-style" and l[2] != "LF":
                    print fullName, line
                    if doFix:
                        subprocess.call(["svn", "ps", "svn:eol-style", "LF", fullName])
                if l[0] == "svn:executable":
                    print fullName, line
                    if doFix:
                        subprocess.call(["svn", "pd", "svn:executable", fullName])
        for ignoreDir in ['.svn', 'foreign']:
            if ignoreDir in dirs:
                dirs.remove(ignoreDir)
