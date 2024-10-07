/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2017-2024 German Aerospace Center (DLR) and others.
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
/// @file    MultiClient2.java
/// @author  Jakob Erdmann
/// @author  Mirko Barthauer
/// @date    2019
///
//
/****************************************************************************/

import org.eclipse.sumo.libtraci.*;

public class MultiClient2 {

    public static void main(String[] args) {

        if (System.getProperty("os.name").startsWith("Windows") && Simulation.class.toString().contains("libsumo")) {
            System.loadLibrary("iconv-2");
            System.loadLibrary("intl-8");
            System.loadLibrary("proj_9_0");
        }
        System.loadLibrary("libtracijni");
        try {
            Simulation.init(9999);
            Simulation.setOrder(2);
            
            
            for (int i = 0; i < 3600; i++) {
                Simulation.step();
                double timeSeconds = Simulation.getTime();
                System.out.println(String.format("Step %s", timeSeconds));
            }

            Simulation.close();

        } catch (Exception ex) {
            ex.printStackTrace();
        }

    }

}
