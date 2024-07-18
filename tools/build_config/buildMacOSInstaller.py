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
import tempfile

import sumolib


def run_command(command):
    result = subprocess.run(command, shell=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return result.stdout.decode('utf-8'), result.stderr.decode('utf-8')


def ignore_files(dir, files):
    # Only ignore these directories at the top level
    top_level_ignore = {'.git', '.env', 'tests', 'unittest'}
    if os.path.abspath(dir) == os.path.abspath('.'):
        return top_level_ignore.intersection(files)
    else:
        return {}


def create_sumo_framework(version):
    # Create a temporary directory for the process
    temp_dir = tempfile.mkdtemp()
    print("Building Eclipse SUMO framework package")
    print(f" - Temporary directory created at {temp_dir}")

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

    framework_dir = os.path.join(temp_dir, 'EclipseSUMO.framework')
    version_dir = os.path.join(framework_dir, f'Versions/{version}')
    os.makedirs(os.path.join(version_dir, 'EclipseSUMO'), exist_ok=True)
    os.makedirs(os.path.join(version_dir, 'Resources'), exist_ok=True)

    os.symlink(f'{version}/', os.path.join(framework_dir, 'Versions/Current'), target_is_directory=True)
    os.symlink('Versions/Current/EclipseSUMO/', os.path.join(framework_dir, 'EclipseSUMO'), target_is_directory=True)
    os.symlink('Versions/Current/Resources/', os.path.join(framework_dir, 'Resources'), target_is_directory=True)

    # Create the Info.plist file
    # FIXME: check if BundleName and BundleExecutable can contain spaces
    plist_file = os.path.join(version_dir, 'Resources', 'Info.plist')
    print(f" - Creating plist file {plist_file}")
    plist_content = {
        "CFBundleExecutable": "Eclipse SUMO",
        "CFBundleIdentifier": "org.eclipse.sumo",
        "CFBundleName": "Eclipse SUMO",
        "CFBundleVersion": version,
        "CFBundleShortVersionString": version
    }
    with open(plist_file, 'wb') as f:
        plistlib.dump(plist_content, f)

    # Copy all files from the current repository clone to version_dir/EclipseSUMO
    source_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
    dest_dir = os.path.join(version_dir, 'EclipseSUMO')
    print(f" - Folder 'bin' copied from '{source_dir}' to '{dest_dir}'")
    shutil.copytree(source_dir, dest_dir, dirs_exist_ok=True, ignore=ignore_files)

    # Build the framework package
    # FIXME: check if identifier is ok
    package_name = f"Eclipse-SUMO-{version}.pkg"
    package_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', package_name)

    command = (
        f"pkgbuild --root {framework_dir}                                         "
        f"         --identifier org.eclipse.sumo                                  "
        f"         --version {version}                                            "
        f"         --install-location /Library/Frameworks/EclipseSUMO.framework   "
        f"         {package_path}                                                 "
    )
    run_command(command)
    print(f" - Package built: '{package_path}'")

    # Cleanup the temporary directory
    shutil.rmtree(temp_dir)
    print(f" - Temporary directory {temp_dir} cleaned up")


def main():
    version = sumolib.version.gitDescribe()

    create_sumo_framework(version)


if __name__ == "__main__":
    main()
