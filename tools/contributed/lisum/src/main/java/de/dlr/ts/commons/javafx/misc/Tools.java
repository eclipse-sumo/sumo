/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.javafx.misc;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class Tools
{
    /**
     * 
     * @param webColor
     * @return 
     */
    public static String webToKMLColor(String webColor)
    {
        String opa = webColor.substring(6);
        String first = webColor.substring(0, 2);
        String second = webColor.substring(2, 4);
        String third = webColor.substring(4, 6);
        
        return opa + third + second + first;
    }
    
    /**
     * 
     * @param kmlColor
     * @return 
     */
    public static String kmlToWebColor(String kmlColor)
    {
        String opa = kmlColor.substring(0, 2);
        String second = kmlColor.substring(2, 4);
        String third = kmlColor.substring(4, 6);
        String fourth = kmlColor.substring(6, 8);
                
        return fourth + third + second + opa;
    }
}
