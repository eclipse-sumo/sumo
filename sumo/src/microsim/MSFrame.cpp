/****************************************************************************/
/// @file    MSFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for microsim; inits global outputs and settings
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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include <microsim/MSJunction.h>
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSDevice_HBEFA.h>
#include <utils/common/RandHelper.h>
#include "MSFrame.h"
#include <utils/common/SystemFrame.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
MSFrame::fillOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addCallExample("-b 0 -e 1000 -n net.xml -r routes.xml");
    oc.addCallExample("-c munich_config.cfg");
    oc.addCallExample("--help");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Time");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("TLS Defaults");
    SystemFrame::addReportOptions(oc); // fill this subtopic, too


    // register configuration options
    // register input options
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Load road network description from FILE");

    oc.doRegister("route-files", 'r', new Option_FileName());
    oc.addSynonyme("route-files", "routes");
    oc.addDescription("route-files", "Input", "Load routes descriptions from FILE(s)");

    oc.doRegister("additional-files", 'a', new Option_FileName());
    oc.addSynonyme("additional-files", "additional");
    oc.addDescription("additional-files", "Input", "Load further descriptions from FILE(s)");

    oc.doRegister("weight-files", 'w', new Option_FileName()); // !!! describe
    oc.addSynonyme("weight-files", "weights");
    oc.addDescription("weight-files", "Input", "Load weights from FILE");
    oc.doRegister("measure", 'm', new Option_String()); // !!! describe
    oc.addDescription("measure", "Input", "Load <measure> from weights");

#ifdef HAVE_MESOSIM
    oc.doRegister("load-state", new Option_FileName());//!!! check, describe
    oc.addDescription("load-state", "Input", "Loads a network state from FILE");
    oc.doRegister("load-state.offset", new Option_String("0", "TIME"));//!!! check, describe
    oc.addDescription("load-state.offset", "Input", "Sets the time offset for vehicle segment exit times.");
#endif

    // register output options
    oc.doRegister("netstate-dump", new Option_FileName());
    oc.addSynonyme("netstate-dump", "ndump");
    oc.addSynonyme("netstate-dump", "netstate");
    oc.addDescription("netstate-dump", "Output", "Save complete network states into FILE");
    oc.doRegister("dump-empty-edges", new Option_Bool(false));
    oc.addDescription("dump-empty-edges", "Output", "Write also empty edges completely when dumping");

    oc.doRegister("summary", new Option_FileName());
    oc.addSynonyme("summary", "emissions-output");
    oc.addSynonyme("summary", "emissions");
    oc.addDescription("summary", "Output", "Save aggregated vehicle emission inf. into FILE");

    oc.doRegister("tripinfo-output", new Option_FileName());
    oc.addSynonyme("tripinfo-output", "tripinfo");
    oc.addDescription("tripinfo-output", "Output", "Save single vehicle trip inf. into FILE");

    oc.doRegister("vehroute-output", new Option_FileName());
    oc.addSynonyme("vehroute-output", "vehroutes");
    oc.addDescription("vehroute-output", "Output", "Save single vehicle route inf. into FILE");

    oc.doRegister("vehroute-output.exit-times", new Option_Bool(false));
    oc.addSynonyme("vehroute-output.exit-times", "vehroutes.exit-times");
    oc.addDescription("vehroute-output.exit-times", "Output", "Write the exit times for all edges");

    oc.doRegister("vehroute-output.last-route", new Option_Bool(false));
    oc.addSynonyme("vehroute-output.last-route", "vehroutes.last-route");
    oc.addDescription("vehroute-output.last-route", "Output", "Write the last route only");

    oc.doRegister("vehroute-output.sorted", new Option_Bool(false));
    oc.addSynonyme("vehroute-output.sorted", "vehroutes.sorted");
    oc.addDescription("vehroute-output.sorted", "Output", "Sorts the output by departure time");

#ifdef HAVE_MESOSIM
    oc.doRegister("save-state.times", new Option_IntVector(IntVector()));//!!! check, describe
    oc.addDescription("save-state.times", "Output", "Use INT[] as times at which a network state written");
    oc.doRegister("save-state.prefix", new Option_FileName());//!!! check, describe
    oc.addDescription("save-state.prefix", "Output", "Prefix for network states");
    oc.doRegister("save-state.files", new Option_FileName());//!!! check, describe
    oc.addDescription("save-state.files", "Output", "Files for network states");
#endif

    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_String("0", "TIME"));
    oc.addDescription("begin", "Time", "Defines the begin time; The simulation starts at this time");

    oc.doRegister("end", 'e', new Option_String("-1", "TIME"));
    oc.addDescription("end", "Time", "Defines the end time; The simulation ends at this time");

#ifdef HAVE_SUBSECOND_TIMESTEPS
    oc.doRegister("step-length", new Option_String("1", "TIME"));
    oc.addDescription("step-length", "Time", "Defines the step duration");
#endif


    // register the processing options
    oc.doRegister("route-steps", 's', new Option_Integer(200));
    oc.addDescription("route-steps", "Processing", "Load routes for the next INT steps ahead");

#ifdef HAVE_INTERNAL_LANES
    oc.doRegister("no-internal-links", new Option_Bool(false));
    oc.addDescription("no-internal-links", "Processing", "Disable (junction) internal links");
#endif

    oc.doRegister("ignore-accidents", new Option_Bool(false));
    oc.addDescription("ignore-accidents", "Processing", "Do not check whether accidents occure more deeply");

    oc.doRegister("ignore-route-errors", new Option_Bool(false));
    oc.addDescription("ignore-route-errors", "Processing", "Do not check whether routes are connected");

    oc.doRegister("too-many-vehicles", new Option_Integer(-1));//!!! check, describe
    oc.addDescription("too-many-vehicles", "Processing", "Quit simulation if this number of vehicles is exceeded");

    oc.doRegister("incremental-dua-step", new Option_Integer(-1));//!!! check, describe
    oc.addDescription("incremental-dua-step", "Processing", "Perform the simulation as a step in incremental DUA");
    oc.doRegister("incremental-dua-base", new Option_Integer(10));//!!! check, describe
    oc.addDescription("incremental-dua-base", "Processing", "Base value for incremental DUA");

    oc.doRegister("time-to-teleport", new Option_String("300", "TIME"));
    oc.addDescription("time-to-teleport", "Processing", "Specify how long a vehicle may wait until being teleported, defaults to 300, values < 1 disable teleporting");

    oc.doRegister("max-depart-delay", new Option_String("-1", "TIME"));
    oc.addDescription("max-depart-delay", "Processing", "How long vehicles wait for departure before being skipped, defaults to -1 which means vehicles are never skipped");

    oc.doRegister("sloppy-insert", new Option_Bool(false));
    oc.addDescription("sloppy-insert", "Processing", "Whether insertion on an edge shall not be repeated in same step once failed.");

    oc.doRegister("lanechange.allow-swap", new Option_Bool(false));
    oc.addDescription("lanechange.allow-swap", "Processing", "Whether blocking vehicles trying to change lanes may be swapped.");


    // devices
    MSDevice_Routing::insertOptions();
    MSDevice_HBEFA::insertOptions();


    // tls
    oc.doRegister("agent-tl.detector-len", new Option_Float(75));//!!! recheck
    oc.addDescription("agent-tl.detector-len", "TLS Defaults", "");

    oc.doRegister("agent-tl.learn-horizon", new Option_Integer(3));//!!! recheck
    oc.addDescription("agent-tl.learn-horizon", "TLS Defaults", "");

    oc.doRegister("agent-tl.decision-horizon", new Option_Integer(1));//!!! recheck
    oc.addDescription("agent-tl.decision-horizon", "TLS Defaults", "");

    oc.doRegister("agent-tl.min-diff", new Option_Float((SUMOReal) .1));//!!! recheck
    oc.addDescription("agent-tl.min-diff", "TLS Defaults", "");

    oc.doRegister("agent-tl.tcycle", new Option_Integer(90));//!!! recheck
    oc.addDescription("agent-tl.tcycle", "TLS Defaults", "");

    oc.doRegister("actuated-tl.detector-pos", new Option_Float(100));//!!! recheck
    oc.addDescription("actuated-tl.detector-pos", "TLS Defaults", "");

    oc.doRegister("actuated-tl.max-gap", new Option_Float(3.1f));//!!! recheck
    oc.addDescription("actuated-tl.max-gap", "TLS Defaults", "");

    oc.doRegister("actuated-tl.detector-gap", new Option_Float(3.0f));//!!! recheck
    oc.addDescription("actuated-tl.detector-gap", "TLS Defaults", "");

    oc.doRegister("actuated-tl.passing-time", new Option_Float(1.9f));//!!! recheck
    oc.addDescription("actuated-tl.passing-time", "TLS Defaults", "");


    // register report options
    oc.doRegister("no-duration-log", new Option_Bool(false));
    oc.addDescription("no-duration-log", "Report", "Disable performance reports for individual simulation steps");

    oc.doRegister("no-step-log", new Option_Bool(false));
    oc.addDescription("no-step-log", "Report", "Disable console output of current simulation step");

    oc.doRegister("message-log", new Option_FileName());
    oc.addDescription("message-log", "Report", "Writes all non-error messages to FILE");

    oc.doRegister("error-log", new Option_FileName());
    oc.addDescription("error-log", "Report", "Writes all warnings and errors to FILE");


#ifndef NO_TRACI
    //remote port 0 if not used
    oc.addOptionSubTopic("TraCI Server");
    oc.doRegister("remote-port", new Option_Integer(0));
    oc.addDescription("remote-port", "TraCI Server", "Enables TraCI Server if set");
#endif
    //
#ifdef HAVE_MESOSIM
    oc.addOptionSubTopic("Mesoscopic");
    oc.doRegister("mesosim", new Option_Bool(false));
    oc.addDescription("mesosim", "Mesoscopic", "Enables mesoscopic simulation");
    oc.doRegister("meso-edgelength", new Option_Float(98.0f));
    oc.addDescription("meso-edgelength", "Mesoscopic", "Length of an edge segment in mesoscopic simulation");
    oc.doRegister("meso-tauff", new Option_String("1.4", "TIME"));
    oc.addDescription("meso-tauff", "Mesoscopic", "Factor for calculating the free-free headway time");
    oc.doRegister("meso-taufj", new Option_String("1.4", "TIME"));
    oc.addDescription("meso-taufj", "Mesoscopic", "Factor for calculating the free-jam headway time");
    oc.doRegister("meso-taujf", new Option_String("2", "TIME"));
    oc.addDescription("meso-taujf", "Mesoscopic", "Factor for calculating the jam-free headway time");
    oc.doRegister("meso-taujj", new Option_String("2", "TIME"));
    oc.addDescription("meso-taujj", "Mesoscopic", "Factor for calculating the jam-jam headway time");
    oc.doRegister("meso-jam-threshold", new Option_Float(0.29f));
    oc.addDescription("meso-jam-threshold", "Mesoscopic", "Minimum percentage of occupied space to consider a segment jammed");
    oc.doRegister("meso-multi-queue", new Option_Bool(false));
    oc.addDescription("meso-multi-queue", "Mesoscopic", "Enable multiple queues at edge ends");
    oc.doRegister("meso-junction-control", new Option_Bool(false));
    oc.addDescription("meso-junction-control", "Mesoscopic", "Enable mesoscopic traffic light and priority junction handling");
    oc.doRegister("meso-recheck", new Option_String("0", "TIME"));
    oc.addDescription("meso-recheck", "Mesoscopic", "Time interval for rechecking insertion into the next segment after failure");
#endif

    // add rand options
    RandHelper::insertRandOptions();

    oc.addOptionSubTopic("GUI Only");
    oc.doRegister("gui-settings-file", new Option_FileName(""));
    oc.addDescription("gui-settings-file", "GUI Only", "Load visualisation settings from FILE");

}


void
MSFrame::buildStreams() throw(IOError) {
    // standard outputs
    OutputDevice::createDeviceByOption("netstate-dump", "sumo-netstate");
    OutputDevice::createDeviceByOption("summary", "summary");
    OutputDevice::createDeviceByOption("tripinfo-output", "tripinfos");
    MSDevice_Vehroutes::init();
}


bool
MSFrame::checkOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    bool ok = true;
    // check the existance of a name for simulation file
    if (!oc.isSet("n")) {
        MsgHandler::getErrorInstance()->inform("No network file (-n) specified.");
        ok = false;
    }
    // check if the begin and the end of the simulation are supplied
    if (!oc.isSet("b")) {
        MsgHandler::getErrorInstance()->inform("The begin of the simulation (-b) is not specified.");
        ok = false;
    }
    if (!oc.isSet("e")) {
        MsgHandler::getErrorInstance()->inform("The end of the simulation (-e) is not specified.");
        ok = false;
    }
    if (oc.isSet("incremental-dua-step") && oc.isSet("incremental-dua-base")) {
        if (oc.getInt("incremental-dua-step") > oc.getInt("incremental-dua-base")) {
            MsgHandler::getErrorInstance()->inform("Invalid dua step");
            ok = false;
        }
    }
    if (oc.getBool("vehroutes.exit-times") && !oc.isSet("vehroutes")) {
        MsgHandler::getErrorInstance()->inform("A vehroute-output file is needed for exit times.");
        ok = false;
    }
    return ok;
}


void
MSFrame::setMSGlobals(OptionsCont &oc) {
    // pre-initialise the network
    // set whether empty edges shall be printed on dump
    MSGlobals::gOmitEmptyEdgesOnDump = !oc.getBool("dump-empty-edges");
#ifdef HAVE_INTERNAL_LANES
    // set whether internal lanes shall be used
    MSGlobals::gUsingInternalLanes = !oc.getBool("no-internal-links");
#else
    MSGlobals::gUsingInternalLanes = false;
#endif
    // set the grid lock time
    MSGlobals::gTimeToGridlock = string2time(oc.getString("time-to-teleport"))<0 ? 0 : string2time(oc.getString("time-to-teleport"));
    MSGlobals::gCheck4Accidents = !oc.getBool("ignore-accidents");
    MSGlobals::gCheckRoutes = !oc.getBool("ignore-route-errors");
#ifdef HAVE_MESOSIM
    MSGlobals::gStateLoaded = oc.isSet("load-state");
#endif
    //
#ifdef HAVE_SUBSECOND_TIMESTEPS
    DELTA_T = string2time(oc.getString("step-length"));
#endif
    //
#ifdef HAVE_MESOSIM
    MSGlobals::gUseMesoSim = oc.getBool("mesosim");
#endif
}



/****************************************************************************/

