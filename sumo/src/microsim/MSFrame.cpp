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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
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
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/devices/MSDevice.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/output/MSStopOut.h>
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
    oc.addOptionSubTopic("Routing");
    SystemFrame::addReportOptions(oc); // fill this subtopic, too


    // register configuration options
    //  register input options
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Load road network description from FILE");
    oc.addXMLDefault("net-file", "net");

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
    oc.addDescription("load-state.offset", "Input", "Shifts all times loaded from a saved state by the given offset");
    oc.doRegister("load-state.remove-vehicles", new Option_String(""));
    oc.addDescription("load-state.remove-vehicles", "Input", "Removes vehicles with the given IDs from the loaded state");

    //  register output options
    oc.doRegister("netstate-dump", new Option_FileName());
    oc.addSynonyme("netstate-dump", "ndump");
    oc.addSynonyme("netstate-dump", "netstate");
    oc.addSynonyme("netstate-dump", "netstate-output");
    oc.addDescription("netstate-dump", "Output", "Save complete network states into FILE");
    oc.doRegister("netstate-dump.empty-edges", new Option_Bool(false));
    oc.addSynonyme("netstate-dump.empty-edges", "netstate.empty-edges");
    oc.addSynonyme("netstate-dump.empty-edges", "netstate-output.empty-edges");
    oc.addSynonyme("netstate-dump.empty-edges", "dump-empty-edges", true);
    oc.addDescription("netstate-dump.empty-edges", "Output", "Write also empty edges completely when dumping");
    oc.doRegister("netstate-dump.precision", new Option_Integer(2));
    oc.addSynonyme("netstate-dump.precision", "netstate.precision");
    oc.addSynonyme("netstate-dump.precision", "netstate-output.precision");
    oc.addSynonyme("netstate-dump.precision", "dump-precision", true);
    oc.addDescription("netstate-dump.precision", "Output", "Write positions and speeds with the given precision (default 2)");


    oc.doRegister("emission-output", new Option_FileName());
    oc.addDescription("emission-output", "Output", "Save the emission values of each vehicle");
    oc.doRegister("emission-output.precision", new Option_Integer(2));
    oc.addDescription("emission-output.precision", "Output", "Write emission values with the given precision (default 2)");

    oc.doRegister("battery-output", new Option_FileName());
    oc.addDescription("battery-output", "Output", "Save the battery values of each vehicle");
    oc.doRegister("battery-output.precision", new Option_Integer(2));
    oc.addDescription("battery-output.precision", "Output", "Write battery values with the given precision (default 2)");

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
    oc.doRegister("amitran-output", new Option_FileName());
    oc.addDescription("amitran-output", "Output", "Save the vehicle trajectories in the Amitran format");


    oc.doRegister("summary-output", new Option_FileName());
    oc.addSynonyme("summary-output", "summary");
    oc.addDescription("summary-output", "Output", "Save aggregated vehicle departure info into FILE");

    oc.doRegister("tripinfo-output", new Option_FileName());
    oc.addSynonyme("tripinfo-output", "tripinfo");
    oc.addDescription("tripinfo-output", "Output", "Save single vehicle trip info into FILE");

    oc.doRegister("tripinfo-output.write-unfinished", new Option_Bool(false));
    oc.addDescription("tripinfo-output.write-unfinished", "Output", "Write tripinfo output for vehicles which have not arrived at simulation end");

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

    oc.doRegister("vehroute-output.dua", new Option_Bool(false));
    oc.addSynonyme("vehroute-output.dua", "vehroutes.dua");
    oc.addDescription("vehroute-output.dua", "Output", "Write the output in the duarouter alternatives style");

    oc.doRegister("vehroute-output.intended-depart", new Option_Bool(false));
    oc.addSynonyme("vehroute-output.intended-depart", "vehroutes.intended-depart");
    oc.addDescription("vehroute-output.intended-depart", "Output", "Write the output with the intended instead of the real departure time");

    oc.doRegister("vehroute-output.route-length", new Option_Bool(false));
    oc.addSynonyme("vehroute-output.route-length", "vehroutes.route-length");
    oc.addDescription("vehroute-output.route-length", "Output", "Include total route length in the output");

    oc.doRegister("vehroute-output.write-unfinished", new Option_Bool(false));
    oc.addDescription("vehroute-output.write-unfinished", "Output", "Write vehroute output for vehicles which have not arrived at simulation end");

    oc.doRegister("link-output", new Option_FileName());
    oc.addDescription("link-output", "Output", "Save links states into FILE");

    oc.doRegister("bt-output", new Option_FileName());
    oc.addDescription("bt-output", "Output", "Save bluetooth visibilities into FILE (in conjunction with device.btreceiver and device.btsender)");

    oc.doRegister("lanechange-output", new Option_FileName());
    oc.addDescription("lanechange-output", "Output", "Record lane changes and their motivations for all vehicles into FILE");

    oc.doRegister("stop-output", new Option_FileName());
    oc.addDescription("stop-output", "Output", "Record stops and loading/unloading of passenger and containers for all vehicles into FILE");

#ifdef _DEBUG
    oc.doRegister("movereminder-output", new Option_FileName());
    oc.addDescription("movereminder-output", "Output", "Save movereminder states of selected vehicles into FILE");
    oc.doRegister("movereminder-output.vehicles", new Option_String());
    oc.addDescription("movereminder-output.vehicles", "Output", "List of vehicle ids which shall save their movereminder states");
#endif

    oc.doRegister("save-state.times", new Option_IntVector(IntVector()));
    oc.addDescription("save-state.times", "Output", "Use INT[] as times at which a network state written");
    oc.doRegister("save-state.period", new Option_String("-1", "TIME"));
    oc.addDescription("save-state.period", "Output", "save state repeatedly after TIME period");
    oc.doRegister("save-state.prefix", new Option_FileName("state"));
    oc.addDescription("save-state.prefix", "Output", "Prefix for network states");
    oc.doRegister("save-state.suffix", new Option_FileName(".sbx"));
    oc.addDescription("save-state.suffix", "Output", "Suffix for network states (.sbx or .xml)");
    oc.doRegister("save-state.files", new Option_FileName());//
    oc.addDescription("save-state.files", "Output", "Files for network states");

    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_String("0", "TIME"));
    oc.addDescription("begin", "Time", "Defines the begin time in seconds; The simulation starts at this time");

    oc.doRegister("end", 'e', new Option_String("-1", "TIME"));
    oc.addDescription("end", "Time", "Defines the end time in seconds; The simulation ends at this time");

    oc.doRegister("step-length", new Option_String("1", "TIME"));
    oc.addDescription("step-length", "Time", "Defines the step duration in seconds");

    oc.doRegister("step-method.ballistic", new Option_Bool(false));
    oc.addDescription("step-method.ballistic", "Processing", "Whether to use ballistic method for the positional update of vehicles (default is a semi-implicit Euler method).");

    oc.doRegister("lateral-resolution", new Option_Float(-1));
    oc.addDescription("lateral-resolution", "Processing", "Defines the resolution in m when handling lateral positioning within a lane (with -1 all vehicles drive at the center of their lane");

    // register the processing options
    oc.doRegister("route-steps", 's', new Option_String("200", "TIME"));
    oc.addDescription("route-steps", "Processing", "Load routes for the next number of seconds ahead");

#ifdef HAVE_INTERNAL_LANES
    oc.doRegister("no-internal-links", new Option_Bool(false));
    oc.addDescription("no-internal-links", "Processing", "Disable (junction) internal links");

    oc.doRegister("ignore-junction-blocker", new Option_String("-1", "TIME"));
    oc.addDescription("ignore-junction-blocker", "Processing", "Ignore vehicles which block the junction after they have been standing for SECONDS (-1 means never ignore)");
#endif

    oc.doRegister("ignore-route-errors", new Option_Bool(false));
    oc.addDescription("ignore-route-errors", "Processing", "Do not check whether routes are connected");

    oc.doRegister("ignore-accidents", new Option_Bool(false));
    oc.addDescription("ignore-accidents", "Processing", "Do not check whether accidents occur");

    oc.doRegister("collision.action", new Option_String("teleport"));
    oc.addDescription("collision.action", "Processing", "How to deal with collisions: [none,warn,teleport,remove]");

    oc.doRegister("collision.check-junctions", new Option_Bool(false));
    oc.addDescription("collision.check-junctions", "Processing", "Enables collisions checks on junctions");

    oc.doRegister("max-num-vehicles", new Option_Integer(-1));
    oc.addDescription("max-num-vehicles", "Processing", "Delay vehicle insertion to stay within the given maximum number");

    oc.doRegister("scale", new Option_Float(1.));
    oc.addDescription("scale", "Processing", "Scale demand by the given factor (by discarding or duplicating vehicles)");

    oc.doRegister("time-to-teleport", new Option_String("300", "TIME"));
    oc.addDescription("time-to-teleport", "Processing", "Specify how long a vehicle may wait until being teleported, defaults to 300, non-positive values disable teleporting");

    oc.doRegister("time-to-teleport.highways", new Option_String("0", "TIME"));
    oc.addDescription("time-to-teleport.highways", "Processing", "The waiting time after which vehicles on a fast road (speed > 69m/s) are teleported if they are on a non-continuing lane");

    oc.doRegister("waiting-time-memory", new Option_String("100", "TIME"));
    oc.addDescription("waiting-time-memory", "Processing", "Length of time interval, over which accumulated waiting time is taken into account");

    oc.doRegister("max-depart-delay", new Option_String("-1", "TIME"));
    oc.addDescription("max-depart-delay", "Processing", "How long vehicles wait for departure before being skipped, defaults to -1 which means vehicles are never skipped");

    oc.doRegister("sloppy-insert", new Option_Bool(false));
    oc.addDescription("sloppy-insert", "Processing", "Whether insertion on an edge shall not be repeated in same step once failed");

    oc.doRegister("eager-insert", new Option_Bool(false));
    oc.addDescription("eager-insert", "Processing", "Whether each vehicle is checked separately for insertion on an edge");

    oc.doRegister("random-depart-offset", new Option_String("0", "TIME"));
    oc.addDescription("random-depart-offset", "Processing", "Each vehicle receives a random offset to its depart value drawn uniformly from [0, TIME]");

    oc.doRegister("lanechange.duration", new Option_String("0", "TIME"));
    oc.addDescription("lanechange.duration", "Processing", "Duration of a lane change maneuver (default 0)");

    oc.doRegister("lanechange.overtake-right", new Option_Bool(false));
    oc.addDescription("lanechange.overtake-right", "Processing", "Whether overtaking on the right on motorways is permitted");

    oc.doRegister("tls.all-off", new Option_Bool(false));
    oc.addDescription("tls.all-off", "Processing", "Switches off all traffic lights.");

    // pedestrian model
    oc.doRegister("pedestrian.model", new Option_String("striping"));
    oc.addDescription("pedestrian.model", "Processing", "Select among pedestrian models ['nonInteracting', 'striping']");

    oc.doRegister("pedestrian.striping.stripe-width", new Option_Float(0.65));
    oc.addDescription("pedestrian.striping.stripe-width", "Processing", "Width of parallel stripes for segmenting a sidewalk (meters) for use with model 'striping'");

    oc.doRegister("pedestrian.striping.dawdling", new Option_Float(0.2));
    oc.addDescription("pedestrian.striping.dawdling", "Processing", "factor for random slow-downs [0,1] for use with model 'striping'");

    oc.doRegister("pedestrian.striping.jamtime", new Option_String("300", "TIME"));
    oc.addDescription("pedestrian.striping.jamtime", "Processing", "Time in seconds after which pedestrians start squeezing through a jam when using model 'striping' (non-positive values disable squeezing)");

    // generic routing options
    oc.doRegister("routing-algorithm", new Option_String("dijkstra"));
    oc.addDescription("routing-algorithm", "Routing",
                      "Select among routing algorithms ['dijkstra', 'astar', 'CH', 'CHWrapper']");
    oc.doRegister("weights.random-factor", new Option_Float(1.));
    oc.addDescription("weights.random-factor", "Routing", "Edge weights for routing are dynamically disturbed by a random factor drawn uniformly from [1,FLOAT)");

    // devices
    oc.addOptionSubTopic("Emissions");
    oc.doRegister("phemlight-path", new Option_FileName("./PHEMlight/"));
    oc.addDescription("phemlight-path", "Emissions", "Determines where to load PHEMlight definitions from.");

    oc.addOptionSubTopic("Communication");
    oc.addOptionSubTopic("Battery");
    MSDevice::insertOptions(oc);

    // register report options
    oc.doRegister("duration-log.disable", new Option_Bool(false));
    oc.addSynonyme("duration-log.disable", "no-duration-log", false);
    oc.addDescription("duration-log.disable", "Report", "Disable performance reports for individual simulation steps");

    oc.doRegister("duration-log.statistics", new Option_Bool(false));
    oc.addDescription("duration-log.statistics", "Report", "Enable statistics on vehicle trips");

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
    oc.addOptionSubTopic("Mesoscopic");
    oc.doRegister("mesosim", new Option_Bool(false));
    oc.addDescription("mesosim", "Mesoscopic", "Enables mesoscopic simulation");
    oc.doRegister("meso-edgelength", new Option_Float(98.0f));
    oc.addDescription("meso-edgelength", "Mesoscopic", "Length of an edge segment in mesoscopic simulation");
    oc.doRegister("meso-tauff", new Option_String("1.13", "TIME"));
    oc.addDescription("meso-tauff", "Mesoscopic", "Factor for calculating the net free-free headway time");
    oc.doRegister("meso-taufj", new Option_String("1.13", "TIME"));
    oc.addDescription("meso-taufj", "Mesoscopic", "Factor for calculating the net free-jam headway time");
    oc.doRegister("meso-taujf", new Option_String("1.73", "TIME"));
    oc.addDescription("meso-taujf", "Mesoscopic", "Factor for calculating the jam-free headway time");
    oc.doRegister("meso-taujj", new Option_String("1.4", "TIME"));
    oc.addDescription("meso-taujj", "Mesoscopic", "Factor for calculating the jam-jam headway time");
    oc.doRegister("meso-jam-threshold", new Option_Float(-1));
    oc.addDescription("meso-jam-threshold", "Mesoscopic",
                      "Minimum percentage of occupied space to consider a segment jammed. A negative argument causes thresholds to be computed based on edge speed and tauff (default)");
    oc.doRegister("meso-multi-queue", new Option_Bool(true));
    oc.addDescription("meso-multi-queue", "Mesoscopic", "Enable multiple queues at edge ends");
    oc.doRegister("meso-junction-control", new Option_Bool(false));
    oc.addDescription("meso-junction-control", "Mesoscopic", "Enable mesoscopic traffic light and priority junction handling");
    oc.doRegister("meso-junction-control.limited", new Option_Bool(false));
    oc.addDescription("meso-junction-control.limited", "Mesoscopic",
                      "Enable mesoscopic traffic light and priority junction handling for saturated links. This prevents faulty traffic lights from hindering flow in low-traffic situations");
    oc.doRegister("meso-tls-penalty", new Option_Float(0));
    oc.addDescription("meso-tls-penalty", "Mesoscopic",
                      "Apply scaled time penalties when driving across tls controlled junctions based on green split instead of checking actual phases");
    oc.doRegister("meso-minor-penalty", new Option_String("0", "TIME"));
    oc.addDescription("meso-minor-penalty", "Mesoscopic",
                      "Apply fixed time penalty when driving across a minor link. When using --meso-junction-control.limited, the penalty is not applied whenever limited control is active.");
    oc.doRegister("meso-overtaking", new Option_Bool(false));
    oc.addDescription("meso-overtaking", "Mesoscopic", "Enable mesoscopic overtaking");
    oc.doRegister("meso-recheck", new Option_String("0", "TIME"));
    oc.addDescription("meso-recheck", "Mesoscopic", "Time interval for rechecking insertion into the next segment after failure");

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

    oc.doRegister("demo", 'D', new Option_Bool(false));
    oc.addDescription("demo", "GUI Only", "Restart the simulation after ending (demo mode)");

    oc.doRegister("disable-textures", 'T', new Option_Bool(false));
    oc.addDescription("disable-textures", "GUI Only", "Do not load background pictures");

    oc.doRegister("window-size", new Option_String());
    oc.addDescription("window-size", "GUI Only", "Create initial window with the given x,y size");

    oc.doRegister("window-pos", new Option_String());
    oc.addDescription("window-pos", "GUI Only", "Create initial window at the given x,y position");

#ifdef HAVE_OSG
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
    OutputDevice::createDeviceByOption("battery-output", "battery-export");
    OutputDevice::createDeviceByOption("full-output", "full-export", "full_file.xsd");
    OutputDevice::createDeviceByOption("queue-output", "queue-export", "queue_file.xsd");
    OutputDevice::createDeviceByOption("amitran-output", "trajectories", "amitran/trajectories.xsd\" timeStepSize=\"" + toString(STEPS2MS(DELTA_T)));

    //OutputDevice::createDeviceByOption("vtk-output", "vtk-export");
    OutputDevice::createDeviceByOption("link-output", "link-output");
    OutputDevice::createDeviceByOption("bt-output", "bt-output");
    OutputDevice::createDeviceByOption("lanechange-output", "lanechanges");
    OutputDevice::createDeviceByOption("stop-output", "stops");

#ifdef _DEBUG
    OutputDevice::createDeviceByOption("movereminder-output", "movereminder-output");
#endif

    MSDevice_Vehroutes::init();
    MSStopOut::init();
}


bool
MSFrame::checkOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    bool ok = true;
    if (!oc.isSet("net-file")) {
        WRITE_ERROR("No network file (-n) specified.");
        ok = false;
    }
    if (oc.getFloat("scale") < 0.) {
        WRITE_ERROR("Invalid scaling factor.");
        ok = false;
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
    if (oc.getBool("demo") && oc.isDefault("start")) {
        oc.set("start", "true");
    }
    if (oc.getBool("demo") && oc.getBool("quit-on-end")) {
        WRITE_ERROR("You can either restart or quit on end.");
        ok = false;
    }
    if (oc.getBool("meso-junction-control.limited") && !oc.getBool("meso-junction-control")) {
        oc.set("meso-junction-control", "true");
    }
    const SUMOTime begin = string2time(oc.getString("begin"));
    const SUMOTime end = string2time(oc.getString("end"));
    if (begin < 0) {
        WRITE_ERROR("The begin time should not be negative.");
        ok = false;
    }
    if (end != string2time("-1")) {
        if (end < begin) {
            WRITE_ERROR("The end time should be after the begin time.");
            ok = false;
        }
    }
    if (string2time(oc.getString("step-length")) <= 0) {
        WRITE_ERROR("the minimum step-length is 0.001");
        ok = false;
    }
#ifdef _DEBUG
    if (oc.isSet("movereminder-output.vehicles") && !oc.isSet("movereminder-output")) {
        WRITE_ERROR("option movereminder-output.vehicles requires option movereminder-output to be set");
        ok = false;
    }
#endif
    if (oc.getBool("sloppy-insert")) {
        WRITE_WARNING("The option 'sloppy-insert' is deprecated, because it is now activated by default, see the new option 'eager-insert'.");
    }
    if (string2time(oc.getString("lanechange.duration")) > 0 && oc.getFloat("lateral-resolution") > 0) {
        WRITE_ERROR("Only one of the options 'lanechange.duration' or 'lateral-resolution' may be given.");
    }
    if (oc.getBool("ignore-accidents")) {
        WRITE_WARNING("The option 'ignore-accidents' is deprecated. Use 'collision.action none' instead.");
    }
    if (oc.getBool("duration-log.statistics") && oc.isDefault("verbose")) {
        oc.set("verbose", "true");
    }
    if (oc.isDefault("precision") && string2time(oc.getString("step-length")) < 10) {
        oc.set("precision", "3");
    }
    if (oc.getInt("precision") > 2) {
        if (oc.isDefault("netstate-dump.precision")) {
            oc.set("netstate-dump.precision", toString(oc.getInt("precision")));
        }
        if (oc.isDefault("emission-output.precision")) {
            oc.set("emission-output.precision", toString(oc.getInt("precision")));
        }
        if (oc.isDefault("battery-output.precision")) {
            oc.set("battery-output.precision", toString(oc.getInt("precision")));
        }
    }
    ok &= MSDevice::checkOptions(oc);
    ok &= SystemFrame::checkOptions();
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
    MSGlobals::gLateralResolution = oc.getFloat("lateral-resolution");
    MSGlobals::gStateLoaded = oc.isSet("load-state");
    MSGlobals::gUseMesoSim = oc.getBool("mesosim");
    MSGlobals::gMesoLimitedJunctionControl = oc.getBool("meso-junction-control.limited");
    MSGlobals::gMesoOvertaking = oc.getBool("meso-overtaking");
    MSGlobals::gMesoTLSPenalty = oc.getFloat("meso-tls-penalty");
    MSGlobals::gMesoMinorPenalty = string2time(oc.getString("meso-minor-penalty"));
    MSGlobals::gSemiImplicitEulerUpdate = !oc.getBool("step-method.ballistic");
    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gUsingInternalLanes = false;
    }
    MSGlobals::gWaitingTimeMemory = string2time(oc.getString("waiting-time-memory"));
    MSAbstractLaneChangeModel::initGlobalOptions(oc);
    MSLane::initCollisionOptions(oc);

    DELTA_T = string2time(oc.getString("step-length"));
#ifdef _DEBUG
    if (oc.isSet("movereminder-output")) {
        MSBaseVehicle::initMoveReminderOutput(oc);
    }
#endif
}



/****************************************************************************/

