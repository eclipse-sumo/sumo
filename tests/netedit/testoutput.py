#!/usr/bin/env python3
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

# @file    testoutput.py
# @author  Pablo Alvarez Lopez
# @date    2025-12-05

import os
import subprocess
import sys

# Mappings as a list of lists: [ [left_path, right_path], ... ]
mappings = [
    ["netedit_A.netedit", "netedit_A.netecfg"],
    ["netedit_B.netedit", "netedit_B.netecfg"],
    ["sumo_A.netedit", "sumo_A.sumocfg"],
    ["sumo_B.netedit", "sumo_B.sumocfg"],
    ["sumo.netedit", "sumo.sumocfg"],
    ["netconvert_A.netedit", "netconvert_A.netccfg"],
    ["netconvert_B.netedit", "netconvert_B.netccfg"],
    ["input_net_A_net_xml.netedit", "input_net_A.net.xml"],
    ["input_net_A_xml_gz.netedit", "input_net_A.net.xml.gz"],
    ["input_net_A_xml.netedit", "input_net_A.xml"],
    ["input_net_A_txt.netedit", "input_net_A.txt"],
    ["input_net_B_net_xml.netedit", "input_net_B.net.xml"],
    ["input_net_B_xml_gz.netedit", "input_net_B.net.xml.gz"],
    ["input_net_B_xml.netedit", "input_net_B.xml"],
    ["input_net_B_txt.netedit", "input_net_B.txt"],
    ["input_netconvert_A_con_xml.netedit", "input_netconvert_A.con.xml"],
    ["input_netconvert_A_edg_xml.netedit", "input_netconvert_A.edg.xml"],
    ["input_netconvert_A_nod_xml.netedit", "input_netconvert_A.nod.xml"],
    ["input_netconvert_A_tll_xml.netedit", "input_netconvert_A.tll.xml"],
    ["input_netconvert_B_con_xml.netedit", "input_netconvert_B.con.xml"],
    ["input_netconvert_B_edg_xml.netedit", "input_netconvert_B.edg.xml"],
    ["input_netconvert_B_nod_xml.netedit", "input_netconvert_B.nod.xml"],
    ["input_netconvert_B_tll_xml.netedit", "input_netconvert_B.tll.xml"],
    ["netedit_A_add_xml.netedit", "netedit_A.add.xml"],
    ["netedit_B_add_xml.netedit", "netedit_B.add.xml"],
    ["sumo_A_add_xml.netedit", "sumo_A.add.xml"],
    ["sumo_B_add_xml.netedit", "sumo_B.add.xml"],
    ["input_additionals_A_add_xml.netedit", "input_additionals_A.add.xml"],
    ["input_additionals_A_add_xml_gz.netedit", "input_additionals_A.add.xml.gz"],
    ["input_additionals_A_xml.netedit", "input_additionals_A.xml"],
    ["input_additionals_A_xml_gz.netedit", "input_additionals_A.xml.gz"],
    ["input_additionals_A_txt.netedit", "input_additionals_A.txt"],
    ["input_additionals_B_add_xml.netedit", "input_additionals_B.add.xml"],
    ["input_additionals_B_add_xml_gz.netedit", "input_additionals_B.add.xml.gz"],
    ["input_additionals_B_xml.netedit", "input_additionals_B.xml"],
    ["input_additionals_B_xml_gz.netedit", "input_additionals_B.xml.gz"],
    ["input_additionals_B_txt.netedit", "input_additionals_B.txt"],
    ["netedit_A_rou_xml.netedit", "netedit_A.rou.xml"],
    ["netedit_B_rou_xml.netedit", "netedit_B.rou.xml"],
    ["sumo_A_rou_xml.netedit", "sumo_A.rou.xml"],
    ["sumo_B_rou_xml.netedit", "sumo_B.rou.xml"],
    ["input_routes_A_rou_xml.netedit", "input_routes_A.rou.xml"],
    ["input_routes_A_rou_xml_gz.netedit", "input_routes_A.rou.xml.gz"],
    ["input_routes_A_xml.netedit", "input_routes_A.xml"],
    ["input_routes_A_xml_gz.netedit", "input_routes_A.xml.gz"],
    ["input_routes_A_txt.netedit", "input_routes_A.txt"],
    ["input_routes_B_rou_xml.netedit", "input_routes_B.rou.xml"],
    ["input_routes_B_rou_xml_gz.netedit", "input_routes_B.rou.xml.gz"],
    ["input_routes_B_xml.netedit", "input_routes_B.xml"],
    ["input_routes_B_xml_gz.netedit", "input_routes_B.xml.gz"],
    ["input_routes_B_txt.netedit", "input_routes_B.txt"],
    ["netedit_A_dat_xml.netedit", "netedit_A.dat.xml"],
    ["netedit_B_dat_xml.netedit", "netedit_B.dat.xml"],
    ["sumo_A_dat_xml.netedit", "sumo_A.dat.xml"],
    ["sumo_B_dat_xml.netedit", "sumo_B.dat.xml"],
    ["input_datas_A_dat_xml.netedit", "input_datas_A.dat.xml"],
    ["input_datas_A_dat_xml_gz.netedit", "input_datas_A.dat.xml.gz"],
    ["input_datas_A_xml.netedit", "input_datas_A.xml"],
    ["input_datas_A_xml_gz.netedit", "input_datas_A.xml.gz"],
    ["input_datas_A_txt.netedit", "input_datas_A.txt"],
    ["input_datas_B_dat_xml.netedit", "input_datas_B.dat.xml"],
    ["input_datas_B_dat_xml_gz.netedit", "input_datas_B.dat.xml.gz"],
    ["input_datas_B_xml.netedit", "input_datas_B.xml"],
    ["input_datas_B_xml_gz.netedit", "input_datas_B.xml.gz"],
    ["input_datas_B_txt.netedit", "input_datas_B.txt"],
    ["netedit_A_dat_add_xml.netedit", "netedit_A.dat.add.xml"],
    ["netedit_B_dat_add_xml.netedit", "netedit_B.dat.add.xml"],
    ["sumo_A_dat_add_xml.netedit", "sumo_A.dat.add.xml"],
    ["sumo_B_dat_add_xml.netedit", "sumo_B.dat.add.xml"],
    ["input_meandatas_A_add_xml.netedit", "input_meandatas_A.add.xml"],
    ["input_meandatas_A_add_xml_gz.netedit", "input_meandatas_A.add.xml.gz"],
    ["input_meandatas_A_xml.netedit", "input_meandatas_A.xml"],
    ["input_meandatas_A_xml_gz.netedit", "input_meandatas_A.xml.gz"],
    ["input_meandatas_A_txt.netedit", "input_meandatas_A.txt"],
    ["input_meandatas_B_add_xml.netedit", "input_meandatas_B.add.xml"],
    ["input_meandatas_B_add_xml_gz.netedit", "input_meandatas_B.add.xml.gz"],
    ["input_meandatas_B_xml.netedit", "input_meandatas_B.xml"],
    ["input_meandatas_B_xml_gz.netedit", "input_meandatas_B.xml.gz"],
    ["input_meandatas_B_txt.netedit", "input_meandatas_B.txt"],
]


def main():
    # replace all files
    for src, dst in mappings:
        if os.path.exists(src):
            os.replace(src, dst)
    # get sumo
    sumo = os.environ.get("SUMO_BINARY")
    if not sumo:
        print("Error: SUMO_BINARY must be defined", file=sys.stderr)
        sys.exit(2)
    # run sumo
    if (os.path.exists("sumo_B.sumocfg")):
        rc = subprocess.run([sumo, "-c", "sumo_B.sumocfg", "--no-step-log", "--no-duration-log"]).returncode
    else:
        rc = subprocess.run([sumo, "-c", "sumo.sumocfg", "--no-step-log", "--no-duration-log"]).returncode
    sys.exit(rc)


if __name__ == "__main__":
    main()
