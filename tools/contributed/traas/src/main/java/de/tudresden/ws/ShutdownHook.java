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
/// @file    ShutdownHook.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.ws;

/**
 *
 * @author Mario Krumnow
 *
 */

public class ShutdownHook  extends Thread {

    private boolean shutdown;

    /**
     * Instantiates a new shutdown hook.
     */
    public ShutdownHook() {
        setshutdown(false);
    }

    public void run() {
        System.out.println("Shutdown in progress...");

        setshutdown(true);

        try {
            Thread.sleep(3000);
        } catch (Exception e) {}

        System.out.println("Shutdown finished");

    }


    public void setshutdown(boolean shutdown) {
        this.shutdown = shutdown;
    }

    public boolean isshutdown() {
        return shutdown;
    }


}
