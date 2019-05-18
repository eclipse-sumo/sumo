/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
// TraaS module
// Copyright (C) 2013-2017 Dresden University of Technology
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MultiClient2.java
/// @author  Jakob Erdmann
/// @date    2019
/// @version $Id$
///
//
/****************************************************************************/
import it.polito.appeal.traci.SumoTraciConnection;
import de.tudresden.sumo.cmd.Simulation;
import de.tudresden.sumo.cmd.Vehicle;
import de.tudresden.sumo.cmd.Inductionloop;
import de.tudresden.sumo.cmd.Trafficlight;
import de.tudresden.ws.container.SumoVehicleData;

public class MultiClient2 {

	public static void main(String[] args) {
	
		
		try{
			
			SumoTraciConnection conn = new SumoTraciConnection(9999);
            conn.setOrder(2);
			
			for(int i=0; i<3600; i++){
				conn.do_timestep();
                double timeSeconds = (double)conn.do_job_get(Simulation.getTime());
                System.out.println(String.format("Step %s", timeSeconds));
			}
			
			conn.close();
			
		}catch(Exception ex){ex.printStackTrace();}
		
	}

}
