#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    wix.py
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @date    2011

from __future__ import absolute_import
import optparse
import zipfile
import os
import tempfile
import glob
import shutil

import version
import status

SUMO_VERSION = version.get_pep440_version().replace("post", "")
INPUT_DEFAULT = r"S:\daily\sumo-win64-git.zip"
OUTPUT_DEFAULT = "sumo.msi"
WIX_DEFAULT = os.path.join(os.environ.get("WIX", r"C:\Program Files (x86)\WiX Toolset v3.11"), "bin")
WXS_DEFAULT = os.path.join(os.path.dirname(__file__), "..", "..", "build", "wix", "*.wxs")
LICENSE = os.path.join(os.path.dirname(__file__), "..", "..", "build", "wix", "License.rtf")

SKIP_FILES = ["osmWebWizard.py", "sumo-gui.exe",
              "netedit.exe", "start-command-line.bat"]


def buildFragment(wixBin, sourceDir, targetLabel, tmpDir):
    base = os.path.basename(sourceDir)
    status.log_subprocess([os.path.join(wixBin, "heat.exe"), "dir", sourceDir,
                           "-cg", base, "-gg", "-dr", targetLabel, "-sreg",
                           "-out", os.path.join(tmpDir, base + "RawFragment.wxs")])
    fragIn = open(os.path.join(tmpDir, base + "RawFragment.wxs"))
    fragOut = open(os.path.join(tmpDir, base + "Fragment.wxs"), "w")
    skip = 0
    for fl in fragIn:
        for s in SKIP_FILES:
            if s in fl:
                skip = 3
        if skip == 0:
            fragOut.write(fl.replace("SourceDir", sourceDir))
        else:
            skip -= 1
    fragOut.close()
    fragIn.close()
    return fragOut.name


def buildMSI(sourceZip=INPUT_DEFAULT, outFile=OUTPUT_DEFAULT,
             wixBin=WIX_DEFAULT, wxsPattern=WXS_DEFAULT,
             license=LICENSE):
    tmpDir = tempfile.mkdtemp()
    zipfile.ZipFile(sourceZip).extractall(tmpDir)
    sumoRoot = glob.glob(os.path.join(tmpDir, "sumo-*"))[0]
    fragments = [buildFragment(wixBin, os.path.join(sumoRoot, d), "INSTALLDIR", tmpDir)
                 for d in ["bin", "data", "share", "include", "tools"]]
    for d in ["userdoc", "pydoc", "javadoc", "tutorial", "examples"]:
        docDir = os.path.join(sumoRoot, "docs", d)
        if os.path.exists(docDir):
            fragments.append(buildFragment(wixBin, docDir, "DOCDIR", tmpDir))
    for wxs in glob.glob(wxsPattern):
        with open(os.path.join(tmpDir, os.path.basename(wxs)), "w") as wxsOut:
            dataDir = os.path.dirname(license)
            wxsOut.write(open(wxs).read().format(version=SUMO_VERSION, license=license,
                                                 bindir=os.path.join(sumoRoot, "bin"),
                                                 banner=os.path.join(dataDir, "bannrbmp.bmp"),
                                                 dialogbg=os.path.join(dataDir, "dlgbmp.bmp"),
                                                 webwizico=os.path.join(dataDir, "webWizard.ico")))
        fragments.append(wxsOut.name)
    status.log_subprocess([os.path.join(wixBin, "candle.exe"), "-o", tmpDir + "\\"] + fragments)
    wixObj = [f.replace(".wxs", ".wixobj") for f in fragments]
    status.log_subprocess([os.path.join(wixBin, "light.exe"), "-sw1076",
                           "-ext", "WixUIExtension", "-o", outFile] + wixObj)
    shutil.rmtree(tmpDir, True)  # comment this out when debugging


if __name__ == "__main__":
    optParser = optparse.OptionParser()
    optParser.add_option("-n", "--nightly-zip", dest="nightlyZip",
                         default=INPUT_DEFAULT, help="full path to nightly zip")
    optParser.add_option("-o", "--output", default=OUTPUT_DEFAULT,
                         help="full path to output file")
    optParser.add_option("-w", "--wix", default=WIX_DEFAULT, help="path to the wix binaries")
    optParser.add_option("-x", "--wxs", default=WXS_DEFAULT, help="pattern for wxs templates")
    optParser.add_option("-l", "--license", default=LICENSE, help="path to the license")
    (options, args) = optParser.parse_args()
    buildMSI(options.nightlyZip, options.output,
             options.wix, options.wxs, options.license)
