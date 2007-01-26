//---------------------------------------------------------------------------//
//                        SUMOFrame.cpp -
//  Some helping methods for usage within sumo and sumo-gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log: SUMOFrame.cpp,v $
// Revision 1.61  2006/12/22 12:08:11  dkrajzew
// made c2c defaults variable
//
// Revision 1.60  2006/12/12 12:04:11  dkrajzew
// made the base value for incremental dua changeable
//
// Revision 1.59  2006/12/06 16:55:21  ericnicolay
// added new file option cellphone_dump
//
// Revision 1.58  2006/11/30 07:43:35  dkrajzew
// added the inc-dua option in order to increase dua-computation
//
// Revision 1.57  2006/11/27 14:08:53  dkrajzew
// added Danilot's current changes
//
// Revision 1.55  2006/11/24 10:34:59  dkrajzew
// added Eric Nicolay's current code
//
// Revision 1.54  2006/11/14 06:49:31  dkrajzew
// first steps towards car2car-based rerouting
//
// Revision 1.52  2006/11/02 11:44:51  dkrajzew
// added Danilo Teta-Boyom's changes to car2car-communication
//
// Revision 1.51  2006/10/19 11:11:14  ericnicolay
// change code for ss2-sql-output
//
// Revision 1.50  2006/10/12 10:14:30  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
// Revision 1.49  2006/09/18 11:49:05  dkrajzew
// code beautifying
//
// Revision 1.48  2006/09/18 10:16:22  dkrajzew
// debugging; new c2c and mobile phone options added
//
// Revision 1.46  2006/08/01 07:19:57  dkrajzew
// removed build number information
//
// Revision 1.45  2006/07/10 09:04:23  dkrajzew
// dump-begin/dump-end renamed to dump-begins/dump-ends
//
// Revision 1.44  2006/05/18 13:09:14  awegener
// *** empty log message ***
//
// Revision 1.43  2006/05/15 05:58:08  dkrajzew
// debugging saving/loading of states
//
// Revision 1.43  2006/05/08 11:15:30  dkrajzew
// debugging loading/saving of states
//
// Revision 1.42  2006/04/07 10:41:48  dkrajzew
// code beautifying: embedding string in strings removed
//
// Revision 1.41  2006/03/17 09:15:11  dkrajzew
// changed the Event-interface (execute now gets the current simulation time, event handlers are non-static)
//
// Revision 1.40  2006/03/16 15:22:24  ericnicolay
// add param for the ss2-interface
//
// Revision 1.39  2006/02/23 11:33:26  dkrajzew
// TO SS2 output added
//
// Revision 1.38  2006/02/13 07:27:59  dkrajzew
// code beautifying
//
// Revision 1.37  2006/01/09 13:30:06  dkrajzew
// debugging
//
// Revision 1.36  2005/11/29 13:34:06  dkrajzew
// added a minimum simulation speed definition before the simulation ends (unfinished)
//
// Revision 1.35  2005/11/09 06:44:01  dkrajzew
// debugging
//
// Revision 1.34  2005/10/07 11:42:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.33  2005/09/23 06:05:08  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.32  2005/09/15 12:06:04  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.31  2005/07/12 12:06:12  dkrajzew
// first devices (mobile phones) added
//
// Revision 1.30  2005/05/04 08:58:32  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.29  2005/02/17 10:33:40  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.28  2005/02/01 10:07:27  dkrajzew
// performance computation added
//
// Revision 1.27  2005/01/27 14:32:36  dkrajzew
// patched undefined state of teleporter if a negative number was given
//
// Revision 1.26  2004/12/20 13:15:59  dkrajzew
// options output corrected
//
// Revision 1.25  2004/12/20 10:48:36  dkrajzew
// net-files changed to net-file
//
// Revision 1.24  2004/12/16 12:23:30  dkrajzew
// first steps towards a better parametrisation of traffic lights
//
// Revision 1.23  2004/11/25 16:26:50  dkrajzew
// consolidated and debugged some detectors and their usage
//
// Revision 1.22  2004/11/23 10:27:27  dkrajzew
// debugging
//
// Revision 1.21  2004/08/02 12:48:13  dkrajzew
// using OutputDevices instead of ostreams; first steps towards a lane-change API
//
// Revision 1.20  2004/07/02 09:41:32  dkrajzew
// debugging the repeated setting of a value
//
// Revision 1.19  2004/04/02 11:27:36  dkrajzew
// simulation-wide output files are now handled by MSNet directly
//
// Revision 1.18  2004/03/19 13:04:32  dkrajzew
// tripstate-output is not supported in 0.8
//
// Revision 1.17  2004/02/16 13:44:27  dkrajzew
// dump output generating function renamed in order to add
//  vehicle dump ability in the future
//
// Revision 1.16  2004/01/26 07:09:33  dkrajzew
// added the possibility to place lsa-detectors at a default
//  position/using a default length
//
// Revision 1.15  2003/12/12 12:31:31  dkrajzew
// continuing on accidents is now meant to be the default behaviour
//
// Revision 1.14  2003/12/04 13:16:34  dkrajzew
// handling of internal links added
//
// Revision 1.13  2003/10/28 08:33:44  dkrajzew
// random number specification options added
//
// Revision 1.12  2003/08/20 11:50:54  dkrajzew
// option for suppressing output of empty edges within the raw-output added
//
// Revision 1.11  2003/07/07 08:38:14  dkrajzew
// no configuration is loaded as default any more
//
// Revision 1.10  2003/06/24 14:38:46  dkrajzew
// false instantiation of option "log-file" as Option_String patched into
//  Option_FileName patched
//
// Revision 1.9  2003/06/24 08:09:29  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.8  2003/06/19 11:01:14  dkrajzew
// the simulation has default begin and end times now
//
// Revision 1.7  2003/06/18 11:21:10  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.6  2003/05/20 09:54:45  dkrajzew
// configuration files are no longer set as default
//
// Revision 1.5  2003/03/31 06:13:28  dkrajzew
// junction-options removed; questionmark added (same as --help)
//
// Revision 1.4  2003/03/20 16:45:25  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/02/07 11:19:37  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <iostream>
#include <fstream>
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice_File.h>
#include <microsim/MSJunction.h>
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <utils/common/RandHelper.h>
#include "SUMOFrame.h"
#include <utils/common/SystemFrame.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
SUMOFrame::fillOptions(OptionsCont &oc)
{
    // give some application descriptions
    oc.setApplicationDescription("A microscopic road traffic simulation.");
#ifdef WIN32
    oc.setApplicationName("sumo.exe");
#else
    oc.setApplicationName("sumo");
#endif
    oc.addCallExample("-b 0 -e 1000 -n net.xml -r routes.xml");
    oc.addCallExample("-c munich_config.cfg");
    oc.addCallExample("--spider-net [spider-network opts] -o <OUTPUTFILE>");
    oc.addCallExample("--help");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Time");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("TLS Defaults");
    oc.addOptionSubTopic("C2C");
    oc.addOptionSubTopic("Cellular");
    oc.addOptionSubTopic("Report");


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

    oc.doRegister("load-state", new Option_FileName());//!!! check, describe
    oc.addDescription("load-state", "Input", "Loads a network state from FILE");


        // register output options
    oc.doRegister("netstate-dump", new Option_FileName());
    oc.addSynonyme("netstate-dump", "ndump");
    oc.addSynonyme("netstate-dump", "netstate");
    oc.addDescription("netstate-dump", "Output", "Save complete network states into FILE");

    oc.doRegister("emissions-output", new Option_FileName());
    oc.addSynonyme("emissions-output", "emissions");
    oc.addDescription("emissions-output", "Output", "Save aggregated vehicle emission inf. into FILE");

    oc.doRegister("tripinfo-output", new Option_FileName());
    oc.addSynonyme("tripinfo-output", "tripinfo");
    oc.addDescription("tripinfo-output", "Output", "Save single vehicle trip inf. into FILE");

    oc.doRegister("vehroute-output", new Option_FileName());
    oc.addSynonyme("vehroute-output", "vehroutes");
    oc.addDescription("vehroute-output", "Output", "Save single vehicle route inf. into FILE");

    oc.doRegister("dump-intervals", new Option_IntVector(""));
    oc.addDescription("dump-intervals", "Output", "Build edge-based network dumps for given intervals");
    oc.doRegister("dump-basename", new Option_FileName());
    oc.addDescription("dump-basename", "Output", "Save edge-based network dumps with FILE as prefix");

    oc.doRegister("lanedump-intervals", new Option_IntVector(""));
    oc.addDescription("lanedump-intervals", "Output", "Build lane-based network dumps for given intervals");
    oc.doRegister("lanedump-basename", new Option_FileName());
    oc.addDescription("lanedump-basename", "Output", "Save lane-based network dumps with FILE as prefix");

    oc.doRegister("dump-empty-edges", new Option_Bool(false));
    oc.addDescription("dump-empty-edges", "Output", "Write also empty edges completely when dumping");

    oc.doRegister("dump-begins", new Option_IntVector(""));
    oc.addDescription("dump-begins", "Output", "Use INT[] as times at which a dump must begin in order to be written");
    oc.doRegister("dump-ends", new Option_IntVector(""));
    oc.addDescription("dump-ends", "Output", "Use INT[] as times at which a dump must end in order to be written");

    oc.doRegister("save-state.times", new Option_IntVector(""));//!!! check, describe
    oc.addDescription("save-state.times", "Output", "Use INT[] as times at which a network state written");
    oc.doRegister("save-state.prefix", new Option_FileName());//!!! check, describe
    oc.addDescription("save-state.prefix", "Output", "Prefix for network states");

    
        // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.addDescription("begin", "Time", "Defines the begin time; The simulation starts at this time");
    
    oc.doRegister("end", 'e', new Option_Integer(86400));
    oc.addDescription("end", "Time", "Defines the end time; The simulation ends at this time");


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

    oc.doRegister("incremental-dua-step", new Option_Integer(-1));//!!! check, describe
    oc.addDescription("incremental-dua-step", "Processing", "Perform the simulation as a step in incremental DUA");
    oc.doRegister("incremental-dua-base", new Option_Integer(10));//!!! check, describe
    oc.addDescription("incremental-dua-base", "Processing", "Base value for incremental DUA");

    oc.doRegister("time-to-teleport", new Option_Integer(300));
    oc.addDescription("time-to-teleport", "Processing", "Specify how long a vehicle may wait until being teleported");

    oc.doRegister("lc-teleport.min-dist", new Option_Float(100));//!!! check, describe
    oc.addDescription("lc-teleport.min-dist", "Processing", "");
    oc.doRegister("lc-teleport.veh-maxv", new Option_Float(-1/*20.0/3.6*/));//!!! check, describe
    oc.addDescription("lc-teleport.veh-maxv", "Processing", "");
    oc.doRegister("lc-teleport.lane-min-vmax", new Option_Float((SUMOReal) (80.0/3.6)));//!!! check, describe
    oc.addDescription("lc-teleport.lane-min-vmax", "Processing", "");

    oc.doRegister("default-lanechange-model", new Option_String("dk1"));//!!! check, describe
    oc.addDescription("default-lanechange-model", "Processing", "");

    oc.doRegister("no-duration-log", new Option_Bool(false));//!!! check, describe
    oc.addDescription("no-duration-log", "Processing", "");


    // devices
        // c2x
    oc.doRegister("device.c2x.probability", new Option_Float(0.));//!!! describe
    oc.addDescription("device.c2x.probability", "C2C", "The probability for a vehicle to have c2c");
    
    oc.doRegister("device.c2x.knownveh", new Option_String());//!!! describe
    oc.addDescription("device.c2x.knownveh", "C2C", "Assign a device to named vehicles");
    
    oc.doRegister("device.c2x.range", new Option_Float(100.));//!!! describe
    oc.addDescription("device.c2x.range", "C2C", "The range of the c2c device");
    
    oc.doRegister("device.c2x.keep-duration", new Option_Integer(30 * 60));//!!! describe
    oc.addDescription("device.c2x.keep-duration", "C2C", "Duration of keeping messages");
    
    oc.doRegister("device.c2x.insert-info-factor", new Option_Float((SUMOReal) 1.2));//!!! describe
    oc.addDescription("device.c2x.insert-info-factor", "C2C", "Factor for adding messages");
    
    oc.doRegister("c2x.cluster-info", new Option_FileName());//!!! describe
    oc.addDescription("c2x.cluster-info", "C2C", "Save cluster information into FILE");
    
    oc.doRegister("c2x.edge-near-info", new Option_FileName());//!!! describe
    oc.addDescription("c2x.edge-near-info", "C2C", "Save 'connected' edges into FILE");
    
    oc.doRegister("c2x.saved-info", new Option_FileName());//!!! describe
    oc.addDescription("c2x.saved-info", "C2C", "");
	
    oc.doRegister("c2x.saved-info-freq", new Option_FileName());//!!! describe
    oc.addDescription("c2x.saved-info-freq", "C2C", "");
    
    oc.doRegister("c2x.transmitted-info", new Option_FileName());//!!! describe
    oc.addDescription("c2x.transmitted-info", "C2C", "Save transmitted information into FILE");
    
    oc.doRegister("c2x.vehicle-in-range", new Option_FileName());//!!! describe
    oc.addDescription("c2x.vehicle-in-range", "C2C", "Save names of connected vehicles into FILE");


        // cell-phones
    oc.doRegister("ss2-cellload-file", new Option_FileName());//!!! check, describe
    oc.addDescription("ss2-cellload-file", "Cellular", "");
    
    oc.doRegister("ss2-output", new Option_FileName());//!!! check, describe
    oc.addDescription("ss2-output", "Cellular", "");
    
    oc.doRegister("ss2-cell-output", new Option_FileName());
    oc.addDescription("ss2-cell-output", "Cellular", "");
    
    oc.doRegister("ss2-la-output", new Option_FileName());
    oc.addDescription("ss2-la-output", "Cellular", "");
    
    oc.doRegister("ss2-sql-output", new Option_FileName());//!!! check, describe
    oc.addDescription("ss2-sql-output", "Cellular", "");
    
    oc.doRegister("ss2-sql-cell-output", new Option_FileName());
    oc.addDescription("ss2-sql-cell-output", "Cellular", "");
    
    oc.doRegister("ss2-sql-la-output", new Option_FileName());
    oc.addDescription("ss2-sql-la-output", "Cellular", "");
	
    oc.doRegister("cellphone-dump", new Option_FileName());
    oc.addDescription("cellphone-dump", "Cellular", "");
    
    oc.doRegister("device.cell-phone.knownveh", new Option_String());//!!! check, describe
    oc.addDescription("device.cell-phone.knownveh", "Cellular", "");
    
    oc.doRegister("device.cell-phone.probability", new Option_Float(0.));//!!! check, describe
    oc.addDescription("device.cell-phone.probability", "Cellular", "");
    
    oc.doRegister("device.cell-phone.amount.min", new Option_Float(1.));//!!! check, describe
    oc.addDescription("device.cell-phone.amount.min", "Cellular", "");
    
    oc.doRegister("device.cell-phone.amount.max", new Option_Float(1.));//!!! check, describe
    oc.addDescription("device.cell-phone.amount.max", "Cellular", "");


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


    // debug
    oc.doRegister("track", new Option_Float(0.));//!!! check, describe

    //remote port 0 if not used
    oc.doRegister("remote-port", new Option_Integer(0));

    // add rand options
    RandHelper::insertRandOptions(oc);
}


std::vector<OutputDevice*>
SUMOFrame::buildStreams(const OptionsCont &oc)
{
    std::vector<OutputDevice*> ret(MSNet::OS_MAX, 0);
    // standard outputs
    ret[MSNet::OS_NETSTATE] = buildStream(oc, "netstate-dump");
    ret[MSNet::OS_EMISSIONS] = buildStream(oc, "emissions-output");
    ret[MSNet::OS_TRIPDURATIONS] = buildStream(oc, "tripinfo-output");
    ret[MSNet::OS_VEHROUTE] = buildStream(oc, "vehroute-output");
    // TrafficOnline-outputs
    ret[MSNet::OS_DEVICE_TO_SS2] = buildStream(oc, "ss2-output");
    ret[MSNet::OS_CELL_TO_SS2] = buildStream(oc, "ss2-cell-output");
    ret[MSNet::OS_LA_TO_SS2] = buildStream(oc, "ss2-la-output");
    ret[MSNet::OS_DEVICE_TO_SS2_SQL] = buildStream(oc, "ss2-sql-output");
    ret[MSNet::OS_CELL_TO_SS2_SQL] = buildStream(oc, "ss2-sql-cell-output");
	ret[MSNet::OS_LA_TO_SS2_SQL] = buildStream(oc, "ss2-sql-la-output");
	ret[MSNet::OS_CELLPHONE_DUMP_TO] = buildStream(oc, "cellphone-dump");
    // c2x-outputs
    ret[MSNet::OS_CLUSTER_INFO] = buildStream(oc, "c2x.cluster-info");
    ret[MSNet::OS_EDGE_NEAR] = buildStream(oc, "c2x.edge-near-info");
    ret[MSNet::OS_SAVED_INFO] = buildStream(oc, "c2x.saved-info");
    ret[MSNet::OS_SAVED_INFO_FREQ] = buildStream(oc, "c2x.saved-info-freq");
    ret[MSNet::OS_TRANS_INFO] = buildStream(oc, "c2x.transmitted-info");
    ret[MSNet::OS_VEH_IN_RANGE] = buildStream(oc, "c2x.vehicle-in-range");

    // initialise TrafficOnline-outputs
    if( ret[MSNet::OS_DEVICE_TO_SS2_SQL]!=0 ){
        (ret[MSNet::OS_DEVICE_TO_SS2_SQL])->getOStream()
		    << "DROP TABLE IF EXISTS `COLLECTORPOS`;\n"
            << "CREATE TABLE `COLLECTORPOS` (\n"
            << "`ID` int(11) NOT NULL auto_increment,\n"
            << "`TID` varchar(20) NOT NULL default '',\n"
            << "`DATE_TIME` datetime default NULL,\n"
			<< "`POSITIONS_ID` int(5) NOT NULL default '0',\n"
			<< "`CALL_ID` int(5) NOT NULL default '0',\n"
			<< "`QUALITY_ID` int(5) NOT NULL default '30',\n"
            << "PRIMARY KEY  (`ID`)\n"
            << ") ENGINE=MyISAM DEFAULT CHARSET=latin1;\n\n";
        (ret[MSNet::OS_DEVICE_TO_SS2_SQL])->getOStream()
		    << "INSERT INTO `COLLECTORPOS` (`ID`,`TID`,`DATE_TIME`, `POSITIONS_ID`, `CALL_ID`, `QUALITY_ID`) VALUES "
            << endl;
    }
    if( ret[MSNet::OS_CELL_TO_SS2_SQL]!=0 ){
        (ret[MSNet::OS_CELL_TO_SS2_SQL])->getOStream()
            << "DROP TABLE IF EXISTS `COLLECTORCS`;\n"
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
            << ") ENGINE=MyISAM DEFAULT CHARSET=latin1;\n\n";

        (ret[MSNet::OS_CELL_TO_SS2_SQL])->getOStream()
            << "INSERT INTO `COLLECTORCS` (`ID`,`TID`,`DATE_TIME`,`CELL_ID`,`STAT_CALLS_IN`,`STAT_CALLS_OUT`,"
            << "`DYN_CALLS_IN`,`DYN_CALLS_OUT`,`SUM_CALLS`,`INTERVALL`) VALUES \n";
    }
    if( ret[MSNet::OS_LA_TO_SS2_SQL]!=0 ){
        (ret[MSNet::OS_LA_TO_SS2_SQL])->getOStream()
            << "DROP TABLE IF EXISTS `COLLECTORLA`;\n"
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
            << ") ENGINE=MyISAM DEFAULT CHARSET=latin1;\n\n";

        (ret[MSNet::OS_LA_TO_SS2_SQL])->getOStream()
            << "INSERT INTO `COLLECTORLA` (`ID`,`TID`,`DATE_TIME`,`POSITION_ID`,`DIR`,`SUM_CHANGE`,"
            << "`QUALITY_ID`,`INTERVALL`) VALUES \n";
    }
    return ret;
}


OutputDevice *
SUMOFrame::buildStream(const OptionsCont &oc,
                       const std::string &optionName)
{
    if(!oc.isSet(optionName)) {
        return 0;
    }
    ofstream *ret = new ofstream(oc.getString(optionName).c_str(), ios::out|ios::trunc);
    if(!ret->good()) {
        MsgHandler::getErrorInstance()->inform("The output file '" + oc.getString(optionName) + "' could not be built.\n (Used for '" + optionName + "').");
        throw ProcessError();
    }
    return new OutputDevice_File(ret);
}


bool
SUMOFrame::checkOptions(OptionsCont &oc)
{
    bool ok = true;
    try {
        oc.resetWritable();
        // check the existance of a name for simulation file
        if(!oc.isSet("n")) {
            MsgHandler::getErrorInstance()->inform("No network file (-n) specified.");
            ok = false;
        }
        // check if the begin and the end of the simulation are supplied
        if(!oc.isSet("b")) {
            MsgHandler::getErrorInstance()->inform("The begin of the simulation (-b) is not specified.");
            ok = false;
        }
        if(!oc.isSet("e")) {
            MsgHandler::getErrorInstance()->inform("The end of the simulation (-e) is not specified.");
            ok = false;
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
        return false;
    }
    //
    if(oc.isSet("default-lanechange-model")) {
        string t = oc.getString("default-lanechange-model");
        if(t!="dk1") {
            MsgHandler::getErrorInstance()->inform("Unknown lane change model");
            ok = false;
        }
    }
    return ok;
}


void
SUMOFrame::setMSGlobals(OptionsCont &oc)
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
    // set the vehicle teleport on false lane options
    MSGlobals::gMinLaneVMax4FalseLaneTeleport = oc.getFloat("lc-teleport.lane-min-vmax");
    MSGlobals::gMaxVehV4FalseLaneTeleport = oc.getFloat("lc-teleport.veh-maxv");
    MSGlobals::gMinVehDist4FalseLaneTeleport = oc.getFloat("lc-teleport.min-dist");
    //
    MSGlobals::gCheck4Accidents = oc.getBool("check-accidents");
    MSGlobals::gStateLoaded = oc.isSet("load-state");
    //
    MSGlobals::gUsingC2C = oc.getFloat("device.c2x.probability")!=0||oc.isSet("device.c2x.knownveh");
    MSGlobals::gLANRange = oc.getFloat("device.c2x.range");
    MSGlobals::gLANRefuseOldInfosOffset = oc.getInt("device.c2x.keep-duration");
    MSGlobals::gAddInfoFactor = oc.getFloat("device.c2x.insert-info-factor");
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


