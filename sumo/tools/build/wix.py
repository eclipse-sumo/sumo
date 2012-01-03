#!/usr/bin/env python
"""
@file    wix.py
@author  Michael Behrisch
@date    2011
@version $Id$

Builds the installer based on the nightly zip.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import optparse, subprocess, zipfile, os, sys, tempfile, shutil

optParser = optparse.OptionParser()
optParser.add_option("-n", "--nightly-zip", dest="nightlyZip",
                     default=r"M:\Daten\Sumo\Nightly\sumo-msvc10Win32-svn.zip",
                     help="full path to nightly zip")
optParser.add_option("-o", "--output",
                     default=r"M:\Daten\Sumo\Nightly\sumo-msvc10Win32-svn.msi",
                     help="full path to output file")
optParser.add_option("-w", "--wix", default=r"C:\Program Files (x86)\Windows Installer XML v3.5\bin",
                     help="path to the wix binaries")
optParser.add_option("-x", "--wxs", default=os.path.join(os.path.dirname(__file__), "..", "..", "build", "sumo.wxs"),
                     help="path to wxs template")
(options, args) = optParser.parse_args()

tmpDir = tempfile.mkdtemp()
wxsIn = open(options.wxs)
wxsOut = open(os.path.join(tmpDir,"sumo.wxs"), "w")
for l in wxsIn:
    print >> wxsOut, l.replace(r"M:\Daten\Sumo\Nightly", r"sumo-svn\bin")
wxsOut.close()
wxsIn.close()
zipfile.ZipFile(options.nightlyZip).extractall(tmpDir)
wixObj = os.path.join(tmpDir,"sumo.wixobj")
subprocess.call([os.path.join(options.wix, "candle.exe"), "-o", wixObj, wxsOut.name])
subprocess.call([os.path.join(options.wix, "light.exe"),  "-ext", "WixUIExtension", "-o", options.output, wixObj])
