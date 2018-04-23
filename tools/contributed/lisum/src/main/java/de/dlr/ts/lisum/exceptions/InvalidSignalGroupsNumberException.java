/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.lisum.exceptions;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class InvalidSignalGroupsNumberException extends Exception
{

    public InvalidSignalGroupsNumberException()
    {
    }
    
    public InvalidSignalGroupsNumberException(String message)
    {
        super(message);
    }
}
