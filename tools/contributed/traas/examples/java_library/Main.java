import it.polito.appeal.traci.SumoTraciConnection;
import de.tudresden.sumo.cmd.Vehicle;


/*   
Copyright (C) 2013 Mario Krumnow, Dresden University of Technology

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

public class Main {

	static String sumo_bin = "f:/Programme/sumo/sumo-svn/bin/sumo-gui.exe";
	static String config_file = "simulation/config.sumo.cfg";
	static double step_length = 0.01;		

	public static void main(String[] args) {
	
		
		try{
			
			SumoTraciConnection conn = new SumoTraciConnection(sumo_bin, config_file);
			conn.addOption("step-length", step_length+"");
			conn.addOption("start", ""); //start sumo immediately
		
			//start Traci Server
			conn.runServer();
			
			for(int i=0; i<3600; i++){
			
				conn.do_timestep();
				conn.do_job_set(Vehicle.add("v"+i, "car", "r1", 0, 0, 13.8, (byte) 1));
	
			}
			
			conn.close();
			
		}catch(Exception ex){ex.printStackTrace();}
		
	}

}
