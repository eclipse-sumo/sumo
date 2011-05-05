/****************************************************************************/
/// @file    NWFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id: NWFrame.cpp 10209 2011-05-03 12:29:46Z dkrajzew $
///
// Sets and checks options for netwrite
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "NWFrame.h"
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/common/SystemFrame.h>
#include <netwrite/NWWriter_SUMO.h>
#include <netwrite/NWWriter_MATSim.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
NWFrame::fillOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    // register options
    oc.doRegister("output-format", new Option_String("sumo"));
    oc.addDescription("output-format", "Output", "Determines the output format to use");
}


bool
NWFrame::checkOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
	// check whether the output format is known
	bool ok1 = false;
	std::string outputFormat = oc.getString("output-format");
	if(outputFormat=="sumo") ok1 = true;
	if(outputFormat=="matsim") ok1 = true;
	if(!ok1) {
		MsgHandler::getErrorInstance()->inform("Unknown output format '" + outputFormat + "'.");
	}
	bool ok = ok1;
	//
    return ok;
}


void 
NWFrame::writeNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
	NWWriter_SUMO::writeNetwork(oc, nb);
	NWWriter_MATSim::writeNetwork(oc, nb);
}


/****************************************************************************/

