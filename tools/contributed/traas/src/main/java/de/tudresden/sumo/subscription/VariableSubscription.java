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
/// @file    VariableSubscription.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.sumo.subscription;

import java.util.LinkedList;

import de.tudresden.sumo.config.Constants;
import it.polito.appeal.traci.protocol.Command;

public class VariableSubscription implements Subscription  {

    SubscribtionVariable sv;
    public double start;
    public double stop;
    public String objectID;
    public LinkedList<Integer> commands;

    public VariableSubscription() {

        //demo
        this.sv = SubscribtionVariable.lane;
        this.start = 0;
        this.stop = 100000 * 60;
        this.objectID = "gneE0_0";
        this.commands = new LinkedList<Integer>();
        this.commands.add(Constants.LAST_STEP_VEHICLE_NUMBER);

    }

    public VariableSubscription(SubscribtionVariable sv, double start, double stop, String objectID) {

        this.sv = sv;
        this.start = start;
        this.stop = stop;
        this.objectID = objectID;
        this.commands = new LinkedList<Integer>();
    }

    public void addCommand(int cmd) {
        this.commands.add(cmd);
    }

    public Command getCommand() {

        Command cmd = new Command(this.sv.id);
        cmd.content().writeDouble(this.start);
        cmd.content().writeDouble(this.stop);
        cmd.content().writeStringASCII(this.objectID);
        cmd.content().writeUnsignedByte(this.commands.size());
        for (Integer ix : this.commands) {
            cmd.content().writeUnsignedByte(ix);
        }

        return cmd;
    }

}
