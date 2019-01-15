/*
 * Copyright (C) 2014
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.utils.xmladmin2;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class XMLNodeNotFoundException extends Exception
{

    public XMLNodeNotFoundException(String message)
    {
        super(message);
    }

    public XMLNodeNotFoundException(String message, Throwable cause)
    {
        super(message, cause);
    }
     
    
}
