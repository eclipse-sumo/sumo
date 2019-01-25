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

//import de.dlr.ts.commons.logger.DLRLogger;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
class Value
{
    protected String value = null;
    
    /**
     * 
     * @param value 
     */
    public void setValue(String value)
    {
        this.value = value;
    }
    
    /**
     * 
     * @return 
     */
    public String getValue()
    {
        return value;
    }
    
    /**
     *
     * @param defaultValue
     * @return
     */
    public String getValue(String defaultValue)
    {
        if (value == null || value.isEmpty())
            return defaultValue;

        return value;
    }

    /**
     *
     * @param defaultValue
     * @return
     */
    public boolean getValue(boolean defaultValue)
    {
        if (value == null || value.isEmpty())
            return defaultValue;

        try
        {
            return Boolean.valueOf(value);
        } catch (Exception e)
        {
            XMLAdmin2.printException("Error parsing boolean value: " + value + ", returning default value: " + defaultValue);                   
            return defaultValue;
        }
    }

    /**
     *
     * @param defaultValue
     * @return
     */
    public double getValue(double defaultValue)
    {
        if (value == null || value.isEmpty())
            return defaultValue;

        try
        {
            return Double.valueOf(value);
        } catch (Exception e)
        {
            XMLAdmin2.printException("Error parsing double value: " + value + ", returning default value: " + defaultValue);
            
            //DLRLogger.severe("Error parsing double value: " + value + ", returning default value: " + defaultValue);
            return defaultValue;
        }
    }

    /**
     *
     * @param defaultValue
     * @return
     */
    public int getValue(int defaultValue)
    {
        if (value == null || value.isEmpty())
            return defaultValue;

        try
        {
            return Integer.valueOf(value);
        } catch (Exception ex)
        {
            XMLAdmin2.printException(ex);
            return defaultValue;
        }
    }

    public int getInteger()
    {        
        return Integer.valueOf(value);        
    }
    
    /**
     *
     * @param defaultValue
     * @return
     */
    public long getValue(long defaultValue)
    {
        if (value == null || value.isEmpty())
            return defaultValue;

        try
        {
            return Long.valueOf(value);
        } catch (Exception e)
        {
            XMLAdmin2.printException("Error parsing long value: " + value + ", returning default value: " + defaultValue);            
            //DLRLogger.severe("Error parsing long value: " + value + ", returning default value: " + defaultValue);
            return defaultValue;
        }
    }

}
