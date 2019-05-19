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
/// @file    Service.java
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

public interface Service {

    public String start(String user);

    public String stop(String user);

    public void addOption(String name, String value);

    public void doTimestep();

    public void setConfig(String filename);

    public void setSumoBinary(String filename);


    public String get_Status(String user);

    public String LastActionTime();

    public String version();

    public String TXT_output(boolean input);

}
