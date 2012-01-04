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
optParser.add_option("-w", "--wix", default="%sbin" % os.environ.get("WIX", r"C:\Programme\Windows Installer XML v3.5\\"),
                     help="path to the wix binaries")
optParser.add_option("-x", "--wxs", default=os.path.join(os.path.dirname(__file__), "..", "..", "build", "sumo.wxs"),
                     help="path to wxs template")
optParser.add_option("-l", "--license", default=os.path.join(os.path.dirname(__file__), "..", "..", "build", "License.rtf"),
                     help="path to the license")
(options, args) = optParser.parse_args()

#tmpDir = r"C:\Users\behr_mi\AppData\Local\Temp\tmpm34etb"
tmpDir = tempfile.mkdtemp()
zipfile.ZipFile(options.nightlyZip).extractall(tmpDir)
fragments = []
for d in ["userdoc", "pydoc", "tutorial"]:
    subprocess.call([os.path.join(options.wix, "heat.exe"), "dir", os.path.join(tmpDir, "sumo-svn", "docs", d),
                     "-cg", d, "-gg", "-dr", "DOCDIR", "-out", os.path.join(tmpDir, "Fragment.wxs")])
    fragIn = open(os.path.join(tmpDir, "Fragment.wxs"))
    fragOut = open(os.path.join(tmpDir, d+"Fragment.wxs"), "w")
    for l in fragIn:
        fragOut.write(l.replace("SourceDir", os.path.join(tmpDir, "sumo-svn", "docs", d)))
    fragOut.close()
    fragIn.close()
    fragments.append(fragOut.name)
wxsIn = open(options.wxs)
wxsOut = open(os.path.join(tmpDir, "sumo.wxs"), "w")
for l in wxsIn:
    l = l.replace("License.rtf", options.license)
    wxsOut.write(l.replace(r"M:\Daten\Sumo\Nightly", r"sumo-svn\bin"))
wxsOut.close()
wxsIn.close()
subprocess.call([os.path.join(options.wix, "candle.exe"), "-o", tmpDir+"\\", wxsOut.name] + fragments)
wixObj = [f.replace(".wxs", ".wixobj") for f in [wxsOut.name] + fragments] 
subprocess.call([os.path.join(options.wix, "light.exe"),  "-ext", "WixUIExtension", "-o", options.output] + wixObj)
