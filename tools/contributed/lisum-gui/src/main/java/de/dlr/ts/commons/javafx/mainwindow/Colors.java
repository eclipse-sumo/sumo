/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.javafx.mainwindow;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class Colors
{
    private static Colors instance = new Colors();

    /**
     * 
     * @return 
     */
    public static Colors getInstance()
    {
        return instance;
    }
    
    
    public final String DARK_GRAY = "#989898";
}
