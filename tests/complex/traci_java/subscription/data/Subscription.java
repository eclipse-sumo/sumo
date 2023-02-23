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
/// @file    Subscription.java
/// @author  Jakob Erdmann
/// @date    2019
///
//
/****************************************************************************/
import org.eclipse.sumo.libtraci.*;
import java.util.Map;
import java.util.AbstractMap;
import java.util.TreeMap;

public class Subscription {

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
                                          "--start"
                                                       }));

        Simulation.subscribe(new IntVector(new int[] { libtraci.getVAR_DEPARTED_VEHICLES_IDS() }));

        for (int i = 0; i < 10; i++) {
            Simulation.step();
            Vehicle.add("v" + i, "r1", "car", "now", "0", "0", "max", "current", "max", "current", "", "", "", 0, 0);
            System.out.println("Step: " + i);

            TraCIResults ssRes = Simulation.getSubscriptionResults();
            for (Map.Entry<Integer, TraCIResult> entry : ssRes.entrySet()) {
                TraCIStringList vehIDs = TraCIStringList.cast(entry.getValue());
                for (String vehID : vehIDs.getValue()) {
                    System.out.println("Subscription Departed vehicles: " + vehID);
                    Vehicle.subscribe(vehID, new IntVector(new int[] { Constants.VAR_POSITION, Constants.VAR_SPEED }));
                }
            }
            TreeMap<String, TraCIResults> vsRes = sortedMap(Vehicle.getAllSubscriptionResults());
            for (Map.Entry<String, TraCIResults> vehEntry : vsRes.entrySet()) {
                System.out.println("Vehicle Subscription: id=" + vehEntry.getKey());
                vehEntry.getValue().entrySet().stream().sorted(Map.Entry.comparingByKey())
                    .forEach(entry -> System.out.println("   variable id: " + entry.getKey() + "  value: " + entry.getValue().getString()));
            }
        }

        Simulation.close();
    }

    public static <K, V> TreeMap<K, V> sortedMap(AbstractMap<K, V> map) {
        TreeMap<K, V> result = new TreeMap<K, V>();
        for (Map.Entry<K, V> e : map.entrySet()) {
            result.put(e.getKey(), e.getValue());
        }
        return result;
    }
}
