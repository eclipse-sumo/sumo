import de.tudresden.ws.ServiceImpl;
import de.tudresden.ws.SumoWebservice;
import de.tudresden.ws.TraasWS;


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

	static String sumo_bin = "f:/Programme/sumo/sumo-svn/bin/sumo.exe";
	static String config_file = "benchmark/config.sumo.cfg";
	static double step_length = 0.01;		

	public static void main(String[] args) {
		
		//Start your webservice with the bash or the cmd!
		ServiceImpl ws = new TraasWS().getServiceImplPort();
		
		//optional
		ws.setSumoBinary(sumo_bin);
		ws.setConfig(config_file);
			
			
		ws.addOption("start", "");
		ws.addOption("step-length", step_length+"");
		ws.start("user");
			
		for(int i=0; i<3600; i++){
				
			ws.doTimestep();
			ws.vehicleAdd("v"+i, "car", "r1", 0, 0, 13.8, (byte) 1);
		}
			
		ws.stop("user");
		
	}

}
