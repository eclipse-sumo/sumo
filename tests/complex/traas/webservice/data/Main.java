/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2020 German Aerospace Center (DLR) and others.
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
import de.tudresden.ws.ServiceImpl;
import de.tudresden.ws.TraasWS;

public class Main {

    static String sumo_bin = "sumo";
    static String config_file = "data/config.sumocfg";
    static double step_length = 1.0;

    public static void main(String[] args) {

        //Start your webservice with the bash or the cmd!
        ServiceImpl ws = new TraasWS().getServiceImplPort();

        //optional
        ws.setSumoBinary(sumo_bin);
        ws.setConfig(config_file);

        ws.addOption("step-length", step_length + "");
        ws.start("user");

        for (int i = 0; i < 3600; i++) {

            ws.doTimestep();
            ws.vehicleAdd("v" + i, "car", "r1", 0, 0, 13.8, (byte) 1);
        }

        ws.stop("user");

    }
}
