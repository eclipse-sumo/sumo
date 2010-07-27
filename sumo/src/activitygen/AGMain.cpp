/****************************************************************************/
/// @file    AGMain.cpp
/// @author  Piotr Woznica & Walter Bamberger
/// @date    Tue, 20 Jul 2010
/// @version $Id: AGMain.cpp 8236 2010-02-10 11:16:41Z behrisch $
///
// Main object of the ActivityGen application
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include <sstream>
#include <vector>
#include <list>
#include <iterator>
#include <exception>
#include <functional>
#include <typeinfo>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <router/RONet.h>
#include <router/ROLoader.h>
#include <duarouter/RODUAFrame.h>
#include <duarouter/RODUAEdgeBuilder.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RandHelper.h>
#include <utils/common/DijkstraRouterTT.h>
#include <utils/common/DijkstraRouterEffort.h>
#include <router/RONetHandler.h>
#include <router/ROFrame.h>
//#include "DTPerson.h"
//ActivityGen
#include "AGActivityGen.h"
#include "city/AGTime.h"

#ifdef CHECK_MEMORY_LEAKS
#include <utils/dev/debug_new.h>
#include <router/ROFrame.h>
#endif

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


void initAndOptions(int argc, char *argv[]) {
	OptionsCont &oc = OptionsCont::getOptions();

	XMLSubSys::init(false);

	// Options handling
	oc.addCallExample("-c <CONFIGURATION>");

	// insert options sub-topics
	SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
	oc.addOptionSubTopic("Input");
	oc.addOptionSubTopic("Output");
	oc.addOptionSubTopic("Processing");
	oc.addOptionSubTopic("Defaults");
	oc.addOptionSubTopic("Time");
	SystemFrame::addReportOptions(oc); // fill this subtopic, too

	// insert options
	ROFrame::fillOptions(oc);
	oc.doRegister("expand-weights", new Option_Bool(false));
	oc.addDescription("expand-weights", "Processing",
			"Expand weights behind the simulation's end");
	//        addImportOptions();
	//        addDUAOptions();
	// add rand options

	// Options of the ActivityGen
	//oc.doRegister("debug", new Option_Bool(false));
	//oc.addDescription("debug", "Report",
	//		"Detailled messages about every single step");

	oc.doRegister("stat-file", 's', new Option_FileName());
	oc.addDescription("stat-file", "Input", "Loads the SUMO-statistics FILE");

	//oc.doRegister("duration-s", new Option_Integer());
	//oc.addDescription("duration-s", "duration", "OPTIONAL sets the duration of the simulation in seconds");

	oc.doRegister("duration-d", new Option_Integer());
	oc.addDescription("duration-d", "duration", "OPTIONAL sets the duration of the simulation in days");

	oc.doRegister("time-begin", new Option_Integer());
	oc.addDescription("time-begin", "begin", "OPTIONAL sets the time of beginning of the simulation during the first day (in seconds)");

	oc.doRegister("time-end", new Option_Integer());
	oc.addDescription("time-end", "end", "OPTIONAL sets the time of ending of the simulation during the last day (in seconds)");

	RandHelper::insertRandOptions();
	OptionsIO::getOptions(true, argc, argv);
	MsgHandler::initOutputOptions();
	//if (!RODUAFrame::checkOptions()) throw ProcessError();
	RandHelper::initRandGlobal();
}

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


int main(int argc, char *argv[]) {
	OptionsCont &oc = OptionsCont::getOptions();
	// give some application descriptions
	oc.setApplicationDescription(
			"Generates routes of persons throughout a day for the microscopic road traffic simulation SUMO.");
#ifdef WIN32
	oc.setApplicationName("activitygen.exe", "SUMO activitygen Version " + (std::string)VERSION_STRING);
#else
	oc.setApplicationName("sumo-activitygen", "SUMO activitygen Version "
			+ (std::string) VERSION_STRING);
#endif
	RONet *net = 0;
	try {
		// Initialise subsystems and process options
		initAndOptions(argc, argv);
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
		std::cout << "--- Nr d Edge: " << net->getEdgeNo() << endl;

	} catch (ProcessError &pe) {
		cout << typeid(pe).name() << ": " << pe.what() << endl;
		return 1;

	} catch (exception &e) {
		cout << "Unknown Exception " << typeid(e).name() << ": " << e.what()
				<< endl;
		return 1;

	} catch (...) {
		cout << "Unknown Exception" << endl;
		return 1;
	}

	cout << setprecision(1);
	// build the router
	/*
	SUMOAbstractRouter<ROEdge, ROVehicle> *router;
    std::string measure = oc.getString("measure");
    if (measure=="traveltime") {
        if (net->hasRestrictions()) {
            router = new DijkstraRouterTT_Direct<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                net->getEdgeNo(), oc.getBool("continue-on-unbuild"), &ROEdge::getTravelTime);
        } else {
            router = new DijkstraRouterTT_Direct<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                net->getEdgeNo(), oc.getBool("continue-on-unbuild"), &ROEdge::getTravelTime);
        }
    } else {
        DijkstraRouterEffort_Direct<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >::Operation op;
        if (measure=="CO") {
            op = &ROEdge::getCOEffort;
        } else if (measure=="CO2") {
            op = &ROEdge::getCO2Effort;
        } else if (measure=="PMx") {
            op = &ROEdge::getPMxEffort;
        } else if (measure=="HC") {
            op = &ROEdge::getHCEffort;
        } else if (measure=="NOx") {
            op = &ROEdge::getNOxEffort;
        } else if (measure=="fuel") {
            op = &ROEdge::getFuelEffort;
        } else if (measure=="noise") {
            op = &ROEdge::getNoiseEffort;
        }
        if (net->hasRestrictions()) {
            router = new DijkstraRouterEffort_Direct<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                net->getEdgeNo(), oc.getBool("continue-on-unbuild"), op, &ROEdge::getTravelTime);
        } else {
            router = new DijkstraRouterEffort_Direct<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                net->getEdgeNo(), oc.getBool("continue-on-unbuild"), op, &ROEdge::getTravelTime);
        }
    }
    */
//here execution of dailyTripsGenerator...
	//DailyTripsGenerator dtGenerator;
	//dtGenerator.generateTrips(*net, *router);

/*
    std::cout << "--- Nr d Edge: " << net->getEdgeNo() << endl;
    std::cout << "longueur de l'edge: " << (net->getEdgeMap()).begin()->second->getID() << " = " << (net->getEdgeMap()).begin()->second->getLength() << endl;
    cout << net->getEdge("--2630#2")->getLength() << endl;
    const map<string, ROEdge*> &edgeMap = net->getEdgeMap();
    ROEdge* rue = edgeMap.find("--2630#2")->second;
    std::cout << "edge particuliere: " << rue->getID() << " = " << rue->getLength() << endl;
*/


//here execution of ActivityGen:
    std::cout << "\n\t ---- begin AcitivtyGen ----\n" << std::endl;
    string statFile = oc.getString("stat-file");
    string routeFile = oc.getString("output-file");
    AGTime duration(0);
    AGTime begin(0);
    AGTime end(0);
    //if(oc.exists("duration-s"))
    //{
    //	duration.addSeconds(oc.getInt("duration-s"));
    //}
    if(oc.isSet("duration-d"))
    {
    	duration.addDays(oc.getInt("duration-d"));
    }
    if(oc.isSet("time-begin"))
    {
    	begin.addSeconds(oc.getInt("time-begin") % 86400);
    }
    if(oc.isSet("time-end"))
    {
    	end.addSeconds(oc.getInt("time-end") % 86400);
    }
    AGActivityGen actiGen(statFile, routeFile, net);
    actiGen.importInfoCity();
    actiGen.makeActivityTrips(duration.getDay()+1, begin.getTime(), end.getTime());

    //AGTime instant(439506);
    //cout << "Time: " << instant.getTime() << endl;
    //cout << "day,hour,min,sec: " << instant.getDay() << ", " << instant.getHour() << ", " << instant.getMinute() << ", " << instant.getSecond() << endl;

    //DijkstraRouterTTBase<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> > dijk(net->getEdgeNo(), false);
    ROEdge* from = net->getEdge("--2989");
    ROEdge* to = net->getEdge("--2985#2");
    //RORouteDef* routeDef = net->getRouteDef("");
    ROVehicle* vehicle;
    SUMOTime time(0);
    std::vector<ROEdge*> into;
    //router->compute(&*from, &*to, &*vehicle, time, into);

    //router->

    std::cout << "\n\t ---- end of ActivityGen ---- \n" << std::endl;
	return 0;
}

/****************************************************************************/

