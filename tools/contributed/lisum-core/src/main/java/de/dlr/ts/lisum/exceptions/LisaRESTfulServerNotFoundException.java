/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    LisaRESTfulServerNotFoundException.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
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
