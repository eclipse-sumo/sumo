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
// Revision 1.17  2004/02/16 13:44:27  dkrajzew
// dump output generating function renamed in order to add vehicle dump ability in the future
//
// Revision 1.16  2004/01/26 07:09:33  dkrajzew
// added the possibility to place lsa-detectors at a default position/using a default length
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
// false instantiation of option "log-file" as Option_Strng patched into Option_FileName patched
//
// Revision 1.9  2003/06/24 08:09:29  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.8  2003/06/19 11:01:14  dkrajzew
// the simulation has default begin and end times now
//
// Revision 1.7  2003/06/18 11:21:10  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
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
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <fstream>
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/convert/ToString.h>
#include <microsim/MSJunction.h>
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
    oc.doRegister("net-files", 'n', new Option_FileName());
    oc.doRegister("route-files", 'r', new Option_FileName());
    oc.doRegister("additional-files", 'a', new Option_FileName());
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("net-files", "net");
    oc.addSynonyme("route-files", "routes");
    oc.addSynonyme("additional-files", "additional");
    oc.addSynonyme("configuration-file", "configuration");
    // register output options
    oc.doRegister("netstate-dump", new Option_FileName());
    oc.addSynonyme("netstate-dump", "ndump");
    oc.doRegister("tripstate-output", new Option_FileName());
    oc.addSynonyme("tripstate-output", "tout");
    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.doRegister("end", 'e', new Option_Integer(86400));
    oc.doRegister("route-steps", 's', new Option_Integer(0));
    oc.doRegister("quit-on-accident", new Option_Bool(false));
    // register the report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", '?', new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());
    // register some research options
//    oc.doRegister("initial-density", new Option_Float());
//    oc.doRegister("initial-speed", new Option_Float());
    // register the data processing options
    oc.doRegister("dump-intervals", new Option_UIntVector(""));
    oc.doRegister("dump-basename", new Option_FileName());
    oc.doRegister("dump-empty-edges", new Option_Bool(false));
    //
    oc.doRegister("actuating-detector-pos", new Option_Float(100));
    oc.doRegister("agent-detector-len", new Option_Float(75));
    oc.doRegister("srand", new Option_Integer(23423));
    oc.doRegister("abs-rand", new Option_Bool(false));

    oc.doRegister("use-internal-links", 'I', new Option_Bool(false));
}


ostream *
SUMOFrame::buildNetDumpStream(OptionsCont &oc) {
    if(!oc.isSet("netstate-dump")) {
	    return 0;
    }
    ostream *ret = new ofstream(oc.getString("netstate-dump").c_str(),
        ios::out|ios::trunc);
    if(!ret->good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The output file '") + oc.getString("netstate-dump")
            + string("' could not be built."));
        MsgHandler::getErrorInstance()->inform("Simulation failed.");
        throw ProcessError();
    }
    return ret;
}


ostream *
SUMOFrame::buildTripDescStream(OptionsCont &oc) {
    if(!oc.isSet("tripstate-output")) {
	    return 0;
    }
    ostream *ret = new ofstream(oc.getString("tripstate-output").c_str(),
        ios::out|ios::trunc);
    if(!ret->good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The output file '") + oc.getString("tripstate-output")
            + string("' could not be built."));
        MsgHandler::getErrorInstance()->inform("Simulation failed.");
        throw ProcessError();
    }
    return ret;
}


void
SUMOFrame::postbuild(MSNet &net)
{
    MSJunction::postloadInitContainer();
}


bool
SUMOFrame::checkOptions(OptionsCont &oc)
{
    bool ok = true;
    try {
        oc.resetDefaults();
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
    if(oc.getBool("abs-rand")&&!oc.isSet("srand")) {
        oc.set("srand", toString<int>(time(0)));
    }
    return ok;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "SUMOFrame.icc"
//#endif

// Local Variables:
// mode:C++
// End:


