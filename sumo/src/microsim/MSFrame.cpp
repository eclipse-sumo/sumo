/****************************************************************************/
/// @file    MSFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Jakob Erdmann
/// @author  Axel Wegener
/// @author  Thimor Bohn
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for microsim; inits global outputs and settings
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/geom/GeoConvHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include <microsim/MSBaseVehicle.h>
#include <microsim/MSJunction.h>
#include <microsim/MSRoute.h>
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <microsim/devices/MSDevice.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
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
    OptionsCont& oc = OptionsCont::getOptions();
    oc.addCallExample("-b 0 -e 1000 -n net.xml -r routes.xml", "start a simulation from time 0 to 1000 with given net and routes");
    oc.addCallExample("-c munich_config.cfg", "start with a configuration file");
    oc.addCallExample("--help", "print help");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Time");
    oc.addOptionSubTopic("Processing");
    SystemFrame::addReportOptions(oc); // fill this subtopic, too


    // register configuration options
    //  register input options
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Load road network description from FILE");

    oc.doRegister("route-files", 'r', new Option_FileName());
    oc.addSynonyme("route-files", "routes");
    oc.addDescription("route-files", "Input", "Load routes descriptions from FILE(s)");

    oc.doRegister("additional-files", 'a', new Option_FileName());
    oc.addSynonyme("additional-files", "additional");
    oc.addDescription("additional-files", "Input", "Load further descriptions from FILE(s)");

    oc.doRegister("weight-files", 'w', new Option_FileName());
    oc.addSynonyme("weight-files", "weights");
    oc.addDescription("weight-files", "Input", "Load edge/lane weights for online rerouting from FILE");
    oc.doRegister("weight-attribute", 'x', new Option_String("traveltime"));
    oc.addSynonyme("weight-attribute", "measure", true);
    oc.addDescription("weight-attribute", "Input", "Name of the xml attribute which gives the edge weight");

    oc.doRegister("load-state", new Option_FileName());//!!! check, describe
    oc.addDescription("load-state", "Input", "Loads a network state from FILE");
    oc.doRegister("load-state.offset", new Option_String("0", "TIME"));//!!! check, describe
    oc.addDescription("load-state.offset", "Input", "Sets the time offset for vehicle segment exit times");

    //  register output options
    oc.doRegister("netstate-dump", new Option_FileName());
    oc.addSynonyme("netstate-dump", "ndump");
    oc.addSynonyme("netstate-dump", "netstate");
    oc.addDescription("netstate-dump", "Output", "Save complete network states into FILE");
    oc.doRegister("netstate-dump.empty-edges", new Option_Bool(false));
    oc.addSynonyme("netstate-dump.empty-edges", "netstate.empty-edges");
    oc.addSynonyme("netstate-dump.empty-edges", "dump-empty-edges", true);
    oc.addDescription("netstate-dump.empty-edges", "Output", "Write also empty edges completely when dumping");


    oc.doRegister("emission-output", new Option_FileName());
    oc.addDescription("emission-output", "Output", "Save the emission values of each vehicle");
    oc.doRegister("fcd-output", new Option_FileName());
    oc.addDescription("fcd-output", "Output", "Save the Floating Car Data");
    oc.doRegister("fcd-output.geo", new Option_Bool(false));
    oc.addDescription("fcd-output.geo", "Output", "Save the Floating Car Data using geo-coordinates (lon/lat)");
    oc.doRegister("fcd-output.signals", new Option_Bool(false));
    oc.addDescription("fcd-output.signals", "Output", "Add the vehicle signal state to the FCD output (brake lights etc.)");
    oc.doRegister("full-output", new Option_FileName());
    oc.addDescription("full-output", "Output", "Save a lot of information for each timestep (very redundant)");
    oc.doRegister("queue-output", new Option_FileName());
    oc.addDescription("queue-output", "Output", "Save the vehicle queues at the junctions (experimental)");
    oc.doRegister("vtk-output", new Option_FileName());
    oc.addDescription("vtk-output", "Output", "Save complete vehicle positions inclusive speed values in the VTK Format (usage: /path/out will produce /path/out_$TIMESTEP$.vtp files)");


    oc.doRegister("summary-output", new Option_FileName());
    oc.addSynonyme("summary-output", "summary");
    oc.addSynonyme("summary-output", "emissions-output", true);
    oc.addSynonyme("summary-output", "emissions", true);
    oc.addDescription("summary-output", "Output", "Save aggregated vehicle departure info into FILE");

    oc.doRegister("tripinfo-output", new Option_FileName());
    oc.addSynonyme("tripinfo-output", "tripinfo");
    oc.addDescription("tripinfo-output", "Output", "Save single vehicle trip info into FILE");

    oc.doRegister("vehroute-output", new Option_FileName());
    oc.addSynonyme("vehroute-output", "vehroutes");
    oc.addDescription("vehroute-output", "Output", "Save single vehicle route info into FILE");

    oc.doRegister("vehroute-output.exit-times", new Option_Bool(false));
    oc.addSynonyme("vehroute-output.exit-times", "vehroutes.exit-times");
    oc.addDescription("vehroute-output.exit-times", "Output", "Write the exit times for all edges");

    oc.doRegister("vehroute-output.last-route", new Option_Bool(false));
    oc.addSynonyme("vehroute-output.last-route", "vehroutes.last-route");
    oc.addDescription("vehroute-output.last-route", "Output", "Write the last route only");

    oc.doRegister("vehroute-output.sorted", new Option_Bool(false));
    oc.addSynonyme("vehroute-output.sorted", "vehroutes.sorted");
    oc.addDescription("vehroute-output.sorted", "Output", "Sorts the output by departure time");

    oc.doRegister("vehroute-output.write-unfinished", new Option_Bool(false));
    oc.addDescription("vehroute-output.write-unfinished", "Output", "Write vehroute output for vehicles which have not arrived at simulation end");

    oc.doRegister("link-output", new Option_FileName());
    oc.addDescription("link-output", "Output", "Save links states into FILE");

    oc.doRegister("bt-output", new Option_FileName());
    oc.addDescription("bt-output", "Output", "Save bt visibilities into FILE");


#ifdef _DEBUG
    oc.doRegister("movereminder-output", new Option_FileName());
    oc.addDescription("movereminder-output", "Output", "Save movereminder states of selected vehicles into FILE");
    oc.doRegister("movereminder-output.vehicles", new Option_String());
    oc.addDescription("movereminder-output.vehicles", "Output", "List of vehicle ids which shall save their movereminder states");
#endif

    oc.doRegister("save-state.times", new Option_IntVector(IntVector()));//!!! check, describe
    oc.addDescription("save-state.times", "Output", "Use INT[] as times at which a network state written");
    oc.doRegister("save-state.prefix", new Option_FileName("state"));//!!! check, describe
    oc.addDescription("save-state.prefix", "Output", "Prefix for network states");
    oc.doRegister("save-state.files", new Option_FileName());//!!! check, describe
    oc.addDescription("save-state.files", "Output", "Files for network states");

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
    oc.doRegister("route-steps", 's', new Option_String("200", "TIME"));
    oc.addDescription("route-steps", "Processing", "Load routes for the next number of seconds ahead");

#ifdef HAVE_INTERNAL_LANES
    oc.doRegister("no-internal-links", new Option_Bool(false));
    oc.addDescription("no-internal-links", "Processing", "Disable (junction) internal links");

    oc.doRegister("ignore-junction-blocker", new Option_String("-1", "TIME"));
    oc.addDescription("ignore-junction-blocker", "Processing", "Ignore vehicles which block the junction after they have been standing for SECONDS (-1 means never ignore)");
#endif

    oc.doRegister("ignore-accidents", new Option_Bool(false));
    oc.addDescription("ignore-accidents", "Processing", "Do not check whether accidents occure more deeply");

    oc.doRegister("ignore-route-errors", new Option_Bool(false));
    oc.addDescription("ignore-route-errors", "Processing", "Do not check whether routes are connected");

    oc.doRegister("max-num-vehicles", new Option_Integer(-1));
    oc.addSynonyme("max-num-vehicles", "too-many-vehicles", true);
    oc.addDescription("max-num-vehicles", "Processing", "Quit simulation if this number of vehicles is exceeded");

    oc.doRegister("incremental-dua-step", new Option_Integer());//!!! deprecated
    oc.addDescription("incremental-dua-step", "Processing", "Perform the simulation as a step in incremental DUA");
    oc.doRegister("incremental-dua-base", new Option_Integer(10));//!!! deprecated
    oc.addDescription("incremental-dua-base", "Processing", "Base value for incremental DUA");
    oc.doRegister("scale", new Option_Float());
    oc.addDescription("scale", "Processing", "Scale demand by the given factor (0..1)");

    oc.doRegister("time-to-teleport", new Option_String("300", "TIME"));
    oc.addDescription("time-to-teleport", "Processing", "Specify how long a vehicle may wait until being teleported, defaults to 300, non-positive values disable teleporting");
    oc.doRegister("time-to-teleport.highways", new Option_String("0", "TIME"));
    oc.addDescription("time-to-teleport.highways", "Processing", "The teleport time on highways");

    oc.doRegister("max-depart-delay", new Option_String("-1", "TIME"));
    oc.addDescription("max-depart-delay", "Processing", "How long vehicles wait for departure before being skipped, defaults to -1 which means vehicles are never skipped");

    oc.doRegister("sloppy-insert", new Option_Bool(false));
    oc.addDescription("sloppy-insert", "Processing", "Whether insertion on an edge shall not be repeated in same step once failed");

    oc.doRegister("eager-insert", new Option_Bool(false));
    oc.addDescription("eager-insert", "Processing", "Whether each vehicle is checked separately for insertion on an edge");

    oc.doRegister("lanechange.allow-swap", new Option_Bool(false));
    oc.addDescription("lanechange.allow-swap", "Processing", "Whether blocking vehicles trying to change lanes may be swapped");

    oc.doRegister("lanechange.duration", new Option_String("0", "TIME"));
    oc.addDescription("lanechange.duration", "Processing", "Duration of a lane change maneuver (default 0)");

    oc.doRegister("routing-algorithm", new Option_String("dijkstra"));
    oc.addDescription("routing-algorithm", "Processing",
                      "Select among routing algorithms ['dijkstra', 'astar']");

    // devices
    oc.addOptionSubTopic("Emissions");
    oc.doRegister("phemlight-path", new Option_FileName("./PHEMlight/"));
    oc.addDescription("phemlight-path", "Emissions", "Determines where to load PHEMlight definitions from.");
    oc.addOptionSubTopic("Communication");
    MSDevice::insertOptions(oc);


    // register report options
    oc.doRegister("no-duration-log", new Option_Bool(false));
    oc.addDescription("no-duration-log", "Report", "Disable performance reports for individual simulation steps");

    oc.doRegister("no-step-log", new Option_Bool(false));
    oc.addDescription("no-step-log", "Report", "Disable console output of current simulation step");


#ifndef NO_TRACI
    //remote port 0 if not used
    oc.addOptionSubTopic("TraCI Server");
    oc.doRegister("remote-port", new Option_Integer(0));
    oc.addDescription("remote-port", "TraCI Server", "Enables TraCI Server if set");
#ifdef HAVE_PYTHON
    oc.doRegister("python-script", new Option_String());
    oc.addDescription("python-script", "TraCI Server", "Runs TraCI script with embedded python");
#endif
#endif
    //
#ifdef HAVE_INTERNAL
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
    oc.doRegister("meso-jam-threshold", new Option_Float(-1));
    oc.addDescription("meso-jam-threshold", "Mesoscopic", "Minimum percentage of occupied space to consider a segment jammed. A negative argument causes thresholds to be computed based on edge speed and tauff (default)");
    oc.doRegister("meso-multi-queue", new Option_Bool(false));
    oc.addDescription("meso-multi-queue", "Mesoscopic", "Enable multiple queues at edge ends");
    oc.doRegister("meso-junction-control", new Option_Bool(false));
    oc.addDescription("meso-junction-control", "Mesoscopic", "Enable mesoscopic traffic light and priority junction handling");
    oc.doRegister("meso-junction-control.limited", new Option_Bool(false));
    oc.addDescription("meso-junction-control.limited", "Mesoscopic", "Enable mesoscopic traffic light and priority junction handling for saturated links. This prevents faulty traffic lights from hindering flow in low-traffic situations");
    oc.doRegister("meso-recheck", new Option_String("0", "TIME"));
    oc.addDescription("meso-recheck", "Mesoscopic", "Time interval for rechecking insertion into the next segment after failure");
#endif

    // add rand options
    RandHelper::insertRandOptions();

    // add GUI options
    // the reason that we include them in vanilla sumo as well is to make reusing config files easy
    oc.addOptionSubTopic("GUI Only");
    oc.doRegister("gui-settings-file", new Option_FileName());
    oc.addDescription("gui-settings-file", "GUI Only", "Load visualisation settings from FILE");

    oc.doRegister("quit-on-end", 'Q', new Option_Bool(false));
    oc.addDescription("quit-on-end", "GUI Only", "Quits the GUI when the simulation stops");

    oc.doRegister("game", 'G', new Option_Bool(false));
    oc.addDescription("game", "GUI Only", "Start the GUI in gaming mode");

    oc.doRegister("start", 'S', new Option_Bool(false));
    oc.addDescription("start", "GUI Only", "Start the simulation after loading");

    oc.doRegister("disable-textures", 'T', new Option_Bool(false));
    oc.addDescription("disable-textures", "GUI Only", "Do not load background pictures");

#ifdef HAVE_INTERNAL
    oc.doRegister("osg-view", new Option_Bool(false));
    oc.addDescription("osg-view", "GUI Only", "Start with an OpenSceneGraph view instead of the regular 2D view");
#endif

}


void
MSFrame::buildStreams() {
    // standard outputs
    OutputDevice::createDeviceByOption("netstate-dump", "netstate", "netstate_file.xsd");
    OutputDevice::createDeviceByOption("summary-output", "summary", "summary_file.xsd");
    OutputDevice::createDeviceByOption("tripinfo-output", "tripinfos", "tripinfo_file.xsd");

    //extended
    OutputDevice::createDeviceByOption("fcd-output", "fcd-export", "fcd_file.xsd");
    OutputDevice::createDeviceByOption("emission-output", "emission-export", "emission_file.xsd");
    OutputDevice::createDeviceByOption("full-output", "full-export", "full_file.xsd");
    OutputDevice::createDeviceByOption("queue-output", "queue-export", "queue_file.xsd");

    //OutputDevice::createDeviceByOption("vtk-output", "vtk-export");
    OutputDevice::createDeviceByOption("link-output", "link-output");
    OutputDevice::createDeviceByOption("bt-output", "bt-output");

#ifdef _DEBUG
    OutputDevice::createDeviceByOption("movereminder-output", "movereminder-output");
#endif

    MSDevice_Vehroutes::init();
}


bool
MSFrame::checkOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    bool ok = true;
    if (!oc.isSet("net-file")) {
        WRITE_ERROR("No network file (-n) specified.");
        ok = false;
    }
    if (oc.isSet("incremental-dua-step") && oc.isSet("incremental-dua-base")) {
        WRITE_WARNING("The options 'incremental-dua-step' and 'incremental-dua-base' are deprecated, use 'scale' instead.");
        if (oc.getInt("incremental-dua-step") > oc.getInt("incremental-dua-base")) {
            WRITE_ERROR("Invalid dua step.");
            ok = false;
        }
    }
    if (!oc.isDefault("scale")) {
        if (oc.getFloat("scale") < 0. || oc.getFloat("scale") > 1.) {
            WRITE_ERROR("Invalid scaling factor.");
            ok = false;
        }
    }
    if (oc.getBool("vehroute-output.exit-times") && !oc.isSet("vehroute-output")) {
        WRITE_ERROR("A vehroute-output file is needed for exit times.");
        ok = false;
    }
    if (oc.isSet("gui-settings-file") &&
            oc.getString("gui-settings-file") != "" &&
            !oc.isUsableFileList("gui-settings-file")) {
        ok = false;
    }
#ifdef HAVE_INTERNAL
    if (oc.getBool("meso-junction-control.limited") && !oc.getBool("meso-junction-control")) {
        oc.set("meso-junction-control", "true");
    }
#endif
#ifdef HAVE_SUBSECOND_TIMESTEPS
    if (string2time(oc.getString("step-length")) <= 0) {
        WRITE_ERROR("the minimum step-length is 0.001");
        ok = false;
    }
#endif
#ifdef _DEBUG
    if (oc.isSet("movereminder-output.vehicles") && !oc.isSet("movereminder-output")) {
        WRITE_ERROR("option movereminder-output.vehicles requires option movereminder-output to be set");
        ok = false;
    }
#endif
    if (oc.getBool("sloppy-insert")) {
        WRITE_WARNING("The option 'sloppy-insert' is deprecated, because it is now activated by default, see the new option 'eager-insert'.");
    }
    return ok;
}


void
MSFrame::setMSGlobals(OptionsCont& oc) {
    // pre-initialise the network
    // set whether empty edges shall be printed on dump
    MSGlobals::gOmitEmptyEdgesOnDump = !oc.getBool("netstate-dump.empty-edges");
#ifdef HAVE_INTERNAL_LANES
    // set whether internal lanes shall be used
    MSGlobals::gUsingInternalLanes = !oc.getBool("no-internal-links");
    MSGlobals::gIgnoreJunctionBlocker = string2time(oc.getString("ignore-junction-blocker")) < 0 ?
                                        std::numeric_limits<SUMOTime>::max() : string2time(oc.getString("ignore-junction-blocker"));
#else
    MSGlobals::gUsingInternalLanes = false;
    MSGlobals::gIgnoreJunctionBlocker = 0;
#endif
    // set the grid lock time
    MSGlobals::gTimeToGridlock = string2time(oc.getString("time-to-teleport")) < 0 ? 0 : string2time(oc.getString("time-to-teleport"));
    MSGlobals::gTimeToGridlockHighways = string2time(oc.getString("time-to-teleport.highways")) < 0 ? 0 : string2time(oc.getString("time-to-teleport.highways"));
    MSGlobals::gCheck4Accidents = !oc.getBool("ignore-accidents");
    MSGlobals::gCheckRoutes = !oc.getBool("ignore-route-errors");
    MSGlobals::gLaneChangeDuration = string2time(oc.getString("lanechange.duration"));
    MSGlobals::gStateLoaded = oc.isSet("load-state");
#ifdef HAVE_INTERNAL
    MSGlobals::gUseMesoSim = oc.getBool("mesosim");
    MSGlobals::gMesoLimitedJunctionControl = oc.getBool("meso-junction-control.limited");
    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gUsingInternalLanes = false;
    }
#endif

#ifdef HAVE_SUBSECOND_TIMESTEPS
    DELTA_T = string2time(oc.getString("step-length"));
#endif
#ifdef _DEBUG
    if (oc.isSet("movereminder-output")) {
        MSBaseVehicle::initMoveReminderOutput(oc);
    }
#endif
}



/****************************************************************************/

