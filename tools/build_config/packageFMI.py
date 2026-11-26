#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    packageFMI.py
# @author  Mark Colley
# @date    2026-11-23

"""Package the built SUMO libraries as an FMI 2.0 co-simulation FMU."""

import argparse
from pathlib import Path
from shutil import copyfileobj
from tempfile import TemporaryDirectory
from xml.etree import ElementTree
from zipfile import ZIP_DEFLATED, ZipFile, ZipInfo


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--platform", required=True, help="FMI 2 platform directory (such as win64)")
    parser.add_argument("--model-description", required=True, type=Path)
    parser.add_argument("--fmi-library", required=True, type=Path)
    parser.add_argument("--sumo-library", required=True, type=Path)
    parser.add_argument("runtime_libraries", nargs="*", type=Path)
    options = parser.parse_args()

    model = ElementTree.parse(options.model_description).find("CoSimulation")
    if model is None or not model.get("modelIdentifier"):
        parser.error("the model description must specify a co-simulation modelIdentifier")
    bridgeName = model.get("modelIdentifier") + options.fmi_library.suffix
    libraries = {path.name: path for path in options.runtime_libraries}
    libraries[options.sumo_library.name] = options.sumo_library
    libraries[bridgeName] = options.fmi_library

    files = [("modelDescription.xml", options.model_description, 0o100644)]
    files.extend(("binaries/%s/%s" % (options.platform, name), path, 0o100755)
                 for name, path in sorted(libraries.items()))
    options.output.parent.mkdir(parents=True, exist_ok=True)
    with TemporaryDirectory(dir=options.output.parent) as tmp:
        archivePath = Path(tmp) / options.output.name
        with ZipFile(archivePath, "w") as archive:
            for name, path, mode in files:
                # Use fixed timestamps and archive-relative filenames for repeatable builds.
                entry = ZipInfo(name)
                entry.create_system = 3  # UNIX file modes
                entry.external_attr = mode << 16
                entry.compress_type = ZIP_DEFLATED
                entry.file_size = path.stat().st_size
                with path.open("rb") as source, archive.open(entry, "w") as out:
                    copyfileobj(source, out)
        archivePath.replace(options.output)


if __name__ == "__main__":
    main()
