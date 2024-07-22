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

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from sumolib.options import ArgumentParser  # noqa
from sumolib.version import gitDescribe  # noqa


def parse_args():
    op = ArgumentParser(description="Build an installer for macOS", usage=f"Usage: {sys.argv[0]} -b <build_directory>")
    op.add_argument("-b", "--build-dir", dest="build_dir", required=True, help="Build directory of sumo")
    op.add_argument("-n", "--name", dest="name", default="EclipseSUMO", help="Name of the framework")
    op.add_argument("-l", "--longname", dest="longname", default="Eclipse SUMO", help="Long name of the framework")
    op.add_argument("-i", "--id", dest="id", default="org.eclipse.sumo", help="Identifier of the framework")
    op.add_argument("-v", "--version", dest="version", default=gitDescribe(), help="Version of the framework")

    return op.parse_args()


def get_dependencies(file_path):
    try:
        output = subprocess.check_output(["otool", "-L", file_path], stderr=subprocess.STDOUT).decode("utf-8")
        # Skip the first line which is the file name
        lines = output.split("\n")[1:]
        dependencies = [line.split()[0] for line in lines if line]
        return dependencies
    except subprocess.CalledProcessError as e:
        print(f"Error running otool on {file_path}: {e.output.decode('utf-8')}", file=sys.stderr)
        sys.exit(1)


def filter_libraries(libraries):
    filtered_libraries = []
    for lib in libraries:
        if (
            lib.startswith("/opt/homebrew")
            and not lib.startswith("/opt/homebrew/opt/mesa")
            and not lib.startswith("/opt/homebrew/opt/libx")
        ):
            filtered_libraries.append(lib)
    return filtered_libraries


def create_framework(name, longname, id, version, sumo_build_directory):
    print(" - Creating directory structure")
    temp_dir = tempfile.mkdtemp()

    # Create the directory structure for the framework bundle
    # see: https://developer.apple.com/library/archive/documentation/MacOSX/Conceptual/BPFrameworks/Concepts/FrameworkAnatomy.html  # noqa
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

    framework_dir = os.path.join(temp_dir, f"{name}.framework")
    version_dir = os.path.join(framework_dir, f"Versions/{version}")
    os.makedirs(os.path.join(version_dir, name), exist_ok=True)
    os.makedirs(os.path.join(version_dir, "Resources"), exist_ok=True)

    os.symlink(f"{version}/", os.path.join(framework_dir, "Versions/Current"), True)
    os.symlink(f"Versions/Current/{name}/", os.path.join(framework_dir, name), True)
    os.symlink("Versions/Current/Resources/", os.path.join(framework_dir, "Resources"), True)

    # Create the Info.plist file
    plist_file = os.path.join(version_dir, "Resources", "Info.plist")
    print(" - Creating plist file")
    plist_content = {
        "CFBundleExecutable": longname,
        "CFBundleIdentifier": f"{id}.framework",
        "CFBundleName": longname,
        "CFBundleVersion": version,
        "CFBundleShortVersionString": version,
    }
    with open(plist_file, "wb") as f:
        plistlib.dump(plist_content, f)

    # Copy files from the current repository clone to version_dir/EclipseSUMO
    print(" - Calling cmake install")
    cmake_install_command = [
        "cmake",
        "--install",
        sumo_build_directory,
        "--prefix",
        os.path.join(version_dir, name),
    ]
    subprocess.run(cmake_install_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Determine library dependencies and copy all from homebrew,
    # which are not libx* or mesa*
    print(" - Copying all libraries")
    bin_dir = os.path.join(version_dir, name, "bin")
    libs_dir = os.path.join(version_dir, name, "lib")
    all_libraries = set()
    for root, _, files in os.walk(bin_dir):
        for file in files:
            file_path = os.path.join(root, file)
            dependencies = get_dependencies(file_path)
            all_libraries.update(dependencies)
    filtered_libraries = filter_libraries(all_libraries)

    if not os.path.exists(libs_dir):
        os.makedirs(libs_dir)
    for lib in filtered_libraries:
        shutil.copy(lib, libs_dir)

    # Build the framework package
    cwd = os.path.dirname(os.path.abspath(__file__))
    pkg_name = f"{name}-{version}.pkg"
    pkg_path = os.path.join(cwd, "..", "..", pkg_name)
    pkg_build_command = [
        "pkgbuild",
        "--root",
        framework_dir,
        "--identifier",
        f"{id}.framework",
        "--version",
        version,
        "--install-location",
        f"/Library/Frameworks/{name}.framework",
        f"{pkg_path}",
    ]
    print(" - Calling pkgbuild")
    subprocess.run(pkg_build_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    pkg_size = os.path.getsize(pkg_path)

    # Cleanup the temporary directory
    print(" - Cleaning up")
    shutil.rmtree(temp_dir)

    return pkg_name, pkg_path, pkg_size


def create_app(app_name, binary_name, framework_name, id, version, icns_path):
    print(" - Creating directory structure")
    temp_dir = tempfile.mkdtemp()

    # Example app structure:
    # SUMO-GUI.app
    # └── Contents
    #     └── Info.plist
    #     ├── MacOS
    #     │   └── SUMO-GUI
    #     └── Resources
    #         └── iconsfile.icns

    os.makedirs(os.path.join(temp_dir, f"{app_name}.app", "Contents", "MacOS"))
    os.makedirs(os.path.join(temp_dir, f"{app_name}.app", "Contents", "Resources"))

    print(" - Creating launcher")
    launcher_content = f"""#!/bin/bash
export SUMO_HOME="/Library/Frameworks/{framework_name}.framework/Versions/Current/{framework_name}"
export DYLD_LIBRARY_PATH="$SUMO_HOME/lib:$DYLD_LIBRARY_PATH"
exec "$SUMO_HOME/bin/{binary_name}" "$@"
"""
    launcher_path = os.path.join(temp_dir, f"{app_name}.app", "Contents", "MacOS", app_name)
    with open(launcher_path, "w") as launcher:
        launcher.write(launcher_content)
    os.chmod(launcher_path, 0o755)

    # Copy the icons
    print(" - Copying icons")
    shutil.copy(icns_path, os.path.join(temp_dir, f"{app_name}.app", "Contents", "Resources", "iconfile.icns"))

    # Create plist file
    print(" - Creating plist file")
    plist_file = os.path.join(temp_dir, f"{app_name}.app", "Contents", "Info.plist")
    plist_content = {
        "CFBundleExecutable": app_name,
        "CFBundleIdentifier": f"{id}.app.{binary_name}",
        "CFBundleName": app_name,
        "CFBundleVersion": version,
        "CFBundleShortVersionString": version,
        "CFBundleIconFile": "iconfile.icns",
    }
    with open(plist_file, "wb") as f:
        plistlib.dump(plist_content, f)

    # Call pkg builder
    print(" - Calling pkgbuild")
    cwd = os.path.dirname(os.path.abspath(__file__))
    pkg_name = f"Launcher-{app_name}-{version}.pkg"
    pkg_path = os.path.join(cwd, "..", "..", pkg_name)
    pkg_build_command = [
        "pkgbuild",
        "--root",
        os.path.join(temp_dir, f"{app_name}.app"),
        "--identifier",
        f"{id}.app.{binary_name}",
        "--version",
        version,
        "--install-location",
        f"/Applications/{app_name}.app",
        f"{pkg_path}",
    ]
    subprocess.run(pkg_build_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    pkg_size = os.path.getsize(pkg_path)

    # Cleanup the temporary directory
    print(" - Cleaning up")
    shutil.rmtree(temp_dir)
    return pkg_name, pkg_path, pkg_size


def main():
    opts = parse_args()
    if not os.path.exists(opts.build_dir):
        print(f"Error: build directory '{opts.build_dir}' does not exist.", file=sys.stderr)
        sys.exit(1)
    if not os.path.exists(os.path.join(opts.build_dir, "CMakeCache.txt")):
        print(f"Error: directory '{opts.build_dir}' is not a build directory.", file=sys.stderr)
        sys.exit(1)

    print(f"Building framework package '{opts.name}'")
    f_name, f_path, f_size = create_framework(opts.name, opts.longname, opts.id, opts.version, opts.build_dir)
    print(f"Successfully built: '{f_name}' ({f_size / (1024 * 1024):.2f} MB)\n")

    for app in ["SUMO-GUI"]:
        print(f"Building app package '{app}'")

        cwd = os.path.dirname(os.path.abspath(__file__))
        icon_path = os.path.join(cwd, "..", "..", "build_config", "macos", "sumo-gui", "build", "sumo-gui-icons.icns")
        a_name, a_path, a_size = create_app(app, "sumo-gui", opts.name, opts.id, opts.version, icon_path)
        print(f"Successfully built: '{a_name}' ({a_size / (1024 * 1024):.2f} MB)\n")


if __name__ == "__main__":
    main()
