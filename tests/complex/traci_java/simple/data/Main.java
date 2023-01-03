/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2022 German Aerospace Center (DLR) and others.
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
/// @date    2013
///
//
/****************************************************************************/
import org.eclipse.sumo.libtraci.*;

public class Main {
    public static void main(String[] args) {
        System.loadLibrary("libtracijni");

        Simulation.start(new StringVector(new String[] {"sumo",
            "-c", "data/config.sumocfg",
            "--start",
            "--step-length", "0.1"}));

        for (int i = 0; i < 5; i++) {
            Simulation.step();
            Vehicle.add("v" + i, "r1", "car", "now", "0", "0", "max", "current", "max", "current", "", "", "", 0, 0);
            double timeSeconds = Simulation.getTime();
            int tlsPhase = TrafficLight.getPhase("gneJ1");
            String tlsPhaseName = TrafficLight.getPhaseName("gneJ1");
            System.out.println(String.format("Step %s, tlsPhase %s (%s)", timeSeconds, tlsPhase, tlsPhaseName));

            //SumoVehicleData vehData = Inductionloop.getVehicleData("loop1");
            //for (SumoVehicleData.VehicleData d : vehData.ll) {
            //    System.out.println(String.format("  veh=%s len=%s entry=%s leave=%s type=%s", d.vehID, d.length, d.entry_time, d.leave_time, d.typeID));
            //}
        }
        Simulation.close();
    }
}
