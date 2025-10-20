#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
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

import glob
import os
import sys
import zipfile

import win32com.client
from pywintypes import com_error


def load(ver_file):
    try:
        visum = win32com.client.Dispatch("Visum.Visum")
        visum.LoadVersion(os.path.abspath(ver_file))
    except com_error as e:
        print("Could not load Visum or the ver file %s (%s)." % (ver_file, e), file=sys.stderr)
        return None
    return visum


def main(visum, ver_file, zip_file, xml=True):
    out_dir = os.path.dirname(zip_file)
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)
    out_dir = os.path.abspath(out_dir)

    base_name = os.path.splitext(os.path.basename(ver_file))[0]
    if xml:
        network = os.path.join(out_dir, base_name + "_anm_network.xml")
        routes = os.path.join(out_dir, base_name + "_anm_routes.xml")
        matrices = os.path.join(out_dir, base_name + "_anm_matrices.xml")
        for f in (network, routes, matrices):
            if os.path.exists(f):
                os.remove(f)
        if hasattr(visum, "IO"):
            visum.IO.ExportANMNet(network, "")
            visum.IO.ExportANMRoutes(routes, "", True, False)
            visum.IO.ExportANMRoutes(matrices, "", False, True)
        else:
            visum.ExportAnmNet(network, "")
            visum.ExportAnmRoutes(routes, "", True, False)
            visum.ExportAnmRoutes(matrices, "", False, True)
        os.rename(network + ".anm", network)
        os.rename(routes + ".anmRoutes", routes)
        os.rename(matrices + ".anmRoutes", matrices)
    else:
        network = os.path.join(out_dir, base_name + ".net")
        matrices = os.path.join(out_dir, base_name + ".dmd")
        routes = ""
        visum.SaveNet(network)
        if hasattr(visum, "IO"):
            visum.IO.SaveDemandFile(matrices, True)
        else:
            visum.SaveDemandFile(matrices, True)

    with zipfile.ZipFile(zip_file, "w", compression=zipfile.ZIP_DEFLATED) as zipf:
        for f in [network, matrices, routes]:
            if os.path.isfile(f):
                zipf.write(f, os.path.basename(f))
                os.remove(f)


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
            main(visum, ver_file, zip_file)
        else:
            # Default: generate two zip files
            zip_file = os.path.splitext(ver_file)[0] + "_anm.zip"
            main(visum, ver_file, zip_file, True)
            zip_file = os.path.splitext(ver_file)[0] + ".zip"
            main(visum, ver_file, zip_file, False)
