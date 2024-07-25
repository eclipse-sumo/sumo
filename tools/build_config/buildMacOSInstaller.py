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
# This build directory needs to be provided as parameter.
#
# This script will then:
# 1. Create a pkg file for Eclipse SUMO (= "framework") (and also add dependent libraries)
# 2. Create pkg files for all launchers (= "apps")
# 3. Create an installer pkg to jointly install the framework pkg and all app pkgs
# 4. Create a disk image (dmg) to store the pkg files for deployment

import os
import plistlib
import shutil
import subprocess
import sys
import tempfile

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from sumolib.options import ArgumentParser  # noqa
from sumolib.version import gitDescribe  # noqa

# Try to import dmgbuild and exit with an error message if it fails
try:
    from dmgbuild.core import build_dmg
except ImportError:
    print("Error: dmgbuild module is not installed. Please install it using 'pip install dmgbuild'.")
    sys.exit(1)


def parse_args():
    default_dmg_name = f"Eclipse SUMO {gitDescribe()}.dmg"
    default_output_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", default_dmg_name))
    op = ArgumentParser(description="Build an installer for macOS (dmg file)")
    op.add_argument("build_dir", help="Build dir of sumo")
    op.add_argument("-o", "--output", dest="output", help="Output path for the dmg file", default=default_output_path)

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


def create_framework(name, longname, pkg_id, version, sumo_build_directory):
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
        "CFBundleIdentifier": pkg_id,
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
    subprocess.run(cmake_install_command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

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
        pkg_id,
        "--version",
        version,
        "--install-location",
        f"/Library/Frameworks/{name}.framework",
        f"{pkg_path}",
    ]
    print(" - Calling pkgbuild")
    subprocess.run(pkg_build_command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    pkg_size = os.path.getsize(pkg_path)

    # Cleanup the temporary directory
    print(" - Cleaning up")
    shutil.rmtree(temp_dir)

    return name, pkg_name, pkg_id, pkg_path, pkg_size


def create_app(app_name, binary_name, framework_name, pkg_id, version, icns_path):
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
        "CFBundleIdentifier": pkg_id,
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
        pkg_id,
        "--version",
        version,
        "--install-location",
        f"/Applications/{app_name}.app",
        f"{pkg_path}",
    ]
    subprocess.run(pkg_build_command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    pkg_size = os.path.getsize(pkg_path)

    # Cleanup the temporary directory
    print(" - Cleaning up")
    shutil.rmtree(temp_dir)
    return app_name, pkg_name, pkg_id, pkg_path, pkg_size


def create_installer(frmwk_pkg, app_pkgs, id, version):
    cwd = os.path.dirname(os.path.abspath(__file__))
    print(" - Creating temporary directory")
    temp_dir = tempfile.mkdtemp()
    resources_dir = os.path.join(temp_dir, "Resources")
    os.makedirs(resources_dir)

    # Copy the framework package
    framework_pkg_path = frmwk_pkg[3]
    shutil.copy(framework_pkg_path, temp_dir)

    # Copy the app packages
    for app_pkg in app_pkgs:
        app_pkg_path = app_pkg[3]
        shutil.copy(app_pkg_path, temp_dir)

    # FIXME: Add license, background and other nice stuff
    print(" - Adding additional resources to the installer")
    sumo_dir = os.path.join(cwd, "..", "..", "")
    sumo_data_installer_dir = os.path.join(sumo_dir, "build_config", "macos", "installer")
    installer_resources_dir = os.path.join(temp_dir, "Resources")
    shutil.copy(os.path.join(sumo_data_installer_dir, "background.png"), installer_resources_dir)
    shutil.copy(os.path.join(sumo_dir, "LICENSE"), os.path.join(installer_resources_dir, "LICENSE.txt"))

    # Create distribution.xml
    print(" - Creating distribution.xml")

    size = frmwk_pkg[4] // 1024
    path = os.path.basename(frmwk_pkg[3])
    refs = f"        <pkg-ref id='{frmwk_pkg[2]}' version='{version}' installKBytes='{size}'>{path}</pkg-ref>"

    for _, app_pkg in enumerate(app_pkgs):
        size = app_pkg[4] // 1024
        path = os.path.basename(app_pkg[3])
        refs += f"\n        <pkg-ref id='{app_pkg[2]}' version='{version}' installKBytes='{size}'>{path}</pkg-ref>"

    # See: https://developer.apple.com/library/archive/documentation/
    #      DeveloperTools/Reference/DistributionDefinitionRef/Chapters/Distribution_XML_Ref.html
    distribution_content = f"""<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="2">
    <title>Eclipse SUMO</title>
    <!-- <allowed-os-versions><os-version min="10.14"/></allowed-os-versions> -->
    <license file="LICENSE.txt"/>
    <background file="background.png" alignment="bottomleft" mime-type="image/png" scaling="none" />
    <!-- <welcome file="Welcome.html" mime-type="text/html"/> -->
    <!-- <conclusion file="Conclusion.html" mime-type="text/html"/> -->
    <options customize="allow" require-scripts="false" rootVolumeOnly="true" hostArchitectures="arm64"/>
    <choices-outline>
        <line choice="default"/>
    </choices-outline>
    <choice id="default" title="Eclipse SUMO">
{refs}
    </choice>
</installer-gui-script>
"""
    distribution_path = os.path.join(temp_dir, "distribution.xml")
    with open(distribution_path, "w") as f:
        f.write(distribution_content)

    # Call productbuild
    print(" - Calling productbuild")

    cwd = os.path.dirname(os.path.abspath(__file__))
    pkg_name = f"Installer-{version}.pkg"
    pkg_path = os.path.join(cwd, "..", "..", pkg_name)
    productbuild_command = [
        "productbuild",
        "--distribution",
        distribution_path,
        "--package-path",
        temp_dir,
        "--resources",
        resources_dir,
        pkg_path,
    ]
    subprocess.run(productbuild_command, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    pkg_size = os.path.getsize(pkg_path)

    print(" - Cleaning up")
    shutil.rmtree(temp_dir)

    pkg_size = os.path.getsize(pkg_path)
    return "Installer", pkg_name, id, pkg_path, pkg_size


def create_dmg(dmg_title, dmg_output_path, installer_pkg_path):

    print(" - Preparing disk image folder")
    dmg_prep_folder = tempfile.mkdtemp()
    shutil.copy(installer_pkg_path, dmg_prep_folder)

    # FIXME: Add uninstaller script to the installer folder

    if os.path.exists(dmg_output_path):
        print(" - Removing already existing disk image before creating a new disk image")
        os.remove(dmg_output_path)

    print(" - Collecting files and calculating file size")
    files_to_store = []
    total_size = 0
    for root, _, files in os.walk(dmg_prep_folder):
        for file in files:
            files_to_store.append((os.path.join(root, file), file))
            total_size += os.path.getsize(os.path.join(root, file))

    print(" - Building diskimage")
    settings = {
        "volume_name": "Eclipse SUMO",
        "size": f"{total_size // 1024 * 1.2}K",
        "files": files_to_store,
        # FIXME: add background and badge
    }
    build_dmg(dmg_output_path, dmg_title, settings=settings)

    print(" - Cleaning up")
    shutil.rmtree(dmg_prep_folder)


def main():
    cwd = os.path.dirname(os.path.abspath(__file__))
    version = gitDescribe()
    base_id = "org.eclipse.sumo"

    # Parse and check the command line arguments
    opts = parse_args()
    if not os.path.exists(opts.build_dir):
        print(f"Error: build directory '{opts.build_dir}' does not exist.", file=sys.stderr)
        sys.exit(1)
    if not os.path.exists(os.path.join(opts.build_dir, "CMakeCache.txt")):
        print(f"Error: directory '{opts.build_dir}' is not a build directory.", file=sys.stderr)
        sys.exit(1)
    if not os.path.exists(os.path.dirname(opts.output)):
        print(f"Error: output directory '{os.path.dirname(opts.output)}' does not exist.", file=sys.stderr)
        sys.exit(1)

    # Building the framework package
    print("Building framework package 'EclipseSUMO'")
    framework_pkg = create_framework("EclipseSUMO", "Eclipse SUMO", f"{base_id}.framework", version, opts.build_dir)
    print(f"Successfully built: '{framework_pkg[1]}' ({framework_pkg[4] / (1024 * 1024):.2f} MB)\n")

    # Building all the app launchers packages
    app_pkgs = []
    app_list = [
        ("Sumo-GUI", "sumo-gui", framework_pkg[0], f"{base_id}.apps.sumo-gui", version, "sumo-gui-icons.icns"),
        ("NetEdit", "netedit", framework_pkg[0], f"{base_id}.apps.netedit", version, "netedit-icons.icns"),
    ]
    for app_name, app_binary, app_framework, app_id, app_ver, app_icons in app_list:
        print(f"Building app package for '{app_name}'")

        icon_path = os.path.join(cwd, "..", "..", "build_config", "macos", "installer", app_icons)
        app_pkg = create_app(app_name, app_binary, app_framework, app_id, app_ver, icon_path)
        app_pkgs.append(app_pkg)
        print(f"Successfully built: '{app_pkg[1]}' ({app_pkg[4] / (1024 * 1024):.2f} MB)\n")

    # Building the installer package
    print("Building installer")
    installer_pkg = create_installer(framework_pkg, app_pkgs, f"{base_id}.installer", version)
    print("Successfully built installer\n")

    print("Building disk image")
    create_dmg("Eclipse SUMO", opts.output, installer_pkg[3])
    print("Successfully built disk image")

    # Removing final pkg-files
    # os.remove(installer_pkg[3])
    # os.remove(framework_pkg[3])
    # for app_pkg in app_pkgs:
    #    os.remove(app_pkg[3])

    print(f"\nDisk image is available: '{opts.output}' (size: {os.path.getsize(opts.output) / (1024 * 1024):.2f} MB)")


if __name__ == "__main__":
    main()
