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

import de.dlr.ts.commons.logger.DLRLogger;
import de.dlr.ts.lisum.GlobalConfig;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class LisaRESTfulServerNotFoundException extends Exception
{

    public LisaRESTfulServerNotFoundException()
    {
    }
    
    public LisaRESTfulServerNotFoundException(String message)
    {
        super(message);
    }
    
    public LisaRESTfulServerNotFoundException(Object clazz)
    {
        String aa = "No Lisa server could be found in " + 
                    GlobalConfig.getInstance().getLisaServerAddress() + ":" + 
                    GlobalConfig.getInstance().getLisaServerPort();
        
        DLRLogger.severe(clazz, aa);
    }
}
