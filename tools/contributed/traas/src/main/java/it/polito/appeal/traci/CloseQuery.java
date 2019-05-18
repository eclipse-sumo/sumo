/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
// TraCI4J module
// Copyright (C) 2011 ApPeAL Group, Politecnico di Torino
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    CloseQuery.java
/// @author  Enrico Gueli
/// @author  Mario Krumnow
/// @date    2011
/// @version $Id$
///
//
/****************************************************************************/
package it.polito.appeal.traci;

import it.polito.appeal.traci.protocol.Command;

import java.io.IOException;
import java.net.Socket;

import de.tudresden.sumo.config.Constants;
import de.tudresden.sumo.util.Query;


public class CloseQuery extends Query {

    public CloseQuery(Socket sock) throws IOException {
        super(sock);
    }

    public void doCommand() throws IOException {
        Command req = new Command(Constants.CMD_CLOSE);
        queryAndVerifySingle(req);
    }
}
