#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2013-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    DetectorsOutToTable.py
# @author  Davide Guastella
# @date    2026-04-24

"""
    Convert a detector output file (generated from induction loops) to a table where rows are the time instants,
    columns are the pivot value provided in input.


    * Example
        python DetectorsOutToTable.py -a det.add.xml -d det.out.xml -p entered -o det_table.csv

    * If you want the time stamps to be in format YYYY-MM-DD:
        python DetectorsOutToTable.py -a det.add.xml -d det.out.xml -p entered -o det_table.csv -t 2024-04-02

    * If you want to aggregate counts from lanes (that is, you want edge-wise values)
        python DetectorsOutToTable.py -a det.add.xml -d det.out.xml -p entered -o det_table.csv --aggregate_lanes

"""

import os
from pathlib import Path
import pandas as pd
import sumolib
import argparse
import sys
import polars as pl
from datetime import datetime, timedelta

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools', 'xml'))
    import xml2csv
else:
    sys.exit("Please declare environment variable 'SUMO_HOME'")


def det_out_to_table(fname, edges_names, pivot_attribute="interval_nVehContrib", aggregate_lanes=False):
    sim_data_pl = pl.from_pandas(pd.read_csv(fname, sep=";"))

    pivot_lower = pivot_attribute.lower()

    matches = [
        col for col in sim_data_pl.columns
        if pivot_lower in col.lower()
    ]

    if len(matches) == 0:
        raise ValueError(
            f"No column contains '{pivot_attribute}' (case-insensitive). "
            f"Available columns: {sim_data_pl.columns}"
        )

    if len(matches) > 1:
        raise ValueError(
            f"Ambiguous pivot_attribute '{pivot_attribute}'. "
            f"Matched multiple columns: {matches}"
        )

    resolved_col = matches[0]

    sim_data_df_formatted = pd.DataFrame()
    for real_edge_name in edges_names:
        aggr_counts = sim_data_pl.filter(pl.col('interval_id').str.contains(real_edge_name)).group_by(
            ["interval_begin"]).agg(pl.col(resolved_col).sum()).to_pandas(use_pyarrow_extension_array=False)
        aggr_counts['interval_id'] = real_edge_name
        sim_data_df_formatted = pd.concat([sim_data_df_formatted, aggr_counts])

    sim_data_df_formatted.reset_index(inplace=True, drop=True)
    sim_data_df_formatted.sort_values(by=['interval_begin'], inplace=True)

    if aggregate_lanes:
        df = sim_data_df_formatted.copy()

        # Extract prefix before "_" (e.g., BXLAND031450F1 from BXLAND031450F1_0)
        df["interval_id"] = df["interval_id"].str.split("_").str[0]

        # Aggregate
        sim_data_df_formatted = (
            df.groupby(["interval_begin", "interval_id"], as_index=False)
            .agg({resolved_col: "sum"})
        )

    result = sim_data_df_formatted.pivot_table(
        index="interval_begin",
        columns="interval_id",
        values=resolved_col,
        aggfunc="sum"
    )
    return result


def convert_offsets_to_timestamps(offsets, base_date_str):
    """
    offsets: list of seconds [0, 3600, 7200, ...]
    base_date_str: "2024-03-01"
    """
    # Interpret the base date at midnight
    base = datetime.strptime(base_date_str, "%Y-%m-%d")
    return [
        (base + timedelta(seconds=s)).strftime("%Y-%m-%d %H:%M:%S")
        for s in offsets
    ]


def main(the_args=None):
    parser = argparse.ArgumentParser(description="Convert the output from detectors (in xml) in table format (CSV)")
    parser.add_argument("-a", "--detectors_def", help="Input detectors definition file", required=True)
    parser.add_argument("-d", "--detectors_out", help="Output detector file", required=True)
    parser.add_argument("-p", "--pivot_parameter", required=True, help="The pivot parameter (the content of the table)")
    parser.add_argument("-t", "--base_date_str", required=False,
                        help="Base date to put into index column in format YYYY-MM-DD instead of default time in seconds.")
    parser.add_argument("-x", "--aggregate_lanes", action="store_true",
                        help="If set, the values of all the sensors in the same lanes will be aggregated (sum). The columns of the output CSV will therefore contain edge names.")
    parser.add_argument("-o", "--output", required=True, help="The path to the output CSV file")

    args = parser.parse_args(the_args)
    det_add_xml = args.detectors_def
    det_out_virt_xml = args.detectors_out
    filename = Path(det_out_virt_xml)
    det_out_virt_csv = str(filename.with_suffix('.csv'))
    # NOTE parse_fast sometimes gives issue if the xml is not well formatted...
    sensors_lanes_mapping = dict(sumolib.xml.parse_fast(det_add_xml, 'inductionLoop', ['id', 'lane']))
    xml2csv.main([det_out_virt_xml, '-o', det_out_virt_csv])

    pivot_df = det_out_to_table(det_out_virt_csv, list(sensors_lanes_mapping.keys()),
                                pivot_attribute=args.pivot_parameter,
                                aggregate_lanes=args.aggregate_lanes)

    pivot_df.index.names = ['ts']
    if args.base_date_str:
        date_str = args.base_date_str
        pivot_df['ts_time'] = convert_offsets_to_timestamps(list(pivot_df.index), date_str)
        pivot_df.set_index('ts_time', inplace=True)
        pivot_df.index.names = ['ts']

    pivot_df.to_csv(args.output, sep=";")


if __name__ == '__main__':
    main()
