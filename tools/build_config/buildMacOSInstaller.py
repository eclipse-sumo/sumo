#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
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

import os
import plistlib
import re
import shutil
import subprocess
import sys
import tempfile
from glob import iglob

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from sumolib.options import ArgumentParser  # noqa

from build_config.version import get_pep440_version  # noqa

try:
    from delocate.cmd.delocate_path import delocate_path
except ImportError:
    print("Error: delocate module is not installed. Please install it using 'pip install delocate'.")
    sys.exit(1)

try:
    from dmgbuild.core import build_dmg
except ImportError:
    print("Error: dmgbuild module is not installed. Please install it using 'pip install dmgbuild'.")
    sys.exit(1)


def transform_pep440_version(version):
    post_pattern = re.compile(r"^(.*)\.post\d+$")
    match = post_pattern.match(version)
    if match:
        return f"{match.group(1)}-git"
    else:
        return version


def parse_args(def_dmg_name, def_pkg_name):
    def_build_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "sumo-build"))
    def_output_fw_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "framework"))
    def_output_apps_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "apps"))
    def_output_fw_pkg_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "framework-pkg"))
    def_output_apps_pkg_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "apps-pkg"))
    def_output_pkg_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..",
                                                       "installer", def_pkg_name))
    def_output_dmg_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", def_dmg_name))

    op = ArgumentParser(description="Build an installer for macOS (dmg file)")

    # We can set one these actions exclusively
    action_group = op.add_mutually_exclusive_group("Actions")
    action_group.add_argument("--create-framework-dir", dest="create_framework_dir", action="store_true")
    action_group.add_argument("--create-framework-pkg", dest="create_framework_pkg", action="store_true")
    action_group.add_argument("--create-apps-dir", dest="create_apps_dir", action="store_true")
    action_group.add_argument("--create-apps-pkg", dest="create_apps_pkg", action="store_true")
    action_group.add_argument("--create-installer-pkg", dest="create_installer_pkg", action="store_true")
    action_group.add_argument("--create-installer-dmg", dest="create_installer_dmg", action="store_true")

    # ... and supply some arguments
    op.add_argument("--build-dir", dest="build_dir", default=def_build_dir)
    op.add_argument("--framework-dir", dest="framework_dir", default=def_output_fw_dir)
    op.add_argument("--framework-pkg-dir", dest="framework_pkg_dir", default=def_output_fw_pkg_dir)
    op.add_argument("--apps-dir", dest="apps_dir", default=def_output_apps_dir)
    op.add_argument("--apps-pkg-dir", dest="apps_pkg_dir", default=def_output_apps_pkg_dir)
    op.add_argument("--installer-pkg-file", dest="installer_pkg_file", default=def_output_pkg_path)
    op.add_argument("--installer-dmg-file", dest="installer_dmg_file", default=def_output_dmg_path)

    args = op.parse_args()

    # Validate the basic argument logic
    if args.build_dir is not None and args.create_framework_dir is None:
        print("Error: build directory can only be set when creating the framework directory.", file=sys.stderr)
        sys.exit(1)

    if args.framework_pkg_dir is not None and args.create_framework_pkg is None:
        print("Error: framework pkg directory can only be set when creating the framework pkg.", file=sys.stderr)
        sys.exit(1)

    if args.apps_dir is not None and args.create_apps_dir is None:
        print("Error: apps directory can only be set when creating the apps.", file=sys.stderr)
        sys.exit(1)

    return args


def create_framework_dir(name, longname, pkg_id, version, sumo_build_directory, framework_output_dir):
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

    print(" - Creating directory structure")
    os.makedirs(framework_output_dir, exist_ok=False)

    framework_dir = os.path.join(framework_output_dir, f"{name}.framework")
    version_dir = os.path.join(framework_dir, f"Versions/{version}")
    os.makedirs(os.path.join(version_dir, name), exist_ok=True)
    os.makedirs(os.path.join(version_dir, "Resources"), exist_ok=True)

    os.symlink(f"{version}/", os.path.join(framework_dir, "Versions/Current"), True)
    os.symlink(f"Versions/Current/{name}/", os.path.join(framework_dir, name), True)
    os.symlink("Versions/Current/Resources/", os.path.join(framework_dir, "Resources"), True)

    # Create the Info.plist file
    plist_file = os.path.join(version_dir, "Resources", "Info.plist")
    print(" - Creating properties list")
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
    print(" - Installing Eclipse SUMO build")
    cmake_install_command = [
        "cmake",
        "--install",
        sumo_build_directory,
        "--prefix",
        os.path.join(version_dir, name),
    ]
    subprocess.run(cmake_install_command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    # Remove unneeded libsumo and libtraci folders from the tools directory
    # (If the user needs libsumo or libtraci, they should install these tools with pip)
    print(" - Removing libsumo and libtraci folders from tools")
    shutil.rmtree(os.path.join(version_dir, name, "share", "sumo", "tools", "libsumo"), ignore_errors=True)
    shutil.rmtree(os.path.join(version_dir, name, "share", "sumo", "tools", "libtraci"), ignore_errors=True)

    # We need to add a symlink to the binary folder to have the same folder structure
    os.symlink("../../bin", os.path.join(version_dir, name, "share", "sumo", "bin"))

    # Determine library dependencies
    print(" - Delocating binaries and libraries")
    os.chdir(os.path.join(version_dir, name))

    # - libraries that landed in the lib folder need to be delocated as well
    lib_dir = os.path.join(version_dir, name, "lib")
    bin_dir = os.path.join(version_dir, name, "bin")
    for pattern in ("*.jnilib", "*.dylib"):
        for file in iglob(os.path.join(lib_dir, pattern)):
            file_name = os.path.basename(file)
            shutil.move(os.path.join(lib_dir, file_name), os.path.join(bin_dir, file_name))

    # Start the delocation of the libraries and binaries
    delocate_path("./bin", lib_filt_func=None, lib_path="./lib", sanitize_rpaths=True)

    # - and we need to move them back to the lib folder
    for pattern in ("*.jnilib", "*.dylib"):
        for file in iglob(os.path.join(bin_dir, pattern)):
            file_name = os.path.basename(file)
            shutil.move(os.path.join(bin_dir, file_name), os.path.join(lib_dir, file_name))

    # Add proj db files from /opt/homebrew/Cellar/proj/<X.Y.Z>/share/proj
    print(" - Copying additional files (e.g. proj.db)")
    proj_dir = "/opt/homebrew/Cellar/proj"
    proj_file_list = ["GL27", "ITRF2000", "ITRF2008", "ITRF2014", "nad.lst", "nad27", "nad83", "other.extra", "proj.db",
                      "proj.ini", "projjson.schema.json", "triangulation.schema.json", "world", "CH", "deformation_model.schema.json"]  # noqa
    dest_dir = os.path.join(version_dir, name, "share", "proj")
    if os.path.exists(proj_dir):
        first_dir = next(iter(os.listdir(proj_dir)), None)
        if first_dir:
            source_dir = os.path.join(proj_dir, first_dir, "share/proj")
            if os.path.exists(source_dir):
                os.makedirs(dest_dir, exist_ok=True)
                for file in proj_file_list:
                    shutil.copy2(os.path.join(source_dir, file), os.path.join(dest_dir, file))


def create_framework_pkg(name, pkg_id, version, framework_dir, framework_pkg_dir):
    # Build the framework package
    os.makedirs(framework_pkg_dir, exist_ok=False)
    pkg_name = f"{name}-{version}.pkg"
    pkg_path = os.path.join(framework_pkg_dir, pkg_name)
    pkg_build_command = [
        "pkgbuild",
        "--root",
        os.path.join(framework_dir, f"{name}.framework"),
        "--identifier",
        pkg_id,
        "--version",
        version,
        "--install-location",
        f"/Library/Frameworks/{name}.framework",
        f"{pkg_path}",
    ]
    print(f" - Using the call {pkg_build_command}")
    print(f" - Calling pkgbuild to create \"{pkg_path}\"")
    subprocess.run(pkg_build_command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    pkg_size = os.path.getsize(pkg_path)
    return name, pkg_name, pkg_id, pkg_path, pkg_size


def create_app_dir(app_name, exec_call, framework_name, pkg_id, version, icns_path, app_output_dir):
    # Example app structure:
    # SUMO-GUI.app
    # └── Contents
    #     └── Info.plist
    #     ├── MacOS
    #     │   └── SUMO-GUI
    #     └── Resources
    #         └── iconsfile.icns

    print("   . Creating directory structure")
    os.makedirs(os.path.join(app_output_dir, f"{app_name}.app", "Contents", "MacOS"))
    os.makedirs(os.path.join(app_output_dir, f"{app_name}.app", "Contents", "Resources"))

    print("   . Creating launcher")
    launcher_content = f"""#!/bin/bash
export SUMO_HOME="/Library/Frameworks/{framework_name}.framework/Versions/Current/{framework_name}/share/sumo"
{exec_call}
"""
    launcher_path = os.path.join(app_output_dir, f"{app_name}.app", "Contents", "MacOS", app_name)
    with open(launcher_path, "w") as launcher:
        launcher.write(launcher_content)
    os.chmod(launcher_path, 0o755)

    # Copy the icons
    print("   . Copying icons")
    shutil.copy(icns_path, os.path.join(app_output_dir, f"{app_name}.app", "Contents", "Resources", "iconfile.icns"))

    # Create plist file
    print("   . Creating properties file")
    plist_file = os.path.join(app_output_dir, f"{app_name}.app", "Contents", "Info.plist")
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


def create_app_pkg(app_name, pkg_id, version, app_dir, apps_pkg_dir):
    pkg_name = f"Launcher-{app_name}-{version}.pkg"
    pkg_path = os.path.join(apps_pkg_dir, pkg_name)
    pkg_build_command = [
        "pkgbuild",
        "--root",
        os.path.join(app_dir, f"{app_name}.app"),
        "--identifier",
        pkg_id,
        "--version",
        version,
        "--install-location",
        f"/Applications/{app_name}.app",
        f"{pkg_path}",
    ]
    subprocess.run(pkg_build_command, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    pkg_size = os.path.getsize(pkg_path)
    return app_name, pkg_name, pkg_id, pkg_path, pkg_size


def create_installer(framework_pkg, apps_pkg, version, installer_pkg_file):
    """"Creates the installer package

        framework_pkg: framework info [framework_path, framework_id]
        apps_pkg: apps info [[app1_path, app1_id], [app2_path, app2_id], ...]
        id: id of the pkg-file for the installer
        version: 1.20.0
        installer_pkg_file: name of the output pkg file
    """

    # Create a temporary directory to assemble everything for the installer
    temp_dir = tempfile.mkdtemp()

    # Copy the framework pkg file and the launcher apps pkg files
    shutil.copy(framework_pkg[0], temp_dir)
    for app_pkg in apps_pkg:
        shutil.copy(app_pkg[0], temp_dir)

    # Add license, background and other nice stuff
    resources_dir = os.path.join(temp_dir, "Resources")
    os.makedirs(resources_dir)
    sumo_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")
    sumo_data_installer_dir = os.path.join(sumo_dir, "build_config", "macos", "installer")
    shutil.copy(os.path.join(sumo_data_installer_dir, "background.png"), resources_dir)
    shutil.copy(os.path.join(sumo_dir, "LICENSE"), os.path.join(resources_dir, "LICENSE.txt"))

    # Create conclusion.html in the installer resources folder
    with open(os.path.join(resources_dir, "conclusion.html"), "w") as file:
        file.write("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <style>
        body {
            font-family: Helvetica;
            font-size: 14px;
        }
    </style>
</head>
<body>
    <div>
        <h4>Important:</h4>
        <ul>
            <li>
                For applications with a graphical user interface to function properly, please ensure
                you have <b>XQuartz</b> installed.
                It can be obtained from: <a href="https://www.xquartz.org" target="_blank">XQuartz</a>.
            </li>
            <li>
                You may need to install Python 3, if it is not installed yet. Python is required for the
                Scenario Wizard and other tools.
            </li>
            <li>
                If you intend to use SUMO from the command line, please remember to set
                the <b>SUMO_HOME</b> environment variable and add it to the <b>PATH</b> variable.
                <br>
                For more details, visit the
                <a href="https://sumo.dlr.de/docs/Installing/index.html#macos" target="_blank">
                    SUMO macOS installation guide
                </a>.
            </li>
        </ul>
        </p>
        <p>For support options, including the "sumo-user" mailing list, please visit:
           <a href="https://eclipse.dev/sumo/contact/" target="_blank">SUMO Contact</a>.
        </p>
    </div>
</body>
</html>
""")

    # Create distribution.xml
    print(" - Creating distribution.xml")
    size = os.path.getsize(framework_pkg[0]) // 1024
    path = os.path.basename(framework_pkg[0])
    refs = f"        <pkg-ref id='{framework_pkg[1]}' version='{version}' installKBytes='{size}'>{path}</pkg-ref>"

    for app_pkg in apps_pkg:
        size = os.path.getsize(app_pkg[0]) // 1024
        path = os.path.basename(app_pkg[0])
        refs += f"\n        <pkg-ref id='{app_pkg[1]}' version='{version}' installKBytes='{size}'>{path}</pkg-ref>"

    # See: https://developer.apple.com/library/archive/documentation/
    #      DeveloperTools/Reference/DistributionDefinitionRef/Chapters/Distribution_XML_Ref.html
    distribution_content = f"""<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="2">
    <title>Eclipse SUMO</title>
    <allowed-os-versions><os-version min="10.14"/></allowed-os-versions>
    <license file="LICENSE.txt"/>
    <background file="background.png" alignment="bottomleft" mime-type="image/png" scaling="none" />
    <conclusion file="conclusion.html" mime-type="text/html"/>
    <options customize="allow" require-scripts="false" rootVolumeOnly="true" hostArchitectures="arm64"/>
    <choices-outline>
        <line choice="default"/>
    </choices-outline>
    <choice id="default" title="Eclipse SUMO {version}">
{refs}
    </choice>
</installer-gui-script>
"""
    distribution_path = os.path.join(temp_dir, "distribution.xml")
    with open(distribution_path, "w") as f:
        f.write(distribution_content)

    # Call productbuild
    print(" - Calling productbuild")
    productbuild_command = [
        "productbuild",
        "--distribution",
        distribution_path,
        "--package-path",
        temp_dir,
        "--resources",
        resources_dir,
        installer_pkg_file,
    ]
    subprocess.run(productbuild_command, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    # Removing temporary build directory
    print(" - Cleaning up")
    shutil.rmtree(temp_dir)


def create_dmg(dmg_title, version, installer_pkg_path, installer_dmg_path):

    if os.path.exists(installer_dmg_path):
        print(" - Removing existing disk image before creating a new disk image")
        os.remove(installer_dmg_path)

    print(" - Preparing disk image folder")
    dmg_prep_folder = tempfile.mkdtemp()

    # Copy the installer pkg
    shutil.copy(installer_pkg_path, dmg_prep_folder)

    # Add the uninstall script
    uninstall_script_path = os.path.join(dmg_prep_folder, "uninstall.command")
    with open(uninstall_script_path, "w") as f:
        f.write("""#!/bin/bash
echo "This will uninstall Eclipse SUMO and its components."
read -p "Are you sure? (y/N): " CONFIRM
if [[ ! "$CONFIRM" =~ ^[Yy]$ ]]; then
    echo "Uninstallation aborted."
    exit 0
fi

# Request sudo privileges
osascript -e 'do shell script "sudo -v" with administrator privileges'

# Define installed paths
FRAMEWORK="/Library/Frameworks/EclipseSUMO.framework"
APP1="/Applications/SUMO sumo-gui.app"
APP2="/Applications/SUMO netedit.app"
APP3="/Applications/SUMO Scenario Wizard.app"

# Remove framework
if [ -d "$FRAMEWORK" ]; then
    echo "Removing framework: $FRAMEWORK"
    sudo rm -rf "$FRAMEWORK"
else
    echo "Framework not found: $FRAMEWORK"
fi

# Remove apps
for APP in "$APP1" "$APP2" "$APP3"; do
    if [ -d "$APP" ]; then
        echo "Removing application: $APP"
        sudo rm -rf "$APP"
    else
        echo "Application not found: $APP"
    fi
done

echo "Eclipse SUMO has been successfully uninstalled!"
exit 0
""")
    # Make the script executable
    os.chmod(uninstall_script_path, 0o755)

    # Collect all files and add to the dmg
    print(" - Collecting files and calculating file size")
    files_to_store = []
    total_size = 0
    for root, _, files in os.walk(dmg_prep_folder):
        for file in files:
            files_to_store.append((os.path.join(root, file), file))
            total_size += os.path.getsize(os.path.join(root, file))

    print(" - Building diskimage")
    settings = {
        "volume_name": f"Eclipse SUMO {version}",
        "size": f"{total_size // 1024 * 1.2}K",
        "files": files_to_store,
        # FIXME: add background and badge
    }
    build_dmg(installer_dmg_path, dmg_title, settings=settings)

    print(" - Cleaning up")
    shutil.rmtree(dmg_prep_folder)


def main():
    base_id = "org.eclipse.sumo"
    default_framework_name = "EclipseSUMO"
    default_framework_long_name = "Eclipse SUMO"
    version = transform_pep440_version(get_pep440_version())
    default_pkg_name = f"sumo-{version}.pkg"
    default_dmg_name = f"sumo-{version}.dmg"

    # Which launcher apps do we have?
    app_list = [
        (
            "SUMO sumo-gui",
            'exec "$SUMO_HOME/bin/sumo-gui" "$@" &',
            default_framework_name,
            f"{base_id}.apps.sumo-gui",
            version,
            "sumo-gui.icns",
            "sumo-gui"
        ),
        (
            "SUMO netedit",
            'exec "$SUMO_HOME/bin/netedit" "$@" &',
            default_framework_name,
            f"{base_id}.apps.netedit",
            version,
            "netedit.icns",
            "netedit"
        ),
        (
            "SUMO Scenario Wizard",
            (
                "python  $SUMO_HOME/tools/osmWebWizard.py ||"
                "python3 $SUMO_HOME/tools/osmWebWizard.py  &"
            ),
            default_framework_name,
            f"{base_id}.apps.scenario-wizard",
            version,
            "scenario-wizard.icns",
            "scenario-wizard"
        ),
    ]

    # Parse and check the command line arguments
    opts = parse_args(default_dmg_name, default_pkg_name)

    # Let's see what we need to do
    if opts.create_framework_dir:
        if os.path.exists(opts.framework_dir):
            print(f"Directory {opts.framework_dir} already exists. Aborting.")
            sys.exit(1)
        if not os.path.exists(opts.build_dir):
            print(f"Error: build directory '{opts.build_dir}' does not exist.", file=sys.stderr)
            sys.exit(1)
        if not os.path.exists(os.path.join(opts.build_dir, "CMakeCache.txt")):
            print(f"Error: directory '{opts.build_dir}' is not a build directory.", file=sys.stderr)
            sys.exit(1)

        print(f"Creating {default_framework_name} framework directory: \"{opts.framework_dir}\"")
        create_framework_dir(default_framework_name, default_framework_long_name, f"{base_id}.framework", version,
                             opts.build_dir, opts.framework_dir)
        print(f"Successfully created {default_framework_name} framework directory")

    elif opts.create_framework_pkg:
        if os.path.exists(opts.framework_pkg_dir):
            print(f"Directory {opts.framework_pkg_dir} already exists. Aborting.")
            sys.exit(1)
        if not os.path.exists(opts.framework_dir):
            print(f"Error: framework directory '{opts.framework_dir}' does not exist.", file=sys.stderr)
            sys.exit(1)

        print(f"Creating {default_framework_name} framework *.pkg file")
        print(f" - Using framework directory: \"{opts.framework_dir}\"")
        _, pkg_name, _, _, pkg_size = create_framework_pkg(default_framework_name, f"{base_id}.framework", version,
                                                           opts.framework_dir, opts.framework_pkg_dir)
        print(f"Successfully created \"{pkg_name}\" ({pkg_size / (1024 * 1024):.2f} MB)")

    elif opts.create_apps_dir:
        if os.path.exists(opts.apps_dir):
            print(f"Directory {opts.apps_dir} already exists. Aborting.")
            sys.exit(1)

        print(f"Creating {default_framework_name} launcher apps directories")
        os.makedirs(opts.apps_dir, exist_ok=False)
        for app_name, app_binary, app_framework, app_id, app_ver, app_icons, app_folder in app_list:
            app_dir = os.path.join(opts.apps_dir, app_folder)
            print(f" - Building app directory for '{app_name}' in folder {app_dir}")
            os.makedirs(app_dir)
            icon_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..",
                                     "build_config", "macos", "installer", app_icons)
            create_app_dir(app_name, app_binary, app_framework, app_id, app_ver, icon_path, app_dir)
            print(f" - Successfully created app directory for '{app_name}'")

    elif opts.create_apps_pkg:
        if os.path.exists(opts.apps_pkg_dir):
            print(f"Directory {opts.apps_pkg_dir} already exists. Aborting.")
            sys.exit(1)

        print(f"Creating {default_framework_name} launcher app pkg files")
        os.makedirs(opts.apps_pkg_dir, exist_ok=False)

        for app_name, app_binary, app_framework, app_id, app_ver, app_icons, app_folder in app_list:
            app_dir = os.path.join(opts.apps_dir, app_folder)
            _, pkg_name, _, _, pkg_size = create_app_pkg(app_name, app_id, app_ver, app_dir, opts.apps_pkg_dir)
            print(f" - Created \"{pkg_name}\" ({pkg_size / (1024 * 1024):.2f} MB)")

    elif opts.create_installer_pkg:
        if os.path.exists(os.path.dirname(opts.installer_pkg_file)):
            print(f"Error: pkg output directory '{os.path.dirname(opts.installer_pkg_file)}' exists.",
                  file=sys.stderr)
            sys.exit(1)

        # Create the output directory for the installer pkg
        os.makedirs(os.path.dirname(opts.installer_pkg_file))

        print("Building installer pkg file")
        # Where do we find our pkgs?
        fw_pkg = [os.path.join(opts.framework_pkg_dir, f"{default_framework_name}-{version}.pkg"),
                  f"{base_id}.framework"]
        app_pkgs = []
        for app_name, app_binary, app_framework, app_id, app_ver, app_icons, app_folder in app_list:
            app_pkgs.append([os.path.join(opts.apps_pkg_dir, f"Launcher-{app_name}-{version}.pkg"), app_id])

        # Build the installer pkg file
        create_installer(fw_pkg, app_pkgs, version, opts.installer_pkg_file)
        pkg_size = os.path.getsize(opts.installer_pkg_file)

        print(f"Installer pkg file created: \"{opts.installer_pkg_file}\" ({pkg_size / (1024 * 1024):.2f} MB)")

    elif opts.create_installer_dmg:
        if not os.path.exists(os.path.dirname(opts.installer_dmg_file)):
            print(f"Error: output directory '{os.path.dirname(opts.installer_dmg_file)}' does not exist.",
                  file=sys.stderr)
            sys.exit(1)

        if not os.path.exists(opts.installer_pkg_file):
            print(f"Error: installer pkg file '{opts.installer_pkg_file}' does not exist.",
                  file=sys.stderr)
            sys.exit(1)

        print("Building installer disk image (dmg file)")
        create_dmg(default_framework_long_name, version, opts.installer_pkg_file, opts.installer_dmg_file)
        pkg_size = os.path.getsize(opts.installer_dmg_file)
        print(f"Successfully built disk image: \"{opts.installer_dmg_file}\" ({pkg_size / (1024 * 1024):.2f} MB)")


if __name__ == "__main__":
    main()
