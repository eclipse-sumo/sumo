#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    relocate.py
# @author  Michael Behrisch
# @date    2026-02-07

import glob
import hashlib
import os
import shutil
import subprocess
import sys
if sys.platform == "darwin":
    from delocate.tools import get_install_names, set_install_name


def file_hash(path, chunk_size=8192):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(chunk_size), b""):
            h.update(chunk)
    return h.hexdigest()


def compare_directories(dir1, dir2):
    entries = sorted(os.listdir(dir1))
    if entries != sorted(os.listdir(dir2)):
        print(entries, sorted(os.listdir(dir2)))
        return False
    for name in entries:
        path1 = os.path.join(dir1, name)
        path2 = os.path.join(dir2, name)
        if os.path.getsize(path1) != os.path.getsize(path2) or (sys.platform != "darwin" and file_hash(path1) != file_hash(path2)):
            print(path1, path2)
            return False
    return True


def fix_links_and_strip(binaries, lib_dir_prefix):
    if sys.platform == "darwin":
        for bin in binaries:
            for lib in get_install_names(bin):
                if lib.startswith(("@loader_path/.dylibs", "@loader_path/../.dylibs")):
                    set_install_name(bin, lib, lib.replace(".dylibs", "../sumo_data/.libs"))
        lib_dir = os.path.join(lib_dir_prefix, ".dylibs")
        subprocess.check_call(["strip", "-S", "-x"] + binaries + glob.glob(lib_dir + "/*"))
    else:
        for bin in binaries:
            subprocess.check_call(["patchelf", "--force-rpath", "--add-rpath",
                                   "$ORIGIN/../sumo_data/.libs", bin])
        lib_dir = lib_dir_prefix + ".libs"
        subprocess.check_call(["strip", "--strip-unneeded"] + binaries + glob.glob(lib_dir + "/*"))
    return lib_dir


if __name__ == "__main__":
    root = sys.argv[1]
    # preparing sumo_data wheel
    sumo_data = glob.glob(root + "/sumo_data*.whl")[0]
    subprocess.check_call(["wheel", "unpack", "-d", root, sumo_data])
    pack_dirs = [os.path.join(root, "-".join(os.path.basename(sumo_data).split("-")[:2]))]
    data_libs = os.path.join(pack_dirs[0], "sumo_data", ".libs")
    if os.path.exists(data_libs):
        sys.exit("Data lib dir already exists.")

    # relocating eclipse_sumo wheel
    sumo = glob.glob(root + "/eclipse_sumo*.whl")[0]
    subprocess.check_call(["wheel", "unpack", "-d", root, sumo])
    sumo_dir = os.path.join(root, "-".join(os.path.basename(sumo).split("-")[:2]))
    pack_dirs.append(sumo_dir)
    binaries = glob.glob(sumo_dir + "/sumo/bin/*") + glob.glob(sumo_dir + "/sumo/lib/*")
    sumo_libs = fix_links_and_strip(binaries,
                                    os.path.join(sumo_dir, "sumo" if sys.platform == "darwin" else "eclipse_sumo"))
    os.rename(sumo_libs, data_libs)

    # relocating libsumo wheels
    for f in glob.glob(root + "/libsumo*.whl"):
        subprocess.check_call(["wheel", "unpack", "-d", f[:-4], f])
        libsumo_dir = os.path.join(f[:-4], "-".join(os.path.basename(f).split("-")[:2]))
        pack_dirs.append(libsumo_dir)
        libsumo_so = os.path.join(libsumo_dir, "libsumo", "_libsumo.so")
        libsumo_libs = fix_links_and_strip([libsumo_so], os.path.join(libsumo_dir, "libsumo"))
        if compare_directories(data_libs, libsumo_libs):
            shutil.rmtree(libsumo_libs)
        else:
            sys.exit("Lib mismatch.")

    # packing resulting wheels
    dest_dir = os.path.join(root, "relocate")
    os.makedirs(dest_dir)
    for pd in pack_dirs:
        subprocess.check_call(["wheel", "pack", "-d", dest_dir, pd])
