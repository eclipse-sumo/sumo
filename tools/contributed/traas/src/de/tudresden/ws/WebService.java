/*   
    Copyright (C) 2016 Mario Krumnow, Dresden University of Technology

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

package de.tudresden.ws;

import javax.xml.ws.Endpoint;

import de.tudresden.ws.conf.Config;
import de.tudresden.ws.log.Log;

/**
 * 
 * @author Mario Krumnow
 *
 */

public class WebService 
{
	
	static Config conf;
	static Log logger;
	
	public static void main (String args[]) 
	{
	
		conf = new Config();
		if(args.length == 1) conf.read_config(args[0]);
		
			logger = conf.logger;
			logger.write(conf.name + " is going to start", 1);			
		    		
			Service server = new ServiceImpl(conf);
			logger.write("The webservice (Version "+server.version()+") is available under " + conf.get_url(),1);
		        	
			//start webservice
			Endpoint endpoint = Endpoint.publish(conf.get_url(), server);
	
	        ShutdownHook shutdownHook = new ShutdownHook();
	        Runtime.getRuntime().addShutdownHook(shutdownHook);
	        
			while( shutdownHook.isshutdown() != true )
			{	
				try{Thread.sleep(1000);} 
				catch (Exception e){logger.write(e.getStackTrace());}	
			}
			
			
			try{endpoint.stop();}
			catch (Exception e){logger.write(e.getStackTrace());}
			
			logger.write(conf.name + " finished successfully.", 1);			

	}
}