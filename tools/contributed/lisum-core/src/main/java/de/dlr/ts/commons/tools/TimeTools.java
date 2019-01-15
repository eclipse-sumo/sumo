/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.dlr.ts.commons.tools;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

/**
 *
 * @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class TimeTools
{
    private static SimpleDateFormat sdf = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss.SSS");
    private static int thisYear = 0;
    private static long millisMinusThisYear;
    
        
    /**
     * 
     * @return 
     */
    public static int getMinuteOfTheYear()
    {
        return (int) (getMillisOfTheYear() / (60 * 1000));
    }        
    
    public static int getCurrentSecond()
    {
        Calendar calendar = Calendar.getInstance();        
        return calendar.get(Calendar.SECOND);
    }
    
    /**
     * 
     * @return 
     */
    public static long getMillisOfTheYear()
    {
        try {
            Calendar now = Calendar.getInstance();
            int year = now.get(Calendar.YEAR);
            
            if(thisYear == year)
                return System.currentTimeMillis() - millisMinusThisYear;
            
            thisYear = year;
            String myDate = year + "/01/01 00:00:00.000";   
            
            Date date;
            date = sdf.parse(myDate);
            millisMinusThisYear = date.getTime();
                        
            return System.currentTimeMillis() - millisMinusThisYear;
            
        } catch (ParseException ex) {
            ex.printStackTrace();
        }        
        
        return 0;
    }
    
    /**
     * 
     * @param millisecs
     * @return 
     */
    public static String formattedDate(long millisecs)
    {
        SimpleDateFormat date_format = new SimpleDateFormat("MMM dd,yyyy HH:mm:ss.S");
        
        Date resultdate = new Date(millisecs);
        return date_format.format(resultdate);
  }

}
