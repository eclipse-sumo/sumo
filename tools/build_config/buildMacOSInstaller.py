#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    buildMacOSInstaller.py
# @author  Robert Hilbrich
# @date    2024-07-16

# Creates the macOS installer for the current version of SUMO.
# It requires a build to have completed successfully.

import os
import plistlib
import shutil
import subprocess
import sys
import tempfile

import sumolib


def parse_args():

    op = sumolib.options.ArgumentParser(description="Build macOS installer for sumo",
                                        usage="Usage: " + sys.argv[0] + " -b <build_directory>")
    op.add_argument("-b", "--build-directory", dest="build_directory", required=True,
                    help="The build directory of sumo to take the binaries from")
    op.add_argument("-n", "--name", dest="name", default="EclipseSUMO",
                    help="The name of the framework (default: EclipseSUMO)")
    op.add_argument("-l", "--longname", dest="longname", default="Eclipse SUMO",
                    help="The long name of the framework (default: Eclipse SUMO)")
    op.add_argument("-i", "--id", dest="id", default="org.eclipse.sumo",
                    help="The identifier of the framework (default: org.eclipse.sumo)")
    op.add_argument("-v", "--version", dest="version", default=sumolib.version.gitDescribe(),
                    help="The version of the framework (default: version from sumolib.version.gitDescribe())")

    return op.parse_args()


def create_sumo_framework(name, longname, id, version, sumo_build_directory):
    print(f"Building framework package '{name}'")

    print(" - Creating directory structure")
    temp_dir = tempfile.mkdtemp()

    # Create the directory structure for the framework bundle
    # see: https://developer.apple.com/library/archive/documentation/MacOSX/Conceptual/BPFrameworks/Concepts/FrameworkAnatomy.html
    #
    # EclipseSUMO.framework/
    # ├── EclipseSUMO   --> Versions/Current/EclipseSUMO
    # ├── Resources     --> Versions/Current/Resources
    # └── Versions
    #     ├── v1_20_0
    #     │   ├── EclipseSUMO
    #     │   └── Resources
    #     │       └── Info.plist
    #     └── Current   --> v_1_20_0

    framework_dir = os.path.join(temp_dir, f'{name}.framework')
    version_dir = os.path.join(framework_dir, f'Versions/{version}')
    os.makedirs(os.path.join(version_dir, name), exist_ok=True)
    os.makedirs(os.path.join(version_dir, 'Resources'), exist_ok=True)

    os.symlink(f'{version}/', os.path.join(framework_dir, 'Versions/Current'), target_is_directory=True)
    os.symlink(f'Versions/Current/{name}/', os.path.join(framework_dir, name), target_is_directory=True)
    os.symlink('Versions/Current/Resources/', os.path.join(framework_dir, 'Resources'), target_is_directory=True)

    # Create the Info.plist file
    plist_file = os.path.join(version_dir, 'Resources', 'Info.plist')
    print(" - Creating plist file")
    plist_content = {
        "CFBundleExecutable": longname,
        "CFBundleIdentifier": id,
        "CFBundleName": longname,
        "CFBundleVersion": version,
        "CFBundleShortVersionString": version
    }
    with open(plist_file, 'wb') as f:
        plistlib.dump(plist_content, f)

    # Copy files from the current repository clone to version_dir/EclipseSUMO
    print(" - Calling cmake install")
    cmake_install_command = [
        "cmake",
        "--install",
        sumo_build_directory,
        "--prefix",
        os.path.join(version_dir, name)
    ]
    subprocess.run(cmake_install_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Also copy dependencies
    # FIXME ...

    # Build the framework package
    pkg_name = f"{name}-{version}.pkg"
    pkg_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', pkg_name)
    pkg_build_command = [
        "pkgbuild",
        "--root",
        framework_dir,
        "--identifier",
        id,
        "--version",
        version,
        "--install-location",
        f"/Library/Frameworks/{name}.framework",
        f"{pkg_path}"
    ]
    print(" - Calling pkgbuild")
    subprocess.run(pkg_build_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    pkg_size = os.path.getsize(pkg_path)

    # Cleanup the temporary directory
    print(" - Cleaning up")
    shutil.rmtree(temp_dir)

    # Done
    print(f"Successfully built: '{pkg_name}' ({pkg_size / (1024 * 1024):.2f} MB)")
    print(f"Hint: install the package with 'sudo installer -pkg {pkg_path} -target /'")

    return pkg_path


def main():
    options = parse_args()
    if not os.path.exists(options.build_directory):
        print(f"Error: The sumo build directory '{options.build_directory}' does not exist.", file=sys.stderr)
        sys.exit(1)
    if not os.path.exists(os.path.join(options.build_directory, 'CMakeCache.txt')):
        print(f"Error: The directory '{options.build_directory}' is not a build directory.", file=sys.stderr)
        sys.exit(1)

    create_sumo_framework(options.name, options.longname, options.id, options.version, options.build_directory)


if __name__ == "__main__":
    main()
