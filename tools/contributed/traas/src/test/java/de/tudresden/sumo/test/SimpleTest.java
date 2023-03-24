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
/// @file    SimpleTest.java
/// @author  Mario Krumnow
/// @date    2021
///
//
/****************************************************************************/
package de.tudresden.sumo.test;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

import de.tudresden.sumo.objects.SumoGeometry;

public class SimpleTest {

    public static void main(String[] args) {

        SimpleTest s = new SimpleTest();
        s.validate();

    }

    @Test
    public void validate() {


        StringBuilder sb = new StringBuilder();
        Number size = Math.max(1, Math.random() * 100);

        for (int i = 0; i < size.intValue(); i++) {
            sb.append(i + "," + i + " ");
        }

        SumoGeometry sg = new SumoGeometry();
        sg.fromString(sb.toString().trim());

        assertEquals(sg.coords.size(), size.intValue());

    }

}
