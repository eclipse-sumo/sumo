/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// TraaS module
// Copyright (C) 2016-2017 Dresden University of Technology
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SumoTLSProgram.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.ws.container;

import java.util.LinkedList;

/**
 * 
 * @author Mario Krumnow
 *
 */

public class SumoTLSProgram implements SumoObject {

	public String subID;
	public int type;
	public int subParameter;
	public int currentPhaseIndex;
	public LinkedList<SumoTLSPhase> phases;
		
	public SumoTLSProgram(){
		
		this.subID = "unkown";
		this.type = -1;
		this.subParameter = -1;
		this.currentPhaseIndex = -1;
		this.phases = new LinkedList<SumoTLSPhase>();
	}
	
	public SumoTLSProgram(String subID, int type, int subParameter, int currentPhaseIndex){
		
		this.subID = subID;
		this.type = type;
		this.subParameter = subParameter;
		this.currentPhaseIndex = currentPhaseIndex;
		this.phases = new LinkedList<SumoTLSPhase>();
		
	}
	
	public void add(SumoTLSPhase phase){
		this.phases.add(phase);
	}
	
	public String toString(){
		StringBuilder sb = new StringBuilder();
		sb.append(this.subID+"#");
		sb.append(this.type+"#");
		sb.append(this.subParameter+"#");
		sb.append(this.currentPhaseIndex+"#");
		for(SumoTLSPhase sp : this.phases){
			sb.append(sp.toString()+"#");
		}	
		return sb.toString();
	}
	
}
