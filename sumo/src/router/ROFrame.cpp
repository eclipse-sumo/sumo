/****************************************************************************/
/// @file    ROFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// Some helping methods for usage within sumo and sumo-gui
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

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


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
ROFrame::fillOptions(OptionsCont &oc)
{
    // add rand and dev options
    RandHelper::insertRandOptions(oc);
    // register the file i/o options
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("configuration-file", "configuration");

    oc.doRegister("output", 'o', new Option_FileName());
    oc.addSynonyme("output-file", "output");

    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");

    oc.doRegister("alternatives", 'a', new Option_FileName());

    oc.doRegister("weights", 'w', new Option_FileName());
    oc.addSynonyme("weights", "weight-files");

    oc.doRegister("lane-weights", 'l', new Option_FileName());

    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.doRegister("end", 'e', new Option_Integer(864000));

    // register vehicle type defaults
    oc.doRegister("krauss-vmax", 'V', new Option_Float(SUMOReal(70)));
    oc.doRegister("krauss-a", 'A', new Option_Float(SUMOReal(2.6)));
    oc.doRegister("krauss-b", 'B', new Option_Float(SUMOReal(4.5)));
    oc.doRegister("krauss-length", 'L', new Option_Float(SUMOReal(5)));
    oc.doRegister("krauss-eps", 'E', new Option_Float(SUMOReal(0.5)));

    // register the report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());
    oc.doRegister("stats-period", new Option_Integer(-1));

    // register the data processing options
    oc.doRegister("continue-on-unbuild", new Option_Bool(false));
    oc.doRegister("unsorted", new Option_Bool(false));
    oc.doRegister("randomize-flows", new Option_Bool(false)); // !!! undescibed
    oc.doRegister("move-on-short", new Option_Bool(false));
    oc.doRegister("max-alternatives", new Option_Integer(5));
    // add possibility to insert random vehicles
    oc.doRegister("random-per-second", 'R', new Option_Float());
    oc.doRegister("prune-random", new Option_Bool(false));
    oc.doRegister("remove-loops", new Option_Bool(false)); // !!! undescibed
}



bool
ROFrame::checkOptions(OptionsCont &oc)
{
    // check whether the output is valid and can be build
    if (!oc.isSet("o")) {
        MsgHandler::getErrorInstance()->inform("No output specified.");
        return false;
    }
    //
    if (oc.getInt("max-alternatives")<2) {
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
    ROVehicleType_Krauss::myDefault_A =
        oc.getFloat("krauss-a");
    ROVehicleType_Krauss::myDefault_B =
        oc.getFloat("krauss-b");
    ROVehicleType_Krauss::myDefault_EPS =
        oc.getFloat("krauss-eps");
    ROVehicleType_Krauss::myDefault_LENGTH =
        oc.getFloat("krauss-length");
    ROVehicleType_Krauss::myDefault_MAXSPEED =
        oc.getFloat("krauss-vmax");
}



/****************************************************************************/

