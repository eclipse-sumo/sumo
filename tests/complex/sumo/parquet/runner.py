#!/usr/bin/env python3
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

# @file    runner.py
# @author  Michael Behrisch
# @date    2026-03-04

"""
Test that SUMO parquet output matches XML output for the main output types.

For each output type the script:
  1. Runs SUMO once writing that output as XML.
  2. Runs SUMO again writing it as parquet.
  3. Converts the XML file to CSV with xml2csv.py.
  4. Converts the parquet file to CSV with parquet2csv.py.
  5. Compares both CSVs numerically.

Parquet may expose columns that the XML format omits (e.g. vehicle_edge in
FCD output).  Such extra parquet columns are reported but not counted as
failures.  Missing columns (XML has data that parquet drops) are failures.

Run with:
    SUMO_HOME=<sumo-root> python3 tests/test_parquet_output.py
"""

import os
import subprocess
import sys

import pandas as pd

SUMO_HOME = os.environ.get(
    "SUMO_HOME",
    os.path.abspath(os.path.join(os.path.dirname(__file__), "..")),
)
SUMO_BIN = os.path.join(SUMO_HOME, "bin", "sumo")
XML2CSV = os.path.join(SUMO_HOME, "tools", "xml", "xml2csv.py")

# (sumo-option, file-stem, end-time)
# end-time of 1000 s ensures vehicles complete their trips for
# tripinfo / vehroute output.
OUTPUTS = [
    ("fcd-output",        "fcd",        120),
    ("emission-output",   "emissions",  120),
    ("tripinfo-output",   "tripinfo",   1000),
    ("summary-output",    "summary",    1000),
    ("vehroute-output",   "vehroutes",  1000),
    ("queue-output",      "queue",      200),
    ("lanechange-output", "lanechanges", 1000),
    ("edgedata-output",   "edgedata", 1000),
    ("lanedata-output",   "lanedata", 1000),
]

# Absolute tolerance for floating-point comparison.
# XML output rounds values to 2 decimal places.
FLOAT_ATOL = 0.0101


def run_sumo(outdir, opt, stem, ext, end_time):
    """Run SUMO for a single output type with the given file extension.

    Returns (success, output_path).
    """
    outfile = os.path.join(outdir, f"{stem}.{ext}")
    cmd = [
        SUMO_BIN,
        "-c", "sumo.sumocfg",
        "-b", "0", "-e", str(end_time),
        f"--{opt}", outfile,
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        last_line = result.stderr.strip().splitlines()[-1] if result.stderr.strip() else "(no stderr)"
        print(f"  SUMO ({ext}) failed (rc={result.returncode}): {last_line}")
        return False, outfile
    return True, outfile


def xml_to_df(xml_path):
    """Convert an XML output file to a DataFrame via xml2csv.py.

    xml2csv writes <stem>.csv next to the input file.
    Returns None if the CSV is empty or missing.
    """
    csv_path = os.path.splitext(xml_path)[0] + ".csv"
    result = subprocess.run(
        [sys.executable, XML2CSV, xml_path],
        capture_output=True, text=True,
    )
    if result.returncode != 0:
        print(f"  xml2csv error: {result.stderr.strip()[:200]}")
        return None
    if not os.path.exists(csv_path) or os.path.getsize(csv_path) == 0:
        return None
    try:
        df = pd.read_csv(csv_path, sep=";", low_memory=False)
    except Exception as e:
        print(f"  Failed to read xml CSV: {e}")
        return None
    return df if not df.empty else None


def compare(df_xml, df_parquet):
    """Compare two DataFrames column by column.

    Returns (ok, notes) where ok is True if all XML columns match and
    notes lists any informational messages (e.g. extra columns in parquet).
    """
    notes = []
    xml_cols = set(df_xml.columns)
    pq_cols = set(df_parquet.columns)

    missing = xml_cols - pq_cols   # data present in XML but absent in parquet
    extra = pq_cols - xml_cols     # extra columns that parquet adds

    if missing:
        return False, [f"parquet is missing XML columns: {sorted(missing)}"]
    if extra:
        notes.append(f"parquet has extra columns not in XML: {sorted(extra)}")

    # Compare only the columns that appear in the XML output.
    cols = sorted(xml_cols)
    df_xml = df_xml[cols].copy()
    df_parquet = df_parquet[cols].copy()

    if len(df_xml) != len(df_parquet):
        return False, notes + [f"row count differs: xml={len(df_xml)}, parquet={len(df_parquet)}"]

    ok = True
    for col in cols:
        s_xml = df_xml[col]
        s_pq = df_parquet[col]
        diff_pairs = None
        try:
            pd.testing.assert_series_equal(
                s_xml.astype(float),
                s_pq.astype(float),
                check_names=False,
                rtol=0,
                atol=FLOAT_ATOL,
            )
        except (ValueError, TypeError):
            try:
                pd.testing.assert_series_equal(
                    s_xml.where(s_xml.notna(), None).astype(str).replace('nan', None),
                    s_pq.astype(str).replace('', None),
                    check_names=False,
                )
            except AssertionError as e:
                notes.append(f"column '{col}' differs: {str(e)[:200]}")
                diff_pairs = zip(list(s_xml), list(s_pq))
        except AssertionError as e:
            notes.append(f"column '{col}' differs: {str(e)[:200]}")
            diff_pairs = zip(list(s_xml), list(s_pq))
        if diff_pairs:
            err = 0
            for i, p in enumerate(diff_pairs):
                if p[0] != p[1]:
                    notes.append(str((i, p)))
                    err += 1
                    if err == 10:
                        break
            ok = False

    return ok, notes


def main():
    tmpdir = "sumo_parquet_test"
    os.makedirs(tmpdir, exist_ok=True)
    errors = 0
    skipped = 0

    for opt, stem, end_time in OUTPUTS:
        print(f"Testing {stem} ({opt}, end={end_time}s)...")

        xml_ok, xml_path = run_sumo(tmpdir, opt, stem, "xml", end_time)
        pq_ok, parquet_path = run_sumo(tmpdir, opt, stem, "parquet", end_time)

        if not xml_ok:
            print(f"[ERROR] {stem}: XML SUMO run failed")
            errors += 1
            continue
        if not pq_ok:
            print(f"[FAIL]  {stem}: parquet SUMO run failed")
            errors += 1
            continue

        xml_exists = os.path.exists(xml_path) and os.path.getsize(xml_path) > 0
        pq_exists = os.path.exists(parquet_path) and os.path.getsize(parquet_path) > 0

        if not xml_exists and not pq_exists:
            print(f"[SKIP]  {stem}: no output generated by either format")
            skipped += 1
            continue
        if not xml_exists:
            print(f"[FAIL]  {stem}: XML output file is empty")
            errors += 1
            continue
        if not pq_exists:
            print(f"[FAIL]  {stem}: parquet output file is empty")
            errors += 1
            continue

        df_xml = xml_to_df(xml_path)
        df_pq = pd.read_parquet(parquet_path)

        if df_xml is None and (df_pq is None or len(df_pq) == 0):
            print(f"[SKIP]  {stem}: both are empty")
            skipped += 1
            continue
        if df_xml is None:
            print(f"[FAIL]  {stem}: XML CSV is empty but parquet is not")
            errors += 1
            continue
        if df_pq is None:
            print(f"[FAIL]  {stem}: parquet CSV is empty but XML is not")
            errors += 1
            continue
        ok, notes = compare(df_xml, df_pq)
        for note in notes:
            print(f"  note: {note}")
        if ok:
            print(f"[OK]    {stem}: {len(df_xml)} rows match")
        else:
            errors += 1
            print(f"[FAIL]  {stem}: outputs differ")
        print()
    if errors:
        print(f"Result: FAILED ({errors} output type(s) differ, {skipped} skipped)")
        sys.exit(1)
    else:
        print(f"Result: PASSED (all outputs match, {skipped} skipped)")


if __name__ == "__main__":
    main()
