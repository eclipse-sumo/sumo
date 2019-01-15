/*
 * Copyright (C) 2014
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
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
