/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
// TraaS module
// Copyright (C) 2016-2017 Dresden University of Technology
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SumoPrimitive.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.ws.container;

public class SumoPrimitive implements SumoObject {

    public Object val;
    public String type;

    public SumoPrimitive(Object o) {

        this.val = o;
        if (o.getClass().equals(Integer.class)) {
            type = "integer";
        } else if (o.getClass().equals(Double.class)) {
            type = "double";
        } else if (o.getClass().equals(String.class)) {
            type = "string";
        } else {
            System.err.println("unsupported format " + o.getClass());
        }

    }

}
