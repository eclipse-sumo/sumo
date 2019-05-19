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
/// @file    SumoBestLanes.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.ws.container;

import java.util.LinkedList;

/**
 *
 * @author Mario Krumnow
 *
 */

public class SumoBestLanes implements SumoObject {

    public LinkedList<SumoBestLane> ll;

    public SumoBestLanes() {
        this.ll = new LinkedList<SumoBestLane>();
    }

    public void add(String laneID, double length, double occupation, int offset, int allowsContinuation, LinkedList<String> ll) {
        this.ll.add(new SumoBestLane(laneID, length, occupation, offset, allowsContinuation, ll));
    }

    public String toString() {
        return "";
    }

    public class SumoBestLane {

        public LinkedList<String> ll;
        public String laneID;
        public double length;
        public double occupation;
        public int offset;
        public int allowsContinuation;

        public SumoBestLane(String laneID, double length, double occupation, int offset, int allowsContinuation, LinkedList<String> ll) {
            this.laneID = laneID;
            this.length = length;
            this.occupation = occupation;
            this.offset = offset;
            this.allowsContinuation = allowsContinuation;
            this.ll = ll;
        }
    }
}
