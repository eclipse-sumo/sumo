/****************************************************************************/
/// @file    AGFrame.cpp
/// @author  Walter Bamberger & Daniel Krajzewicz
/// @date    Mo, 13 Sept 2010
/// @version $Id: AGMain.cpp 9036 2010-09-03 02:50:45Z simsiem $
///
// Configuration of the options of ActivityGen
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
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
#include "AGFrame.h"
#include <router/ROFrame.h>
#include <duarouter/RODUAFrame.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/options/OptionsCont.h>
#include <version.h>


// ===========================================================================
// method definitions
// ===========================================================================
void AGFrame::fillOptions()
{
	OptionsCont &oc = OptionsCont::getOptions();
	// give some application descriptions
	oc.setApplicationDescription(
			"Generates routes of persons throughout a day for the microscopic road traffic simulation SUMO.");
	oc.setApplicationName("activitygen", "SUMO activitygen Version " + (std::string)VERSION_STRING);
	oc.addCopyrightNotice("(c) Technische Universitaet Muenchen, 2010");
	// Options handling
	oc.addCallExample("--net-file <INPUT>.net.xml --stat-file <INPUT>.stat.xml --output-file <OUTPUT>.rou.xml --abs-rand");
	oc.addCallExample("--net-file <INPUT>.net.xml --stat-file <INPUT>.stat.xml --output-file <OUTPUT>.rou.xml --duration-d <NBR_OF_DAYS>");

	// Add categories and insert the standard options
	SystemFrame::addConfigurationOptions(oc);
	oc.addOptionSubTopic("Input");
	oc.addOptionSubTopic("Output");
	oc.addOptionSubTopic("Processing");
	oc.addOptionSubTopic("Time");
	SystemFrame::addReportOptions(oc);
	RandHelper::insertRandOptions();

	// Insert options
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Use FILE as SUMO-network to route on");

	oc.doRegister("stat-file", 's', new Option_FileName());
	oc.addDescription("stat-file", "Input", "Loads the SUMO-statistics FILE");

    oc.doRegister("output", 'o', new Option_FileName());
    oc.addSynonyme("output-file", "output");
    oc.addDescription("output-file", "Output", "Write generated routes to FILE");

	oc.doRegister("debug", new Option_Bool(false));
	oc.addDescription("debug", "Report",
			"Detailed messages about every single step");

	// TODO: What time options are consistent with other parts of SUMO and
	// useful for the user?
	oc.doRegister("begin", 'b', new Option_Integer());
	oc.addDescription("begin", "Time", "Sets the time of beginning of the simulation during the first day (in seconds)");

	oc.doRegister("end", 'e', new Option_Integer());
	oc.addDescription("end", "Time", "Sets the time of ending of the simulation during the last day (in seconds)");

	oc.doRegister("duration-d", new Option_Integer());
	oc.addDescription("duration-d", "Time", "Sets the duration of the simulation in days");

	// Options needed by the router
    oc.doRegister("interpolate", new Option_Bool(false));
    oc.addDescription("interpolate", "Processing", "Interpolate edge weights at interval boundaries");

    oc.doRegister("expand-weights", new Option_Bool(false));
	oc.addDescription("expand-weights", "Processing",
			"Expand weights behind the simulation's end");
}


bool AGFrame::checkOptions()
{
	return true;
}
