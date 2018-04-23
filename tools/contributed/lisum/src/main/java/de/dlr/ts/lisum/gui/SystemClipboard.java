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

import javafx.scene.input.Clipboard;


/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class SystemClipboard
{
    private static final SystemClipboard INSTANCE = new SystemClipboard();    
    private final Clipboard clipboard = Clipboard.getSystemClipboard();


    /**
     * 
     * @return 
     */
    public static SystemClipboard getInstance()
    {
        return INSTANCE;
    }
    
    /**
     * 
     * @return 
     */
    public Clipboard getClipboard()
    {
        return clipboard;
    }

    /**
     * 
     */
    public SystemClipboard()
    {        
    }    
}
