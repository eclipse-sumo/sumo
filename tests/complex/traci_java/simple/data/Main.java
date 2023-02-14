/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
// TraaS module
// Copyright (C) 2013-2017 Dresden University of Technology
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    Main.java
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @date    2013
///
//
/****************************************************************************/
import org.eclipse.sumo.libtraci.*;

public class Main {
    public static void main(String[] args) {
        if (System.getProperty("os.name").startsWith("Windows") && Simulation.class.toString().contains("libsumo")) {
            System.loadLibrary("iconv-2");
            System.loadLibrary("intl-8");
            System.loadLibrary("proj_9_0");
        }
        System.loadLibrary("libtracijni");
        String sumo_bin = "sumo";
        String config_file = "data/config.sumocfg";
        double step_length = 0.1;
        if (args.length > 0) {
            sumo_bin = args[0];
        }
        if (args.length > 1) {
            config_file = args[1];
        }

        Simulation.start(new StringVector(new String[] {sumo_bin,
                                          "-c", config_file,
                                          "--start",
                                          "--step-length", "0.1"
                                                       }));

        for (int i = 0; i < 3600; i++) {
            Simulation.step();
            Vehicle.add("v" + i, "r1", "car", "now", "0", "0", "max", "current", "max", "current", "", "", "", 0, 0);
            double timeSeconds = Simulation.getTime();
            int tlsPhase = TrafficLight.getPhase("gneJ1");
            String tlsPhaseName = TrafficLight.getPhaseName("gneJ1");
            System.out.println(String.format("Step %s, tlsPhase %s (%s)", timeSeconds, tlsPhase, tlsPhaseName));

            TraCIVehicleDataVector vehData = InductionLoop.getVehicleData("loop1");
            for (TraCIVehicleData d : vehData) {
                System.out.println(String.format("  veh=%s len=%s entry=%s leave=%s type=%s",
                                                 d.getId(), d.getLength(), d.getEntryTime(), d.getLeaveTime(), d.getTypeID()));
            }
        }
        Simulation.close();
    }
}
