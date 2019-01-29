/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.dlr.ts.lisum.lisa;

import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author bott_ma
 */
public class APWertZustType
{        

    /**
     * 
     * @param vector 
     * @return  
     */
    public final static List<String> parse(String vector) 
    {
        if(vector == null || vector.isEmpty())
            return null;
        
        List<String> values = new ArrayList<>();
        
        vector = vector.replaceAll("\\{", "");
        vector = vector.replaceAll("\\}", "");
        
        String[] split = vector.split("/");
                
        for (String s : split)
            values.add(s);        
        
        return values;
    }
    
}
