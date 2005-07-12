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
// $Log$
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <iostream>
#include <fstream>
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/convert/ToString.h>
#include <utils/iodevices/OutputDevice_File.h>
#include <microsim/MSJunction.h>
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <utils/common/RandHelper.h>
#include <utils/common/DevHelper.h>
#include "SUMOFrame.h"


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
    // register input options
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.doRegister("route-files", 'r', new Option_FileName());
    oc.doRegister("additional-files", 'a', new Option_FileName());
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addSynonyme("route-files", "routes");
    oc.addSynonyme("additional-files", "additional");
    oc.addSynonyme("configuration-file", "configuration");
    // register output options
    oc.doRegister("netstate-dump", new Option_FileName());
    oc.addSynonyme("netstate-dump", "ndump");
    oc.addSynonyme("netstate-dump", "netstate");
    oc.doRegister("emissions-output", new Option_FileName());
    oc.addSynonyme("emissions-output", "emissions");
    oc.doRegister("tripinfo-output", new Option_FileName());
    oc.addSynonyme("tripinfo-output", "tripinfo");
    oc.doRegister("vehroute-output", new Option_FileName());
    oc.addSynonyme("vehroute-output", "vehroutes");
    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.doRegister("end", 'e', new Option_Integer(86400));
    oc.doRegister("route-steps", 's', new Option_Integer(0));
    oc.doRegister("quit-on-accident", new Option_Bool(false));
    oc.doRegister("check-accidents", new Option_Bool(false));
    // register the report options
    oc.doRegister("no-duration-log", new Option_Bool(false));
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", '?', new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());
    // register some research options
//    oc.doRegister("initial-density", new Option_Float());
//    oc.doRegister("initial-speed", new Option_Float());
    // register the data processing options
    oc.doRegister("dump-intervals", new Option_IntVector(""));
    oc.doRegister("dump-basename", new Option_FileName());
    oc.doRegister("lanedump-intervals", new Option_IntVector(""));
    oc.doRegister("lanedump-basename", new Option_FileName());
    oc.doRegister("dump-empty-edges", new Option_Bool(false));
    //
    oc.doRegister("time-to-teleport", new Option_Integer(300));

    oc.doRegister("use-internal-links", 'I', new Option_Bool(false));
    oc.doRegister("default-lanechange-model", new Option_String("dk1"));

    oc.doRegister("lc-teleport.min-dist", new Option_Float(100));
    oc.doRegister("lc-teleport.veh-maxv", new Option_Float(-1/*20.0/3.6*/));
    oc.doRegister("lc-teleport.lane-min-vmax", new Option_Float((float) (80.0/3.6)));

    // tls
    oc.doRegister("agent-tl.detector-len", new Option_Float(75));
    oc.doRegister("agent-tl.learn-horizon", new Option_Integer(3));
    oc.doRegister("agent-tl.decision-horizon", new Option_Integer(1));
    oc.doRegister("agent-tl.min-diff", new Option_Float((float) .1));
    oc.doRegister("agent-tl.tcycle", new Option_Integer(90));

    oc.doRegister("actuated-tl.detector-pos", new Option_Float(100));
    oc.doRegister("actuated-tl.max-gap", new Option_Float(3.1f));
    oc.doRegister("actuated-tl.detector-gap", new Option_Float(3.0f));
    oc.doRegister("actuated-tl.passing-time", new Option_Float(1.9f));

    // device
    oc.doRegister("device", new Option_Float(0.5));
    oc.doRegister("knownveh", new Option_String("444_4"));

    oc.doRegister("device.cell-phone.probability", new Option_Float(0.));
    oc.doRegister("device.cell-phone.amount.min", new Option_Float(1.));
    oc.doRegister("device.cell-phone.amount.max", new Option_Float(1.));


    //
#ifdef HAVE_MESOSIM
    oc.doRegister("mesosim", new Option_Bool(false));
    oc.doRegister("mesosim-edgelength", new Option_Float(98.0f));
#endif

    // add rand and dev options
    RandHelper::insertRandOptions(oc);
    DevHelper::insertDevOptions(oc);
}


std::vector<OutputDevice*>
SUMOFrame::buildStreams(const OptionsCont &oc)
{
    std::vector<OutputDevice*> ret(MSNet::OS_MAX, 0);
    ret[MSNet::OS_NETSTATE] = buildStream(oc, "netstate-dump");
    ret[MSNet::OS_EMISSIONS] = buildStream(oc, "emissions-output");
    ret[MSNet::OS_TRIPDURATIONS] = buildStream(oc, "tripinfo-output");
    ret[MSNet::OS_VEHROUTE] = buildStream(oc, "vehroute-output");
    return ret;
}


OutputDevice *
SUMOFrame::buildStream(const OptionsCont &oc,
                       const std::string &optionName)
{
    if(!oc.isSet(optionName)) {
        return 0;
    }
    ofstream *ret = new ofstream(oc.getString(optionName).c_str(),
        ios::out|ios::trunc);
    if(!ret->good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The output file '") + oc.getString(optionName)
            + string("' could not be built."));
        MsgHandler::getErrorInstance()->inform(
            string(" (Used for '") + optionName + string("')."));
        MsgHandler::getErrorInstance()->inform("Simulation failed.");
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
            MsgHandler::getErrorInstance()->inform(
                "No simulation file (-n) specified.");
            ok = false;
        }
        // check if the begin and the end of the simulation are supplied
        if(!oc.isSet("b")) {
            MsgHandler::getErrorInstance()->inform(
                "The begin of the simulation (-b) is not specified.");
            ok = false;
        }
        if(!oc.isSet("e")) {
            MsgHandler::getErrorInstance()->inform(
                "The end of the simulation (-e) is not specified.");
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
            MsgHandler::getErrorInstance()->inform(
                "Unknown lane change model");
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
    MSGlobals::gOmitEmptyEdgesOnDump =
        !oc.getBool("dump-empty-edges");
    // set whether internal lanes shall be used
    MSGlobals::gUsingInternalLanes =
        oc.getBool("use-internal-links");
    // set the grid lock time
    MSGlobals::gTimeToGridlock =
        oc.getInt("time-to-teleport")<0
        ? 0
        : oc.getInt("time-to-teleport");
    // set the vehicle teleport on false lane options
    MSGlobals::gMinLaneVMax4FalseLaneTeleport =
        oc.getFloat("lc-teleport.lane-min-vmax");
    MSGlobals::gMaxVehV4FalseLaneTeleport =
        oc.getFloat("lc-teleport.veh-maxv");
    MSGlobals::gMinVehDist4FalseLaneTeleport =
        oc.getFloat("lc-teleport.min-dist");
    //
    MSGlobals::gCheck4Accidents =
        oc.getBool("check-accidents");
    //
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


