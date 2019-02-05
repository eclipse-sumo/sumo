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
    
    public static void main(String[] args) {
        List<String> a = new ArrayList<>();
        
        a.add("111");
        a.add("222");
        a.add("");
        a.add("333");
        a.add("333");
        
        System.out.println(generate(a));
    }
    /**
     * 
     * @param apWerte
     * @return 
     */
    public final static String generate(List<String> apWerte) 
    {   
        StringBuilder sb = new StringBuilder();
        boolean addIndex = false;
        
        for (int i = 0; i < apWerte.size(); i++) {
            if(apWerte.get(i).isEmpty()) {
                addIndex = true;
                continue;
            }                
            
            if(addIndex)
            sb.append("(").append(i).append(")");
            sb.append(apWerte.get(i));
            sb.append("!");
            addIndex = false;
        }
        
        String ret = sb.toString();
        if(!ret.isEmpty())
            ret = ret.substring(0, ret.length()-1);
                
        return ret;
    }
        
}
