//---------------------------------------------------------------------------//
//                        ROFrame.cpp -
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
// $Log: ROFrame.cpp,v $
// Revision 1.14  2006/10/13 13:07:50  dkrajzew
// added the option to not emit vehicles from flows using a fix frequency
//
// Revision 1.13  2006/08/01 07:19:57  dkrajzew
// removed build number information
//
// Revision 1.12  2006/04/18 08:15:49  dkrajzew
// removal of loops added
//
// Revision 1.11  2006/04/07 10:41:47  dkrajzew
// code beautifying: embedding string in strings removed
//
// Revision 1.10  2006/01/26 08:37:23  dkrajzew
// removed warnings 4786
//
// Revision 1.9  2006/01/16 13:38:22  dkrajzew
// help and error handling patched
//
// Revision 1.8  2006/01/09 12:00:58  dkrajzew
// debugging vehicle color usage
//
// Revision 1.7  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/07/12 12:39:01  dkrajzew
// edge-based mean data implemented; previous lane-based is now optional
//
// Revision 1.3  2005/05/04 08:47:14  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; usage of the -R option within the jtr-router debugged
//
// Revision 1.2  2004/12/20 10:48:36  dkrajzew
// net-files changed to net-file
//
// Revision 1.1  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 06:18:15  dksumo
// max-alternatives options added;
// fastened up the output of warnings and messages
//
// Revision 1.1  2004/10/22 12:50:23  dksumo
// initial checkin into an internal, standalone SUMO CVS
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
#include <utils/common/RandHelper.h>
#include <utils/common/ToString.h>
#include <router/ROVehicleType_Krauss.h>
#include "ROFrame.h"

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
ROFrame::fillOptions(OptionsCont &oc)
{
    // register options
        // register I/O options
    oc.doRegister("output", 'o', new Option_FileName());
    oc.addSynonyme("output-file", "output");
    oc.addDescription("output-file", "Output", "Write generated routes to FILE");

    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Use FILE as SUMO-network to route on");

    oc.doRegister("alternatives", 'a', new Option_FileName());
    oc.addDescription("alternatives", "Input", "Read alternatives from FILE");

    oc.doRegister("weights", 'w', new Option_FileName());
    oc.addSynonyme("weights", "weight-files");
    oc.addDescription("weights", "Input", "Read network weights from FILE");

    oc.doRegister("lane-weights", 'l', new Option_FileName());
    oc.addDescription("lane-weights", "Input", "Read lane-weights from FILE");


        // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.addDescription("begin", "Time", "Defines the begin time; Previous trips will be discarded");

    oc.doRegister("end", 'e', new Option_Integer(864000));
    oc.addDescription("end", "Time", "Defines the end time; Later trips will be discarded");


        // register the processing options
    oc.doRegister("continue-on-unbuild", new Option_Bool(false));
    oc.addDescription("continue-on-unbuild", "Processing", "Continue if a route could not be build");
    
    oc.doRegister("unsorted", new Option_Bool(false));
    oc.addDescription("unsorted", "Processing", "Assume input is unsorted");
    
    oc.doRegister("randomize-flows", new Option_Bool(false)); // !!! undescibed
    oc.addDescription("randomize-flows", "Processing", "");
    
    oc.doRegister("move-on-short", new Option_Bool(false));
    oc.addDescription("move-on-short", "Processing", "Move vehicles to the next edge if the first is too short");
    
    oc.doRegister("max-alternatives", new Option_Integer(5));
    oc.addDescription("max-alternatives", "Processing", "Prune the number of alternatives to INT");
    
    // add possibility to insert random vehicles
    oc.doRegister("random-per-second", 'R', new Option_Float());
    oc.addDescription("random-per-second", "Processing", "Emit FLOAT random vehicles per second");
    
    oc.doRegister("prune-random", new Option_Bool(false));
    oc.addDescription("prune-random", "Processing", "");
    
    oc.doRegister("remove-loops", new Option_Bool(false)); // !!! undescibed
    oc.addDescription("remove-loops", "Processing", "");


        // register vehicle type defaults
    oc.doRegister("krauss-vmax", 'V', new Option_Float(SUMOReal(70)));
    oc.addDescription("krauss-vmax", "Generated Vehicles", "Defines emitted vehicles' max. velocity");

    oc.doRegister("krauss-a", 'A', new Option_Float(SUMOReal(2.6)));
    oc.addDescription("krauss-a", "Generated Vehicles", "Defines emitted vehicles' max. acceleration");

    oc.doRegister("krauss-b", 'B', new Option_Float(SUMOReal(4.5)));
    oc.addDescription("krauss-b", "Generated Vehicles", "Defines emitted vehicles' max. deceleration");

    oc.doRegister("krauss-length", 'L', new Option_Float(SUMOReal(5)));
    oc.addDescription("krauss-length", "Generated Vehicles", "Defines emitted vehicles' length");

    oc.doRegister("krauss-eps", 'E', new Option_Float(SUMOReal(0.5)));
    oc.addDescription("krauss-eps", "Generated Vehicles", "Defines emitted vehicles' driver imperfection");


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

    oc.doRegister("stats-period", new Option_Integer(-1));
    oc.addDescription("stats-period", "Report", "Defines how often statistics shall be printed");
}



bool
ROFrame::checkOptions(OptionsCont &oc)
{
    // check whether the output is valid and can be build
    if(!oc.isSet("o")) {
        MsgHandler::getErrorInstance()->inform("No output specified.");
        return false;
    }
    //
    if(oc.getInt("max-alternatives")<2) {
        MsgHandler::getErrorInstance()->inform("At least two alternatives should be enabled");
        return false;
    }
    //
    return true;
}


/**
 * Inserts the default from options into the vehicle
 *  type descriptions
 */
void
ROFrame::setDefaults(OptionsCont &oc)
{
    // insert the krauss-values
    ROVehicleType_Krauss::myDefault_A = oc.getFloat("krauss-a");
    ROVehicleType_Krauss::myDefault_B = oc.getFloat("krauss-b");
    ROVehicleType_Krauss::myDefault_EPS = oc.getFloat("krauss-eps");
    ROVehicleType_Krauss::myDefault_LENGTH = oc.getFloat("krauss-length");
    ROVehicleType_Krauss::myDefault_MAXSPEED = oc.getFloat("krauss-vmax");
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


