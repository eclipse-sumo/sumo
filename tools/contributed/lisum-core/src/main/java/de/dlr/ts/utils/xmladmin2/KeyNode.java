/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.dlr.ts.utils.xmladmin2;


/**
 * 
 * @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 * 
 */
class KeyNode //implements Comparable<Attributes>
{
    private String name = "";
    private String value = "";
    private int occurrence = 0;
    private Attributes attributes = new Attributes();
    private boolean selectAll = false;

    
    
    /**
     * 
     * @param occurrence 
     */
    public void setOccurrence(int occurrence)
    {
        this.occurrence = occurrence;
    }
    
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
     * @return 
     */
    public Attributes getAttributes()
    {
        return attributes;
    }
    
    /**
     * 
     * @return 
     */
    public boolean isSelectAll()
    {
        return selectAll;
    }
    
    /**
     * 
     * @return 
     */
    public String getName()
    {
        return name;
    }

    /**
     * 
     * @return 
     */
    public int getOccurrence()
    {
        return occurrence;
    }
    
    /**
     * 
     * @param s
     * @throws de.dlr.ts.utils.xmladmin2.exceptions.MalformedKeyOrNameException
     */
    public KeyNode(String s) throws MalformedKeyOrNameException 
    {
        s = s.trim();

        /**
         * With attributes
         */
        if(!s.contains("["))
            name = s.trim();
        else
        {
            name = s.substring(0, s.indexOf("[")).trim();
            s = s.replace(name, "");
            extractAttributes(s.trim());
        }
    }

    /**
     * 
     * @param line 
     */
    private void extractAttributes(String s) throws MalformedKeyOrNameException
    {
        while(s.contains("[") && !s.isEmpty())
        {
            String tmp = s.substring(s.indexOf("["), s.indexOf("]") + 1);
            
            attributes.add(Tools.parseKeyAttribute(tmp));
            s = s.replace(tmp, "").trim();
        }
    }
    
    /**
     * 
     * @return 
     */
    @Override
    public String toString()
    {
        return "KeyNode name=" + name + " occurr=" + occurrence + " attrs_count=" + attributes.size() + " (" + attributes + ")";
    }

}
