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

package de.tudresden.sumo.util;

import java.util.Iterator;
import java.util.Map.Entry;
import java.util.Set;

import it.polito.appeal.traci.SumoTraciConnection;
import de.tudresden.sumo.util.SumoCommand;
import de.tudresden.ws.conf.Config;
import de.tudresden.ws.log.Log;

/**
 * 
 * @author Mario Krumnow
 *
 */

public class Sumo {

	Log logger;
	Config conf;
	
	public SumoTraciConnection conn;
	

	
	boolean running = false;
	
	//default constructor
	public Sumo(){}
	
	public Sumo(Config conf){
		this.conf=conf;
	}
	
	public void start(String sumo_bin, String configFile){
	
		//start SUMO
		conn = new SumoTraciConnection(sumo_bin, configFile);
		 try{
			 conn.runServer();
			 this.running=true;
			 
		 }catch(Exception ex){
			 ex.printStackTrace();
			 this.running=false;
		 }
	}
	
	public void start(String sumo_bin, String net_file, String route_file){
		
		//start SUMO
		conn = new SumoTraciConnection(
					sumo_bin, //binary
					net_file,   // net file
					route_file // route file
			);
		
		 try{
			 conn.runServer();
			 this.running=true;
		
		 }catch(Exception ex){
			 ex.printStackTrace();
			 this.running=false;
		 }
	}
	
	public boolean set_cmd(Object in){
		
		boolean output = false;
		if(this.running){
			try{
				this.conn.do_job_set((SumoCommand) in);
				output = true;
			}catch(Exception ex){
				this.logger.write(ex.getStackTrace());
			}
		}
		
		return output;
	
	}
	
	public Object get_cmd(SumoCommand cmd){
		
		Object obj = -1;
		if(this.running){
			try{obj = conn.do_job_get(cmd);
			}catch(Exception ex){this.logger.write(ex.getStackTrace());}
		}
		return obj;
	}
	
	public void do_timestep(){

		try {conn.do_timestep();
		} catch (Exception ex) {this.logger.write(ex.getStackTrace());}

	}
	
	@SuppressWarnings("static-access")
	public void start_ws(){

		//start SUMO
		conn = new SumoTraciConnection(conf.sumo_bin, conf.config_file);
		
			//Add Options
			this.add_options();
			
			 try{
				 conn.runServer();
				 this.running=true;
			 }catch(Exception ex){
				 logger.write(ex.getStackTrace());
			 this.running=false;
			 }
			 
	}

	private void add_options(){
		
		Set<Entry<String, String>> set = this.conf.sumo_output.entrySet();
		Iterator<Entry<String, String>> it = set.iterator();
		while(it.hasNext()){
			Entry<String, String> option = it.next();
			conn.addOption(option.getKey(), option.getValue());
		}
		
	}
	
	public void stop_instance(){
		
		try{
			 
			conn.close();
			this.running=false;
			 
		}catch(Exception ex){logger.write(ex.getStackTrace());}
		
	}
	
}
