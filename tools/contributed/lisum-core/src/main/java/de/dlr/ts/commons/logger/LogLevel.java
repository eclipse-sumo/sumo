/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.dlr.ts.commons.logger;


/**
 *
 * @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public enum LogLevel //implements Comparable<LogLevel>
{
    OFF, SEVERE, WARNING, CONFIG, INFO, FINE, FINER, FINEST, ALL;    
    
    /**
     * 
     * @param level
     * @return 
     */
    public static LogLevel parse(String level)
    {
        try {            
            Integer i = Integer.valueOf(level);            
            
            if(i > 7)
                return ALL;                        
            
            return LogLevel.values()[i];            
        } catch (Exception e) {            
            for (LogLevel ll : LogLevel.values())
                if(ll.toString().equalsIgnoreCase(level))
                    return ll;
        }
        
        return null;
    }
    
    
}
