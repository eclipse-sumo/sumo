/****************************************************************************/
/// @file    MSFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for microsim; inits global outputs and settings
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <microsim/devices/MSDevice_C2C.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSDevice_CPhone.h>
#include <utils/common/RandHelper.h>
#include "MSFrame.h"
#include <utils/common/SystemFrame.h>

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
void
MSFrame::fillOptions()
{
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
    oc.addOptionSubTopic("Report");
#ifndef NO_TRACI
    oc.addOptionSubTopic("Traffic Control Interface (TraCI) Server");
#endif // TRACI
#ifdef HAVE_MESOSIM
    oc.addOptionSubTopic("Mesoscopic");
#endif


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

#ifdef HAVE_MESOSIM
    oc.doRegister("load-state", new Option_FileName());//!!! check, describe
    oc.addDescription("load-state", "Input", "Loads a network state from FILE");
    oc.doRegister("load-state.offset", new Option_Float());//!!! check, describe
    oc.addDescription("load-state.offset", "Input", "Sets the time offset for vehicle segment exit times.");
#endif

    // register output options
    oc.doRegister("netstate-dump", new Option_FileName());
    oc.addSynonyme("netstate-dump", "ndump");
    oc.addSynonyme("netstate-dump", "netstate");
    oc.addDescription("netstate-dump", "Output", "Save complete network states into FILE");
    oc.doRegister("dump-empty-edges", new Option_Bool(false));
    oc.addDescription("dump-empty-edges", "Output", "Write also empty edges completely when dumping");

    oc.doRegister("emissions-output", new Option_FileName());
    oc.addSynonyme("emissions-output", "emissions");
    oc.addDescription("emissions-output", "Output", "Save aggregated vehicle emission inf. into FILE");

    oc.doRegister("tripinfo-output", new Option_FileName());
    oc.addSynonyme("tripinfo-output", "tripinfo");
    oc.addDescription("tripinfo-output", "Output", "Save single vehicle trip inf. into FILE");

    oc.doRegister("vehroute-output", new Option_FileName());
    oc.addSynonyme("vehroute-output", "vehroutes");
    oc.addDescription("vehroute-output", "Output", "Save single vehicle route inf. into FILE");

    oc.doRegister("dump-intervals", new Option_IntVector(IntVector()));
    oc.addDescription("dump-intervals", "Output", "Build edge-based network dumps for given intervals");
    oc.doRegister("dump-basename", new Option_FileName());
    oc.addDescription("dump-basename", "Output", "Save edge-based network dumps with FILE as prefix");
    oc.doRegister("lanedump-intervals", new Option_IntVector(IntVector()));
    oc.addDescription("lanedump-intervals", "Output", "Build lane-based network dumps for given intervals");
    oc.doRegister("lanedump-basename", new Option_FileName());
    oc.addDescription("lanedump-basename", "Output", "Save lane-based network dumps with FILE as prefix");
    oc.doRegister("exclude-empty-edges", new Option_Bool(false));
    oc.addDescription("exclude-empty-edges", "Output", "Do not write information about empty edges");
    oc.doRegister("exclude-empty-lanes", new Option_Bool(false));
    oc.addDescription("exclude-empty-lanes", "Output", "Do not write information about empty lanes");
    oc.doRegister("dump-begins", new Option_IntVector(IntVector()));
    oc.addDescription("dump-begins", "Output", "Use INT[] as times at which a dump must begin in order to be written");
    oc.doRegister("dump-ends", new Option_IntVector(IntVector()));
    oc.addDescription("dump-ends", "Output", "Use INT[] as times at which a dump must end in order to be written");

    oc.doRegister("physical-states-output", new Option_FileName());
    oc.addDescription("physical-states-output", "Output", "Write vehicle states");

    oc.doRegister("lanechange-output", new Option_FileName());
    oc.addSynonyme("lanechange-output", "lanechanges");
    oc.addDescription("lanechange-output", "Output", "Write lane change information");

#ifdef HAVE_MESOSIM
    oc.doRegister("save-state.times", new Option_IntVector(IntVector()));//!!! check, describe
    oc.addDescription("save-state.times", "Output", "Use INT[] as times at which a network state written");
    oc.doRegister("save-state.prefix", new Option_FileName());//!!! check, describe
    oc.addDescription("save-state.prefix", "Output", "Prefix for network states");
#endif

    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.addDescription("begin", "Time", "Defines the begin time; The simulation starts at this time");

    oc.doRegister("end", 'e', new Option_Integer(86400));
    oc.addDescription("end", "Time", "Defines the end time; The simulation ends at this time");

#ifdef HAVE_SUBSECOND_TIMESTEPS
#ifdef HAVE_VARIABLE_SUBSECOND_TIMESTEPS
    oc.doRegister("step-length", new Option_Float((SUMOReal) .1));
    oc.addDescription("step-length", "Time", "Defines the step duration");
#endif
#endif


    // register the processing options
    oc.doRegister("route-steps", 's', new Option_Integer(200));
    oc.addDescription("route-steps", "Processing", "Load routes for the next INT steps ahead");

    oc.doRegister("use-internal-links", 'I', new Option_Bool(false));//!!! check, describe
    oc.addDescription("use-internal-links", "Processing", "Enable internal links (must be in the network)");

    oc.doRegister("quit-on-accident", new Option_Bool(false));
    oc.addDescription("quit-on-accident", "Processing", "Quit (with an error) if an accident occures");

    oc.doRegister("check-accidents", new Option_Bool(false));
    oc.addDescription("check-accidents", "Processing", "Check whether accidents occure more deeply");

    oc.doRegister("too-slow-rtf", new Option_Float(-1));//!!! check, describe
    oc.addDescription("too-slow-rtf", "Processing", "Quit simulation if the rtf gets too small");

    oc.doRegister("too-many-vehicles", new Option_Integer(-1));//!!! check, describe
    oc.addDescription("too-many-vehicles", "Processing", "Quit simulation if this number of vehicles is exceeded");

    oc.doRegister("incremental-dua-step", new Option_Integer(-1));//!!! check, describe
    oc.addDescription("incremental-dua-step", "Processing", "Perform the simulation as a step in incremental DUA");
    oc.doRegister("incremental-dua-base", new Option_Integer(10));//!!! check, describe
    oc.addDescription("incremental-dua-base", "Processing", "Base value for incremental DUA");

    oc.doRegister("time-to-teleport", new Option_Integer(300));
    oc.addDescription("time-to-teleport", "Processing", "Specify how long a vehicle may wait until being teleported");

    oc.doRegister("lanechange.min-sight", new Option_Float());//!!! check, describe
    oc.addDescription("lanechange.min-sight", "Processing", "");
    oc.doRegister("lanechange.min-sight-edges", new Option_Integer(8));//!!! check, describe
    oc.addDescription("lanechange.min-sight-edges", "Processing", "");

    oc.doRegister("no-duration-log", new Option_Bool(false));
    oc.addDescription("no-duration-log", "Processing", "Disable performance reports for individual simulation steps");

    oc.doRegister("no-step-log", new Option_Bool(false));
    oc.addDescription("no-step-log", "Processing", "Disable console output of current simulation step");

    oc.doRegister("max-depart-delay", new Option_Integer(-1));
    oc.addDescription("max-depart-delay", "Processing", "How long vehicles wait for departure before being skipped");


    // devices
    MSDevice_C2C::insertOptions();
    MSDevice_Routing::insertOptions();
    MSDevice_CPhone::insertOptions();


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
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.addDescription("verbose", "Report", "Switches to verbose output");

    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.addDescription("suppress-warnings", "Report", "Disables output of warnings");

    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.addDescription("print-options", "Report", "Prints option values before processing");

    oc.doRegister("help", '?', new Option_Bool(false));
    oc.addDescription("help", "Report", "Prints this screen");

    oc.doRegister("log-file", 'l', new Option_FileName());
    oc.addDescription("log-file", "Report", "Writes all messages to FILE");


    // TraCI server
#ifndef NO_TRACI
    //remote port 0 if not used
    oc.doRegister("remote-port", new Option_Integer(0));
    oc.addDescription("remote-port", "Traffic Control Interface (TraCI) Server", "Enables TraCI Server if set");
    oc.doRegister("penetration", new Option_Float(1.0f));
    oc.addDescription("penetration", "Traffic Control Interface (TraCI) Server", "Value in 0..1 [default: 1]");
#endif
    //
#ifdef HAVE_MESOSIM
    oc.doRegister("mesosim", new Option_Bool(false));
    oc.addDescription("mesosim", "Mesoscopic", "Enables mesoscopic simulation");
    oc.doRegister("mesosim-edgelength", new Option_Float(98.0f));
    oc.addDescription("mesosim-edgelength", "Mesoscopic", "Length of an edge segment in mesoscopic simulation");
    oc.doRegister("meso-lanescale", new Option_Float(1.0f));
    oc.addDescription("meso-lanescale", "Mesoscopic", "Factor for calculating number of lanes");
#endif

    // add rand options
    RandHelper::insertRandOptions();
}


void
MSFrame::buildStreams()
{
    // standard outputs
    OutputDevice::createDeviceByOption("netstate-dump", "sumo-netstate");
    OutputDevice::createDeviceByOption("emissions-output", "emissions");
    OutputDevice::createDeviceByOption("tripinfo-output", "tripinfos");
    OutputDevice::createDeviceByOption("vehroute-output", "routes");
    OutputDevice::createDeviceByOption("physical-states-output", "physical-states");
    // TrafficOnline-outputs
    OutputDevice::createDeviceByOption("ss2-output");
    OutputDevice::createDeviceByOption("ss2-cell-output");
    OutputDevice::createDeviceByOption("ss2-la-output");
    OutputDevice::createDeviceByOption("ss2-sql-output");
    OutputDevice::createDeviceByOption("ss2-sql-cell-output");
    OutputDevice::createDeviceByOption("ss2-sql-la-output");
    OutputDevice::createDeviceByOption("cellphone-dump");
    // c2x-outputs
    OutputDevice::createDeviceByOption("c2x.cluster-info", "clusterInfos");
    OutputDevice::createDeviceByOption("c2x.saved-info", "savedInfos");
    OutputDevice::createDeviceByOption("c2x.saved-info-freq", "savedInfosFreq");
    OutputDevice::createDeviceByOption("c2x.transmitted-info", "transmittedInfos");
    OutputDevice::createDeviceByOption("c2x.vehicle-in-range", "vehicleInRanges");

    // initialise TrafficOnline-outputs
    if (OptionsCont::getOptions().isSet("ss2-sql-output")) {
        OutputDevice::getDeviceByOption("ss2-sql-output")
        << "CREATE TABLE `COLLECTORPOS` (\n"
        << "`ID` int(11) NOT NULL auto_increment,\n"
        << "`TID` varchar(20) NOT NULL default '',\n"
        << "`DATE_TIME` datetime default NULL,\n"
        << "`POSITION_ID` int(5) NOT NULL default '0',\n"
        << "`CALL_ID` int(5) NOT NULL default '0',\n"
        << "`QUALITY_ID` int(5) NOT NULL default '30',\n"
        << "PRIMARY KEY  (`ID`)\n"
        << ") ENGINE=MyISAM DEFAULT CHARSET=latin1;\n\n"

        << "INSERT INTO `COLLECTORPOS` (`ID`,`TID`,`DATE_TIME`, `POSITION_ID`, `CALL_ID`, `QUALITY_ID`) VALUES "
        << "\n";
    }
    if (OptionsCont::getOptions().isSet("ss2-sql-cell-output")) {
        OutputDevice::getDeviceByOption("ss2-sql-cell-output")
        << "CREATE TABLE `COLLECTORCS` ("
        << "`ID` int(11) NOT NULL auto_increment,\n"
        << "`TID` varchar(20) NOT NULL default '',\n"
        << "`DATE_TIME` datetime default NULL,\n"
        << "`CELL_ID` int(5) NOT NULL default '0',\n"
        << "`STAT_CALLS_IN` int(5) NOT NULL default '0',\n"
        << "`STAT_CALLS_OUT` int(5) NOT NULL default '0',\n"
        << "`DYN_CALLS_IN` int(5) NOT NULL default '0',\n"
        << "`DYN_CALLS_OUT` int(5) NOT NULL default '0',\n"
        << "`SUM_CALLS` int(5) NOT NULL default '0',\n"
        << "`INTERVALL` int(5) NOT NULL default '0',\n"
        << "PRIMARY KEY  (`ID`,`TID`)\n"
        << ") ENGINE=MyISAM DEFAULT CHARSET=latin1;\n\n"

        << "INSERT INTO `COLLECTORCS` (`ID`,`TID`,`DATE_TIME`,`CELL_ID`,`STAT_CALLS_IN`,`STAT_CALLS_OUT`,"
        << "`DYN_CALLS_IN`,`DYN_CALLS_OUT`,`SUM_CALLS`,`INTERVALL`) VALUES \n";
    }
    if (OptionsCont::getOptions().isSet("ss2-sql-la-output")) {
        OutputDevice::getDeviceByOption("ss2-sql-la-output")
        << "CREATE TABLE `COLLECTORLA` (\n"
        << "`ID` int(11) NOT NULL auto_increment,\n"
        << "`TID` varchar(20) NOT NULL default '',\n"
        << "`DATE_TIME` datetime default NULL,\n"
        << "`POSITION_ID` int(5) NOT NULL default '0',\n"
        << "`DIR` int(1) NOT NULL default '0',\n"
        << "`SUM_CHANGE` int(5) NOT NULL default '0',\n"
        << "`QUALITY_ID` int(2) NOT NULL default '0',\n"
        << "`INTERVALL` int(5) NOT NULL default '0',\n"
        << "PRIMARY KEY  (`ID`,`TID`)\n"
        << ") ENGINE=MyISAM DEFAULT CHARSET=latin1;\n\n"

        << "INSERT INTO `COLLECTORLA` (`ID`,`TID`,`DATE_TIME`,`POSITION_ID`,`DIR`,`SUM_CHANGE`,"
        << "`QUALITY_ID`,`INTERVALL`) VALUES \n";
    }
}


bool
MSFrame::checkOptions()
{
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
    return ok;
}


void
MSFrame::setMSGlobals(OptionsCont &oc)
{
    // pre-initialise the network
    // set whether empty edges shall be printed on dump
    MSGlobals::gOmitEmptyEdgesOnDump = !oc.getBool("dump-empty-edges");
    // set whether internal lanes shall be used
    MSGlobals::gUsingInternalLanes = oc.getBool("use-internal-links");
    // set the grid lock time
    MSGlobals::gTimeToGridlock = oc.getInt("time-to-teleport")<0
                                 ? 0
                                 : oc.getInt("time-to-teleport");
    MSGlobals::gMinLaneChangeSight = oc.isSet("lanechange.min-sight")
                                     ? oc.getFloat("lanechange.min-sight") : 3000.f;
    MSGlobals::gMinLaneChangeSightEdges = oc.getInt("lanechange.min-sight-edges");
    //
    MSGlobals::gCheck4Accidents = oc.getBool("check-accidents");
#ifdef HAVE_MESOSIM
    MSGlobals::gStateLoaded = oc.isSet("load-state");
#endif
    //
    MSGlobals::gUsingC2C = oc.getFloat("device.c2x.probability")!=0||oc.isSet("device.c2x.knownveh");
    MSGlobals::gLANRange = oc.getFloat("device.c2x.range");
    MSGlobals::gLANRefuseOldInfosOffset = (SUMOTime) oc.getInt("device.c2x.keep-duration"); // !!! SUMOTime-option
    MSGlobals::gAddInfoFactor = oc.getFloat("device.c2x.insert-info-factor");
    //
#ifdef HAVE_SUBSECOND_TIMESTEPS
#ifdef HAVE_VARIABLE_SUBSECOND_TIMESTEPS
    DELTA_T = oc.getFloat("step-length");
#endif
#endif
    //
#ifdef HAVE_MESOSIM
    MSGlobals::gUseMesoSim = oc.getBool("mesosim");
#endif
}



/****************************************************************************/

