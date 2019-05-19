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
/// @file    Log.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.ws.log;


/**
 * The Interface Logger.
 */
public interface Log {

    /**
     * Write.
     *
     * @param input the input
     */
    /**
     * @param input input
     * @param priority priority level
     */
    public void write(String input, int priority);

    /**
     * @param el stacktrace
     */
    public void write(StackTraceElement[] el);

    /**
     * @param txt_output boolean text output
     */
    public void txt_output(boolean txt_output);

}
