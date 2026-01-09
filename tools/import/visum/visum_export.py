#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    visum_export.py
# @author  Michael Behrisch
# @date    2025-10-04

"""
Saves all relevant data (network, demand matrices and routes) from a VISUM .ver file
using VISUM's COM interface and packs them in a zip. If possible the data is retrieved
in the old line based format as well as in XML (ANM). VISUM needs to be installed
and have a current license including COM access on the machine you run this script on.

The easiest way to use it is to copy the script together with run_export.bat
and .ver file in a clean directory and start run_export.bat
which should find an appropriate python.
The script understands two parameters. The first is the .ver file to use (by default the
first one in the same directory as the script), the second is the name of the zip
(default is the name of the .ver but with the .zip ending).
The script is designed to support a wide range of VISUM versions with different COM APIs.
It has been tested successfully with VISUM 13 and VISUM 22.
"""

import glob
import os
import sys
import zipfile

import win32com.client


def load(ver_file):
    try:
        visum = win32com.client.Dispatch("Visum.Visum")
        visum.LoadVersion(os.path.abspath(ver_file))
    except Exception as e:
        print("Could not load Visum or the ver file %s (%s)." % (ver_file, e), file=sys.stderr)
        return None
    return visum


def write(zipf, f, name):
    if os.path.isfile(f):
        zipf.write(f, os.path.basename(name))
        os.remove(f)


def main(visum, ver_file, zip_file):
    out_dir = os.path.dirname(zip_file)
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)
    out_dir = os.path.abspath(out_dir)

    base_name = os.path.splitext(os.path.basename(ver_file))[0]
    network = os.path.join(out_dir, base_name + "_anm_network.xml")
    routes = os.path.join(out_dir, base_name + "_anm_routes.xml")
    matrices = os.path.join(out_dir, base_name + "_anm_matrices.xml")
    net = os.path.join(out_dir, base_name + ".net")
    dmd = os.path.join(out_dir, base_name + ".dmd")
    for f in (network, routes, matrices, net, dmd):
        if os.path.exists(f):
            os.remove(f)
    try:
        visum.SaveNet(net)
        print("Wrote %s successfully." % net)
    except Exception as e:
        print("Could not write net %s (%s)." % (net, e), file=sys.stderr)
    if hasattr(visum, "IO"):
        try:
            visum.IO.ExportANMNet(network, "")
            print("Wrote %s successfully." % network)
        except Exception as e:
            print("Could not write network %s (%s)." % (network, e), file=sys.stderr)
        try:
            visum.IO.ExportANMRoutes(routes, "", True, False)
            print("Wrote %s successfully." % routes)
            visum.IO.ExportANMRoutes(matrices, "", False, True)
            print("Wrote %s successfully." % matrices)
        except Exception as e:
            print("Could not write routes %s (%s)." % (routes, e), file=sys.stderr)
        try:
            visum.IO.SaveDemandFile(dmd, True)
            print("Wrote %s successfully." % dmd)
        except Exception as e:
            print("Could not write dmd %s (%s)." % (dmd, e), file=sys.stderr)
    if not os.path.exists(network + ".anm"):
        try:
            visum.ExportAnmNet(network, "")
            print("Wrote %s successfully." % network)
        except Exception as e:
            print("Could not write network %s (%s)." % (network, e), file=sys.stderr)
    if not os.path.exists(routes + ".anmRoutes"):
        try:
            visum.ExportAnmRoutes(routes, "", True, False)
            print("Wrote %s successfully." % routes)
            visum.ExportAnmRoutes(matrices, "", False, True)
            print("Wrote %s successfully." % matrices)
        except Exception as e:
            print("Could not write routes %s (%s)." % (routes, e), file=sys.stderr)
    if not os.path.exists(dmd):
        try:
            visum.SaveDemandFile(dmd, True)
            print("Wrote %s successfully." % dmd)
        except Exception as e:
            print("Could not write dmd %s (%s)." % (dmd, e), file=sys.stderr)
    with zipfile.ZipFile(zip_file, "w", compression=zipfile.ZIP_DEFLATED) as zipf:
        write(zipf, network + ".anm", network)
        write(zipf, routes + ".anmRoutes", routes)
        write(zipf, matrices + ".anmRoutes", matrices)
        write(zipf, net, net)
        write(zipf, dmd, dmd)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        ver_file = sys.argv[1]
    else:
        ver_files = glob.glob("*.ver")
        if not ver_files:
            sys.exit("No .ver file found in current folder!")
        ver_file = ver_files[0]
    visum = load(ver_file)
    if visum:
        if len(sys.argv) > 2:
            zip_file = sys.argv[2]
        else:
            zip_file = os.path.splitext(ver_file)[0] + ".zip"
        main(visum, ver_file, zip_file)
