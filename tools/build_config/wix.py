#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2024 German Aerospace Center (DLR) and others.
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
import os
import sys
import tempfile
import glob
import shutil
import zipfile
from os.path import abspath, basename, dirname, join

import version
import status

sys.path.append(dirname(dirname(abspath(__file__))))
import sumolib  # noqa

SUMO_ROOT = abspath(join(dirname(__file__), '..', '..'))
SUMO_VERSION = version.get_pep440_version().replace("post", "")
INPUT_DEFAULT = r"S:\daily\sumo-win64-git.zip"
OUTPUT_DEFAULT = "sumo.msi"
WIX_DEFAULT = join(os.environ.get("WIX", r"C:\Program Files (x86)\WiX Toolset v3.11"), "bin")
WXS_DEFAULT = join(SUMO_ROOT, "build_config", "wix", "*.wxs")
LICENSE = join(SUMO_ROOT, "build_config", "wix", "License.rtf")

SKIP_FILES = ["osmWebWizard.py", "sumo-gui.exe",
              "netedit.exe", "start-command-line.bat"]


def buildFragment(wix_bin, source_dir, target_label, tmp_dir):
    base = basename(source_dir)
    tmp_base = join(tmp_dir, base)
    status.log_subprocess([join(wix_bin, "heat.exe"), "dir", source_dir,
                           "-cg", base, "-gg", "-dr", target_label, "-sreg",
                           "-out", tmp_base + "RawFragment.wxs"])
    with open(tmp_base + "RawFragment.wxs") as frag_in, open(tmp_base + "Fragment.wxs", "w") as frag_out:
        skip = 0
        for fl in frag_in:
            for s in SKIP_FILES:
                if s in fl:
                    skip = 3
            if skip == 0:
                frag_out.write(fl.replace("SourceDir", source_dir))
            else:
                skip -= 1
    return frag_out.name


def buildMSI(source_zip=INPUT_DEFAULT, out_file=OUTPUT_DEFAULT,
             wix_bin=WIX_DEFAULT, wxs_pattern=WXS_DEFAULT,
             license_path=LICENSE):
    tmp_dir = tempfile.mkdtemp()
    zipfile.ZipFile(source_zip).extractall(tmp_dir)
    extracted_sumo = glob.glob(join(tmp_dir, "sumo-*"))[0]
    fragments = [buildFragment(wix_bin, join(extracted_sumo, d), "INSTALLDIR", tmp_dir)
                 for d in ["bin", "data", "share", "include", "tools"]]
    for d in ["userdoc", "pydoc", "javadoc", "tutorial", "examples"]:
        doc_dir = join(extracted_sumo, "docs", d)
        if os.path.exists(doc_dir):
            fragments.append(buildFragment(wix_bin, doc_dir, "DOCDIR", tmp_dir))
    for wxs in glob.glob(wxs_pattern):
        with open(wxs) as wxs_in, open(join(tmp_dir, basename(wxs)), "w") as wxs_out:
            data_dir = dirname(license_path)
            wxs_out.write(wxs_in.read().format(version=SUMO_VERSION, license=license_path,
                                               bindir=join(extracted_sumo, "bin"),
                                               banner=join(data_dir, "bannrbmp.bmp"),
                                               dialogbg=join(data_dir, "dlgbmp.bmp"),
                                               webwizico=join(data_dir, "webWizard.ico")))
        fragments.append(wxs_out.name)
    status.log_subprocess([join(wix_bin, "candle.exe"), "-o", tmp_dir + "\\"] + fragments)
    wix_obj = [f.replace(".wxs", ".wixobj") for f in fragments]
    status.log_subprocess([join(wix_bin, "light.exe"), "-sw1076",
                           "-ext", "WixUIExtension", "-o", out_file] + wix_obj)
    shutil.rmtree(tmp_dir, True)  # comment this out when debugging


if __name__ == "__main__":
    ap = sumolib.options.ArgumentParser()
    ap.add_argument("-n", "--nightly-zip", default=INPUT_DEFAULT, help="full path to nightly zip")
    ap.add_argument("-o", "--output", default=OUTPUT_DEFAULT, help="full path to output file")
    ap.add_argument("-w", "--wix", default=WIX_DEFAULT, help="path to the wix binaries")
    ap.add_argument("-x", "--wxs", default=WXS_DEFAULT, help="pattern for wxs templates")
    ap.add_argument("-l", "--license", default=LICENSE, help="path to the license")
    options = ap.parse_args()
    buildMSI(options.nightly_zip, options.output, options.wix, options.wxs, options.license)
