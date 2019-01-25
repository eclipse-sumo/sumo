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
public class AttributeNotFoundException extends RuntimeException
{

    public AttributeNotFoundException()
    {
    }
    
    public AttributeNotFoundException(String message)
    {
        super(message);
    }
    
    public AttributeNotFoundException(String message, Throwable cause)
    {
        super(message, cause);
    }
}
