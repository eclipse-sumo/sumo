#!/usr/bin/env python
"""
@file    checkSvnProps.py
@author  Michael Behrisch
@date    2010
@version $Id$

Checks authors for all files.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2010-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import os, subprocess, sys, xml.sax
from optparse import OptionParser

_SOURCE_EXT = [".h", ".cpp", ".py", ".pl", ".java"]

class PropertyReader(xml.sax.handler.ContentHandler):
    """Reads the svn properties of files as written by svn log --xml"""

    def __init__(self, outfile):
        self._out = outfile
        self._authors = set()
        self._currAuthor = None
        self._value = ""

    def startElement(self, name, attrs):
        self._value = ""

    def characters(self, content):
        self._value += content

    def endElement(self, name):
        if name == 'author':
            self._currAuthor = self._value
        if name == "msg":
            msg = self._value.lower()
            if "svn" in msg or "#22" in msg or "licen" in msg or "copyright" in msg or "style" in msg:
                return
            try:
                if self._currAuthor not in self._authors:
                    self._authors.add(self._currAuthor)
                    print >> self._out, "@author", realNames.get(self._currAuthor, self._currAuthor)
		    print >> self._out, msg
                if "thank" in msg:
                    print >> self._out, "THANKS", " ".join(msg.splitlines())
            except:
                pass


sumoRoot = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
realNames = {}
for line in open(os.path.join(sumoRoot, 'AUTHORS')):
    entries = line.split()
    author = ""
    authorDone = False
    getAccounts = False
    for e in line.split():
        if e[0] == "<":
            author = author[:-1]
            authorDone = True
        if not authorDone:
            author += e + " "
        if e[-1] == ">":
            getAccounts = True
        if getAccounts:
            realNames[e] = author
for root, dirs, files in os.walk(sumoRoot):
    for name in files:
        if "." in name and name[name.index("."):] in _SOURCE_EXT:
            fullName = os.path.join(root, name)
            print "checking authors for", fullName
            p = subprocess.Popen(["svn", "log", "--xml", fullName], stdout=subprocess.PIPE)
            output = p.communicate()[0]
            if p.returncode == 0:
                xml.sax.parseString(output, PropertyReader(open(fullName+".authors", "w")))
    for ignoreDir in ['.svn', 'foreign', 'contributed']:
        if ignoreDir in dirs:
            dirs.remove(ignoreDir)
