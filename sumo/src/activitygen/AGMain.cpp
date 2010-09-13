/****************************************************************************/
/// @file    AGMain.cpp
/// @author  Piotr Woznica & Walter Bamberger
/// @date    Tue, 20 Jul 2010
/// @version $Id$
///
// Main object of the ActivityGen application
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <exception>
#include <typeinfo>
#include <router/RONet.h>
#include <router/ROLoader.h>
#include <router/RONetHandler.h>
#include <duarouter/RODUAEdgeBuilder.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SystemFrame.h>
#include <utils/options/OptionsCont.h>
//ActivityGen
#include "AGFrame.h"
#include "AGActivityGen.h"
#include "city/AGTime.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================

/// Loads the network
void loadNet(RONet &toFill, ROAbstractEdgeBuilder &eb) throw (ProcessError) {
	OptionsCont &oc = OptionsCont::getOptions();
	std::string file = oc.getString("net-file");
	if (file == "") {
		throw ProcessError("Missing definition of network to load!");
	}
	if (!FileHelpers::exists(file)) {
		throw ProcessError("The network file '" + file
				+ "' could not be found.");
	}
	MsgHandler::getMessageInstance()->beginProcessMsg("Loading net ...");
	RONetHandler handler(toFill, eb);
	handler.setFileName(file);
	if (!XMLSubSys::runParser(handler, file)) {
		MsgHandler::getMessageInstance()->endProcessMsg("failed.");
		throw ProcessError();
	} else {
		MsgHandler::getMessageInstance()->endProcessMsg("done.");
	}
}

/****************************************************************************/

int main(int argc, char *argv[]) {
	OptionsCont &oc = OptionsCont::getOptions();
	RONet *net = 0;
	try {
		// Initialise subsystems and process options
		XMLSubSys::init(false);
		AGFrame::fillOptions();
		OptionsIO::getOptions(true, argc, argv);
		MsgHandler::initOutputOptions();
		RandHelper::initRandGlobal();
		if (oc.processMetaOptions(argc < 2)) {
			SystemFrame::close();
			return 0;
		}

		// Load network
		net = new RONet();
		RODUAEdgeBuilder builder(oc.getBool("expand-weights"), oc.getBool("interpolate"));
		loadNet(*net, builder);
		MsgHandler::getMessageInstance()->inform("Loaded " + toString(
				net->getEdgeNo()) + " edges.");

	} catch (ProcessError &pe) {
		// TODO Switch to MessageHandler as soon as the operator<< works
		cout << typeid(pe).name() << ": " << pe.what() << endl;
		return 1;

	} catch (exception &e) {
		// TODO Switch to MessageHandler as soon as the operator<< works
		cout << "Unknown Exception " << typeid(e).name() << ": " << e.what()
				<< endl;
		return 1;

#ifndef _DEBUG
	} catch (...) {
		cout << "Unknown Exception" << endl;
		return 1;
#endif
	}

	if (oc.getBool("debug")) {
		MsgHandler::getMessageInstance()->inform("\n\t ---- begin AcitivtyGen ----\n");
	}

    string statFile = oc.getString("stat-file");
    string routeFile = oc.getString("output-file");
    AGTime duration(1,0,0);
    AGTime begin(0);
    AGTime end(0);
    if(oc.isSet("duration-d"))
    {
    	duration.setDay(oc.getInt("duration-d"));
    }
    if(oc.isSet("begin"))
    {
    	begin.addSeconds(oc.getInt("begin") % 86400);
    }
    if(oc.isSet("end"))
    {
    	end.addSeconds(oc.getInt("end") % 86400);
    }
    AGActivityGen actiGen(statFile, routeFile, net);
    actiGen.importInfoCity();
    actiGen.makeActivityTrips(duration.getDay(), begin.getTime(), end.getTime());

	if (oc.getBool("debug")) {
		MsgHandler::getMessageInstance()->inform("\n\t ---- end of ActivityGen ----\n");
	}
	return 0;
}

/****************************************************************************/

