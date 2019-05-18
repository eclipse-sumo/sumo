/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    DateTools.java
/// @author  Maximiliano Bottazzi
/// @date    2014
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.commons.logger;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano
 * Bottazzi</a>
 */
public class DateTools {

    private static final DateFormat dfmt = new SimpleDateFormat("yyyyMMdd HH:mm:ss.SSS");
    private static final DateFormat dfmt2 = new SimpleDateFormat("yyyyMMdd_HH.mm.ss.SSS");
    private static final DateFormat dfmt3 = new SimpleDateFormat("yyyyMMdd_HH.mm.ss");

    /**
     *
     * Returns date with the format <i>20150910 10:10:43.059</i>
     *
     * @param millis
     * @return
     */
    public static String getDateStringForLogger(long millis) {
        return dfmt.format(new Date(millis));
    }

    /**
     *
     * Returns date with the format <i>20150910_10.16.13.481</i>
     *
     * @param millis
     * @return
     */
    public static String getDateStringForFileNameWithMillis(long millis) {
        return dfmt2.format(new Date(millis));
    }

    /**
     *
     * Returns date with the format <i>20150910_10.16.13</i>
     *
     * @param millis
     * @return
     */
    public static String getDateStringForFileNameNoMillis(long millis) {
        return dfmt3.format(new Date(millis));
    }
}
