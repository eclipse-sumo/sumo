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

# @file    updateTestsuiteFiles.py
# @author  Pablo Alvarez Lopez
# @date    2025-12-10

"""
"""
import os


def update_testsuite_files():
    # Target filenames to look for
    target_files = [
        "testsuite.netedit.external",
        "testsuite.netedit.internal",
        "testsuite.netedit.output"
    ]

    # Get the directory where the script is located
    base_dir = os.path.dirname(os.path.abspath(__file__))

    print(f"Starting scan in: {base_dir}")

    # Walk through the directory tree
    for root, dirs, files in os.walk(base_dir):
        # Check if any of the target files exist in the current directory
        for filename in target_files:
            if filename in files:
                file_path = os.path.join(root, filename)

                # Get list of subdirectories in the current folder (root)
                # We filter 'dirs' provided by os.walk for the current depth
                # Note: 'dirs' list in os.walk is modifiable in-place to prune search,
                # but here we just read it.

                # Let's ensure we get a clean list of immediate subfolders
                subfolders = [
                    d for d in os.listdir(root)
                    if os.path.isdir(os.path.join(root, d)) and not d.startswith('.')
                ]
                subfolders.sort()

                print(f"Updating {filename} in {root}")

                try:
                    with open(file_path, 'w') as f:
                        for folder in subfolders:
                            f.write(f"{folder}\n")
                except IOError as e:
                    print(f"Error writing to {file_path}: {e}")


if __name__ == "__main__":
    update_testsuite_files()
