/*
 * Copyright (C) 2014
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.tools;

/**
 *
 * @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
@Deprecated
public class TypesConversionTools
{
        
    /**
     * 
     * @param b
     * @return 
     */
    @Deprecated
    public static long toLong(byte... b)
    {
        long temp;
        long ret = 0L;
        
        for (int i = 0; i < b.length; i++)
        {
            temp = b[i] << (b.length-1-i)*8;
            ret += temp;
        }
        
        return ret;
    }

    
    public static void main(String[] args)
    {
        System.out.println(toLong((byte)1, (byte)0, (byte)0, (byte)0));
    }
}
