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
import java.util.LinkedList;
import it.polito.appeal.traci.SumoTraciConnection;
import de.tudresden.sumo.cmd.Simulation;
import de.tudresden.sumo.cmd.Vehicle;
import de.tudresden.sumo.cmd.Inductionloop;
import de.tudresden.sumo.cmd.Trafficlight;
import de.tudresden.ws.container.SumoVehicleData;
import de.tudresden.ws.container.SumoStopFlags;
import de.tudresden.ws.container.SumoStage;

public class APITest {

	static String sumo_bin = "sumo";
	static String config_file = "simulation/config.sumo.cfg";
	static double step_length = 1.0;		

	public static void main(String[] args) {
	
		
		try{
			
			SumoTraciConnection conn = new SumoTraciConnection(sumo_bin, config_file);
			conn.addOption("step-length", step_length+"");
			conn.addOption("start", "true"); //start sumo immediately
		
			//start Traci Server
			conn.runServer();
            conn.setOrder(1);

            SumoStage stage = (SumoStage)conn.do_job_get(Simulation.findRoute("gneE0", "gneE2", "car", 0, 0));
            System.out.println("findRoute result stage:");
            for (String s : stage.edges) {
                System.out.println("  " + s);
            }
            LinkedList<SumoStage> stages = (LinkedList<SumoStage>)conn.do_job_get(Simulation.findIntermodalRoute("gneE0", "gneE2", "", 0, 0,
                        1.5, 1, 50, 50, 0, "", "", ""));
            System.out.println("findIntermodalRoute result stages:");
            for (SumoStage s2 : stages) {
                for (String s : s2.edges) {
                    System.out.println("  " + s);
                }
			
            }
			for(int i=0; i<36; i++){
			
				conn.do_timestep();
				conn.do_job_set(Vehicle.addFull("v"+i, "r1", "car", "now", "0", "0", "max", "current", "max", "current", "", "", "", 0, 0));
                double timeSeconds = (double)conn.do_job_get(Simulation.getTime());
                int tlsPhase = (int)conn.do_job_get(Trafficlight.getPhase("gneJ1"));
                String tlsPhaseName = (String)conn.do_job_get(Trafficlight.getPhaseName("gneJ1"));
                System.out.println(String.format("Step %s, tlsPhase %s (%s)", timeSeconds, tlsPhase, tlsPhaseName));

                SumoVehicleData vehData = (SumoVehicleData)conn.do_job_get(Inductionloop.getVehicleData("loop1"));
                for (SumoVehicleData.VehicleData d : vehData.ll) {
                    System.out.println(String.format("  veh=%s len=%s entry=%s leave=%s type=%s", d.vehID, d.length, d.entry_time, d.leave_time, d.typeID));
                }
                if (i == 10) {
                    SumoStopFlags flags = new SumoStopFlags(false, false , false, false, false);
                    conn.do_job_set(Vehicle.setStop("v0","gneE0", 200, (byte)0, 10, flags));
                }
			}
			
			conn.close();
			
		}catch(Exception ex){ex.printStackTrace();}
		
	}

}
