/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2016-2024 German Aerospace Center (DLR) and others.
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
/// @file    LisaRESTfulServerNotFoundException.java
/// @author  Maximiliano Bottazzi
/// @date    2016
///
//
/****************************************************************************/
package de.dlr.ts.lisum.exceptions;

import de.dlr.ts.commons.logger.DLRLogger;
//import de.dlr.ts.lisum.GlobalConfig;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class LisaRESTfulServerNotFoundException extends Exception {

    public LisaRESTfulServerNotFoundException() {
    }

    public LisaRESTfulServerNotFoundException(String message) {
        super(message);
    }

    public LisaRESTfulServerNotFoundException(Object clazz) {
        String aa = "No Lisa server could be found in ";
        //+
        //GlobalConfig.getInstance().getLisaServerAddress() + ":" +
        //GlobalConfig.getInstance().getLisaServerPort();

        DLRLogger.severe(clazz, aa);
    }
}
