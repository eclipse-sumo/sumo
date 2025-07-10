#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2024-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    copyElements.py
# @author  Pablo Alvarez Lopez
# @date    2024-12-09

import os
import shutil

# Function to copy a file if a reference file exists in the folder


def copy_file_if_reference_exists(source_file, base_folder, reference_file):
    # Check if the source file exists
    if not os.path.isfile(source_file):
        print(source_file + " doesn't exist")
        return

    # Traverse all folders and subfolders in the base folder
    for root, dirs, files in os.walk(base_folder):
        for dir_name in dirs:
            destination_folder = os.path.join(root, dir_name)
            reference_file_path = os.path.join(destination_folder, reference_file)

            # Check if the reference file exists in the folder
            if os.path.isfile(reference_file_path):
                try:
                    # Copy the file to the current folder
                    shutil.copy(source_file, destination_folder)
                    print(f"File copied to: {destination_folder}")
                except Exception as e:
                    print("File " + source_file + " cannnot be copied to " + destination_folder)


# Main block
if __name__ == "__main__":

    # Path of the file to be copied
    source_file = r"D:\meandatas.netedit"

    # Path of the base folder where the file will be copied
    base_folder = r"D:\SUMO"

    # Name of the reference file that must exist in the folder
    reference_file = "routes.netedit"

    # Call the function
    copy_file_if_reference_exists(source_file, base_folder, reference_file)
