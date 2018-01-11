/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// TraaS module
// Copyright (C) 2013-2017 Dresden University of Technology
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Main.java
/// @author  Mario Krumnow
/// @date    2013
/// @version $Id$
///
//
/****************************************************************************/
import it.polito.appeal.traci.SumoTraciConnection;
import de.tudresden.sumo.cmd.Vehicle;

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
