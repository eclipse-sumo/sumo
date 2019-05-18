/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AttributeNotFoundException.java
/// @author  Maximiliano Bottazzi
/// @date    2014
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.utils.xmladmin2;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class AttributeNotFoundException extends RuntimeException {

    public AttributeNotFoundException() {
    }

    public AttributeNotFoundException(String message) {
        super(message);
    }

    public AttributeNotFoundException(String message, Throwable cause) {
        super(message, cause);
    }
}
