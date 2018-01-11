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
/// @file    SumoTLSPhase.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.ws.container;

/**
 * 
 * @author Mario Krumnow
 *
 */

public class SumoTLSPhase implements SumoObject {

	public int duration;
	public int duration1;
	public int duration2;
	public String phasedef;
	
	public SumoTLSPhase(){
		
		this.duration = 0;
		this.duration1 = 0;
		this.duration2 = 0;
		this.phasedef = "r";
		
	}
	
	public SumoTLSPhase(int duration, int duration1, int duration2, String phasedef){
		
		this.duration = duration;
		this.duration1 = duration1;
		this.duration2 = duration2;
		this.phasedef = phasedef;
		
	}
	
	public SumoTLSPhase(int duration, String phasedef){
		
		this.duration = duration;
		this.duration1 = duration;
		this.duration2 = duration;
		this.phasedef = phasedef;
		
	}
	
	public String toString(){
		return this.phasedef+"#"+this.duration+"#"+this.duration1+"#"+this.duration2;
	}
	
}
