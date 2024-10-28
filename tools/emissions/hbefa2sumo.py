#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2022-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    hbefa2sumo.py
# @author  Michael Behrisch
# @date    2022-06-14

# This script parses the coefficients for the HBEFA4 model and converts them to a cpp
# table and a markdown file for the docs. It also writes the list of classes.
# The inputs and some docs are in simo/projects/sumo/data/emissions/HBEFA4 (non-public).

import os
import sys
import csv
from collections import defaultdict
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    op = sumolib.options.ArgumentParser(description="Generate cpp file and docs from HBEFA4 csv data")
    op.add_argument("csv", nargs='?', default="HBEFACoefs_ms.csv.gz",
                    help="define the input data file")
    op.add_argument("-o", "--output-basename", dest="basename", default="HBEFA4",
                    help="define the base name of all output files")
    op.add_argument("-p", "--pm-mode", choices=('sum', 'plain', 'non-exhaust'), default="sum",
                    help="how to handle particular matter values")
    return op.parse_args(args=args)


def main(args=None):
    options = get_options(args)
    coeffs = defaultdict(dict)
    errors = defaultdict(dict)
    share = defaultdict(list)
    id = {}
    with sumolib.openz(options.csv, "rt") as data:
        for line in csv.DictReader(data):
            translation = {"<": "lt", ">": "gt", " ": "_", "'": "_", "*": "s", "&": "_", ";": "_", ",": "_", "/": "_"}
            segment = line['Subsegment'].replace("<=", "le").translate(line['Subsegment'].maketrans(translation))
            if segment[:2] == "PC" and line["VC"] != "PC":
                continue
            id[segment] = (line['Subsegment'], line["i"])
            co = [line[c] for c in ('E0', 'V', 'A', 'V2', 'V3', 'AV', 'AV2')]
            if line['e'] in coeffs[segment] and coeffs[segment][line['e']] != co:
                print("Error! two data sets", segment)
            else:
                coeffs[segment][line['e']] = co
            errors[segment][line['e']] = float(line['wmape'])
            s = "%.4f%%" % (100 * float(line["Share"])) if line["Share"] != "NA" else "-"
            if line["e"] == "FC":
                share[line["VC"]].append((segment, s))
    print(len(coeffs), [(vc, sum([float(si[1][:-1]) for si in s if si[1] != "-"])) for vc, s in share.items()])

    wiki = options.basename + ".md"
    cpp = options.basename + ".cpp"
    txt = options.basename + "classes.txt"
    classes = []
    with open(wiki, "w") as wiki_out, open(cpp, "w") as cpp_out, open(txt, "w") as txt_out:
        print("""
double
HelpersHBEFA4::myFunctionParameter[%s][7][7] = {""" % len(coeffs), file=cpp_out)
        for vc, s in share.items():
            print("##", vc, file=wiki_out)
            print("| SUMO emission class | HBEFA subsegment | HBEFA subsegment ID | fleet share 2022 | "
                  "error CO2 | error CO | error HC | error FC | error NOx | error PM | error FC_MJ |", file=wiki_out)
            print("| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |""", file=wiki_out)
            for segment, val in s:
                print("    {", file=cpp_out)
                print(8 * " " + "//", segment, file=cpp_out)
                err = []
                for emi in ("CO2(total)", "CO", "HC", "FC", "NOx", "PM", "FC_MJ"):
                    scale = 1000.
                    if emi in errors[segment] and coeffs[segment][emi] != 7 * ["0"]:
                        err.append("%.2f%%" % (100 * errors[segment][emi]))
                    else:
                        err.append("-")
                    if emi == "CO2(total)" and emi not in coeffs[segment]:
                        emi = "CO2(rep)"
                    if emi == "PM" and "PM (non-exhaust)" in coeffs[segment] and options.pm_mode != "plain":
                        if options.pm_mode == "sum":
                            emi = "PM + PM (non-exhaust)"
                            coeffs[segment][emi] = [float(c1) + float(c2) for c1, c2 in
                                                    zip(coeffs[segment]["PM"], coeffs[segment]["PM (non-exhaust)"])]
                        else:
                            emi = "PM (non-exhaust)"
                        if "PM (non-exhaust)" in errors[segment]:
                            err[-1] += ", %.2f%%" % (100 * errors[segment]["PM (non-exhaust)"])
                    if emi == "FC_MJ":
                        if coeffs[segment]["FC"] != 7 * ["0"]:
                            scale = 0.
                            err[-1] = "-"
                        else:
                            scale = 1000. / 3.6
                    print(8 * " " + "{", ", ".join(["%.3e" % (scale * float(c))
                          for c in coeffs[segment][emi]]), "}, //", emi, file=cpp_out)
                print("    },", file=cpp_out)
                classes.append((segment, "" if vc in ('PC', 'LCV', 'MC') else " | PollutantsInterface::HEAVY_BIT"))
                print("|", segment, "|", " | ".join(id[segment]), "|", val, "|", " | ".join(err), "|", file=wiki_out)
        print("};", file=cpp_out)

        print("""
    HelpersHBEFA4::HelpersHBEFA4() : PollutantsInterface::Helper("HBEFA4", HBEFA4_BASE, -1) {
        int index = HBEFA4_BASE;""", file=cpp_out)
        print("HBEFA4/zero", file=txt_out)
        for c in classes:
            print('    myEmissionClassStrings.insert("%s", index%s); index++;' % c, file=cpp_out)
            print("HBEFA4/%s" % c[0], file=txt_out)


if __name__ == "__main__":
    main()
