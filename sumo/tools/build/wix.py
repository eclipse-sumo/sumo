#!/usr/bin/env python
"""
@file    wix.py
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2011
@version $Id$

Builds the installer based on the nightly zip.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2011-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
import optparse
import subprocess
import zipfile
import os
import sys
import tempfile
import glob
import shutil

INPUT_DEFAULT = r"O:\Daten\Sumo\daily\sumo-win32-svn.zip"
OUTPUT_DEFAULT = "sumo.msi"
WIX_DEFAULT = "%sbin" % os.environ.get(
    "WIX", r"D:\Programme\Windows Installer XML v3.5\\")
WXS_DEFAULT = os.path.join(
    os.path.dirname(__file__), "..", "..", "build", "wix", "*.wxs")
LICENSE = os.path.join(
    os.path.dirname(__file__), "..", "..", "build", "wix", "License.rtf")

SKIP_FILES = ["osmWebWizard.py", "sumo-gui.exe",
              "netedit.exe", "start-command-line.bat"]


def buildFragment(wixBin, sourceDir, targetLabel, tmpDir, log=None):
    base = os.path.basename(sourceDir)
    subprocess.call([os.path.join(wixBin, "heat.exe"), "dir", sourceDir,
                     "-cg", base, "-gg", "-dr", targetLabel, "-sreg",
                     "-out", os.path.join(tmpDir, base + "RawFragment.wxs")],
                    stdout=log, stderr=log)
    fragIn = open(os.path.join(tmpDir, base + "RawFragment.wxs"))
    fragOut = open(os.path.join(tmpDir, base + "Fragment.wxs"), "w")
    skip = 0
    for l in fragIn:
        for s in SKIP_FILES:
            if s in l:
                skip = 3
        if skip == 0:
            fragOut.write(l.replace("SourceDir", sourceDir))
        else:
            skip -= 1
    fragOut.close()
    fragIn.close()
    return fragOut.name


def buildMSI(sourceZip=INPUT_DEFAULT, outFile=OUTPUT_DEFAULT,
             wixBin=WIX_DEFAULT, wxsPattern=WXS_DEFAULT,
             license=LICENSE, log=None):
    tmpDir = tempfile.mkdtemp()
    zipfile.ZipFile(sourceZip).extractall(tmpDir)
    sumoRoot = glob.glob(os.path.join(tmpDir, "sumo-*"))[0]
    fragments = [buildFragment(wixBin, os.path.join(
        sumoRoot, d), "INSTALLDIR", tmpDir, log) for d in ["bin", "data", "tools"]]
    for d in ["userdoc", "pydoc", "javadoc", "tutorial", "examples"]:
        fragments.append(
            buildFragment(wixBin, os.path.join(sumoRoot, "docs", d), "DOCDIR", tmpDir, log))
    for wxs in glob.glob(wxsPattern):
        with open(wxs) as wxsIn:
            with open(os.path.join(tmpDir, os.path.basename(wxs)), "w") as wxsOut:
                for l in wxsIn:
                    l = l.replace("License.rtf", license)
                    dataDir = os.path.dirname(license)
                    for data in ["bannrbmp.bmp", "dlgbmp.bmp"]:
                        l = l.replace(data, os.path.join(dataDir, data))
                    wxsOut.write(
                        l.replace(r"O:\Daten\Sumo\Nightly", os.path.join(sumoRoot, "bin")))
        fragments.append(wxsOut.name)
    subprocess.call([os.path.join(wixBin, "candle.exe"),
                     "-o", tmpDir + "\\"] + fragments,
                    stdout=log, stderr=log)
    wixObj = [f.replace(".wxs", ".wixobj") for f in fragments]
    subprocess.call([os.path.join(wixBin, "light.exe"),
                     "-ext", "WixUIExtension", "-o", outFile] + wixObj,
                    stdout=log, stderr=log)
    shutil.rmtree(tmpDir, True)  # comment this out when debugging

if __name__ == "__main__":
    optParser = optparse.OptionParser()
    optParser.add_option("-n", "--nightly-zip", dest="nightlyZip",
                         default=INPUT_DEFAULT, help="full path to nightly zip")
    optParser.add_option("-o", "--output", default=OUTPUT_DEFAULT,
                         help="full path to output file")
    optParser.add_option(
        "-w", "--wix", default=WIX_DEFAULT, help="path to the wix binaries")
    optParser.add_option(
        "-x", "--wxs", default=WXS_DEFAULT, help="pattern for wxs templates")
    optParser.add_option(
        "-l", "--license", default=LICENSE, help="path to the license")
    (options, args) = optParser.parse_args()
    buildMSI(options.nightlyZip, options.output,
             options.wix, options.wxs, options.license)
