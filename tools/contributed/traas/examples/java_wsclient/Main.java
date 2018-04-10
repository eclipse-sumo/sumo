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
import de.tudresden.ws.ServiceImpl;
import de.tudresden.ws.SumoWebservice;
import de.tudresden.ws.TraasWS;

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
