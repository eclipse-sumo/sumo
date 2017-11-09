/*   
    Copyright (C) 2017 Mario Krumnow, Dresden University of Technology

    This file is part of TraaS.

    TraaS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    TraaS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TraaS.  If not, see <http://www.gnu.org/licenses/>.
*/

package de.tudresden.ws.log;

import java.io.FileWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class Log_txt implements Log{

	private boolean txt_output = false;
	
	@Override
	public void write(String input, int priority) {
		    
		
		if(priority==1){
			System.out.println(input);
		}
		
        if(txt_output){
        	
        	 try{
                 FileWriter fw = new FileWriter("output.txt", true);
                 fw.write(get_message(input));
                 fw.flush();
                 fw.close();
           }
           catch( Exception e){System.err.println(e);}
        	
        }
        
		
	}

	public void write(StackTraceElement[] el){
		
		for(int i=el.length-1; i>=0; i--){
			System.err.println(el[i].toString());
		}
		
	}
	
	
	public void txt_output(boolean txt_output){
		this.txt_output=txt_output;
	}
	
	private String get_message(String input){
		
		DateFormat df = new SimpleDateFormat("hh:mm:ss dd.MM.yyyy"); 
		return df.format(new Date()) + " - " + input + "\n";
		
	}
	
}
