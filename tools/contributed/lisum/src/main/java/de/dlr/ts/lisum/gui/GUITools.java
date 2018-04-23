/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.lisum.gui;

import java.io.PrintStream;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class GUITools
{
    private final static PrintStream DEFAULT_PRINT_STREAM = System.out;
    
    /**
     * 
     */
    public static void setDefaultPrintOutput()
    {
        System.setOut(DEFAULT_PRINT_STREAM);
    }
    
    /**
     * 
     */
    public static void setCustomPrintOutput()
    {
        
    }
}
