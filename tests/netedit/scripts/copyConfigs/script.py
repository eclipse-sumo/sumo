#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2025-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    script.py
# @author  Pablo Alvarez Lopez
# @date    2025-12-05

import os
import shutil
import sys


def copy_file_to_matching_folders(root_path, target_folder_name, source_file_path):
    """
    Traverses root_path, looks for folders matching target_folder_name,
    and copies source_file_path into them.
    """
    # 1. Validate inputs
    if not os.path.isdir(root_path):
        print(f"Error: The root path '{root_path}' is not a valid directory.")
        sys.exit(1)

    if not os.path.isfile(source_file_path):
        print(f"Error: The file '{source_file_path}' does not exist.")
        sys.exit(1)

    print(f"Scanning '{root_path}' for folders named '{target_folder_name}'...")

    copied_count = 0

    # 2. Walk through the directory tree
    for current_root, dirs, files in os.walk(root_path):
        # Check if the target folder exists in the current directory's subdirectories
        if target_folder_name in dirs:
            destination_folder = os.path.join(current_root, target_folder_name)

            try:
                # 3. Copy the file
                shutil.copy2(source_file_path, destination_folder)
                print(f"✅ Copied file to: {destination_folder}")
                copied_count += 1
            except Exception as e:
                print(f"❌ Failed to copy to {destination_folder}: {e}")

    print(f"\nDone! File copied to {copied_count} location(s).")


def main():

    # Run the function
    copy_file_to_matching_folders("D:/Netedit_dev/tests/netedit/elements/demands", "volatile_recomputing",
                                  "D:/Netedit_dev/tests/netedit/scripts/copyConfigs/netedit_A.netecfg")


if __name__ == "__main__":
    main()
