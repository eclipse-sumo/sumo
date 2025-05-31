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

# @file    randomODPairs.py
# @author  Davide Guastella
# @date    2025-05-31

"""
    Create random OD pairs for a given road network
    Example of usage:

    Update an existing OD definition (tazRelation) to vary the demand values over each time interval
    - python RandomODPairs.py -i odpairs.xml -a 900 -d 10 20 30 40 -o output.xml

    In this case, the OD pairs from odpairs.xml are copied into the output. In this, the time interval is the same as
    the input, with a frequency of 900. The following values are used for each time interval and all OD pairs. In this
    case, the first interval, all pairs will receive a demand value of 10, in the second interval a demand value of 20,
    and so on.

    If -s is used, all values will be incremented by a delta value drawn form a normal distribution.

    Using OD input file (single demand value)
    - python RandomODPairs.py -i odpairs.xml -a 900 -d 20 -o output.xml -b 0 -e 10800

    In this case, all demand values over the time intervals and od pairs will be replaced by 20

    Using TAZ file and generating 5 random OD pairs
    - python RandomODPairs.py -t taz.xml -o out_odpairs.xml -a 900 -d 50 -b 0 -e 18000 -m 5
"""

import xml.etree.ElementTree as ET
from xml.dom import minidom
import argparse
import sys
import random
import numpy as np


def get_existing_od_pairs(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    # get the OD pairs from the first interval only
    first_interval = root.find('.//interval')
    if first_interval is None:
        raise ValueError("No <interval> element found in the input file.")

    pairs = set()
    for relation in first_interval.findall('tazRelation'):
        from_edge = relation.get('from')
        to_edge = relation.get('to')
        pairs.add((from_edge, to_edge))

    return sorted(pairs)


def random_od_pairs(taz_file, max_pairs):
    tree = ET.parse(taz_file)
    root = tree.getroot()

    taz_ids = [taz.get('id') for taz in root.findall('taz')]
    if len(taz_ids) < 2:
        raise ValueError("TAZ file must contain at least two zones.")

    all_possible = [(f, t) for f in taz_ids for t in taz_ids if f != t]
    if max_pairs > len(all_possible):
        print(
            f"Warning: Requested max OD pairs ({max_pairs}) exceeds total possible ({len(all_possible)}). Reducing to maximum available.")
        max_pairs = len(all_possible)

    selected = random.sample(all_possible, max_pairs)
    return selected


def generate_output_xml(pairs, aggr_freq, demand_values, output_file, begin=None, end=None, scale=None):
    """

    :param pairs: list of OD pairs
    :param aggr_freq: aggregation frequency
    :param demand_values: a list of demand values (1 per interval) to assign to each od pairs over the different time intervals. If one value is provided, this will be used for all pairs/time intervals
    :param output_file:
    :param begin: begin time (in seconds, optional)
    :param end: begin time (in seconds, optional)
    :param scale: the spread of the normal distribution used to vary the demand values (optional)
    :return:
    """
    data = ET.Element("data", {
        "xmlns:xsi": "http://www.w3.org/2001/XMLSchema-instance",
        "xsi:noNamespaceSchemaLocation": "http://sumo.dlr.de/xsd/datamode_file.xsd"
    })

    if begin is not None and end is not None and len(demand_values) == 1:
        ts = begin
        demand = demand_values[0]
        if scale is not None:
            demand += np.random.normal(scale=10)
        while ts < end:
            interval = ET.SubElement(data, "interval", {
                "id": "DEFAULT_VEHTYPE",
                "begin": str(ts),
                "end": str(ts + aggr_freq)
            })
            for from_taz, to_taz in pairs:
                ET.SubElement(interval, "tazRelation", {
                    "from": from_taz,
                    "to": to_taz,
                    "count": str(demand) if scale is None else str(
                        int(demand + max([0, np.random.normal(scale=scale)])))
                })
            ts += aggr_freq
    else:
        ts = 0
        for i, demand in enumerate(demand_values):
            interval = ET.SubElement(data, "interval", {
                "id": "DEFAULT_VEHTYPE",
                "begin": str(ts),
                "end": str(ts + aggr_freq)
            })
            for from_taz, to_taz in pairs:
                ET.SubElement(interval, "tazRelation", {
                    "from": from_taz,
                    "to": to_taz,
                    "count": str(demand) if scale is None else str(
                        int(demand + max([0, np.random.normal(scale=scale)])))
                })
            ts += aggr_freq

    rough_string = ET.tostring(data, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    pretty_xml = reparsed.toprettyxml(indent="  ")

    with open(output_file, "w") as f:
        f.write(pretty_xml)


def main():
    parser = argparse.ArgumentParser(description="Update or generate random OD demand definitions.")
    parser.add_argument("-i", "--input", help="Input OD XML file")
    parser.add_argument("-t", "--taz", help="Input TAZ definition XML file")
    parser.add_argument("-o", "--output", required=True, help="Output XML file")
    parser.add_argument("-d", "--demand", required=True, nargs='+', type=int, help="Demand values")
    parser.add_argument("-a", "--aggr", required=True, type=int, help="Aggregation frequency in seconds")
    parser.add_argument("-b", "--begin", type=int,
                        help="Begin time (optional, required with -e if one demand value is given)")
    parser.add_argument("-e", "--end", type=int,
                        help="End time (optional, required with -b if one demand value is given)")
    parser.add_argument("-m", "--max-od", type=int, help="Max number of OD pairs to generate (required if using -t)")
    parser.add_argument("-s", "--scale", type=int,
                        help="Scale to use for gaussian distribution. If specified, random values drawn from a normal distribution with spread [this value] are added to the OD definition")

    args = parser.parse_args()

    if args.taz:
        # if taz is specified, then get random OD pairs between TAZs
        if not args.max_od:
            print("Error: -m/--max-od must be specified when using -t/--taz.", file=sys.stderr)
            sys.exit(1)
        if len(args.demand) != 1:
            print("Error: Only one demand value is allowed when using -t/--taz.", file=sys.stderr)
            sys.exit(1)
        pairs = random_od_pairs(args.taz, args.max_od)
    elif args.input:
        # otherwise, parse the OD pairs from the existing input file
        pairs = get_existing_od_pairs(args.input)
    else:
        print("Error: Either -i/--input or -t/--taz must be provided.", file=sys.stderr)
        sys.exit(1)

    if (args.begin is not None or args.end is not None) and len(args.demand) != 1:
        print("Error: When using -b and -e, only one demand value should be provided.", file=sys.stderr)
        sys.exit(1)
    if (args.begin is None) != (args.end is None):
        print("Error: Both -b and -e must be provided together.", file=sys.stderr)
        sys.exit(1)

    generate_output_xml(pairs, args.aggr, args.demand, args.output, args.begin, args.end, args.scale)


if __name__ == "__main__":
    main()
