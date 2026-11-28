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

# @file    tabular.py
# @author  Josh
# @date    2026-11-26

"""Read the columns and rows of flat CSV and Parquet files."""

from contextlib import contextmanager
import csv
from itertools import chain

from ..miscutils import openz


def isTabular(filename):
    lower = filename.lower()
    if lower.endswith(".parquet"):
        # SUMO builds without Parquet support write XML even with a .parquet filename.
        with openz(filename, "rb") as stream:
            return not stream.read(16).startswith(b"<?xml")
    return lower.endswith((".csv", ".csv.gz"))


def _parquetRows(parquet):
    for batch in parquet.iter_batches():
        for row in batch.to_pylist():
            yield {column: None if value is None else str(value) for column, value in row.items()}


@contextmanager
def readTable(filename, separator=None):
    """Yield column names and an iterable of row dictionaries with string or missing values."""
    if filename.lower().endswith(".parquet"):
        from pyarrow.parquet import ParquetFile
        with ParquetFile(filename) as parquet:
            yield parquet.schema_arrow.names, _parquetRows(parquet)
    else:
        with openz(filename) as stream:
            header = stream.readline()
            if separator is None:
                try:
                    separator = csv.Sniffer().sniff(header, delimiters=";,\t|").delimiter
                except csv.Error:
                    separator = ";"
            reader = csv.DictReader(chain([header], stream), delimiter=separator)
            yield reader.fieldnames or [], (
                {column: value if value != "" else None for column, value in row.items()} for row in reader)
