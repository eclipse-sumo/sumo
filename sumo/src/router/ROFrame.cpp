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
// $Log$
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
#include <utils/common/RandHelper.h>
#include <utils/convert/ToString.h>
#include <router/ROVehicleType_Krauss.h>
#include "ROFrame.h"


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
    // add rand options
    RandHelper::insertRandOptions(oc);
    // register the file i/o options
    oc.doRegister("output", 'o', new Option_FileName());
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.doRegister("weights", 'w', new Option_FileName());
    oc.doRegister("alternatives", 'a', new Option_FileName());
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addSynonyme("output-file", "output");
    oc.addSynonyme("configuration-file", "configuration");
    oc.addSynonyme("weights", "weight-files");
    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.doRegister("end", 'e', new Option_Integer(864000));
    // register vehicle type defaults
    oc.doRegister("krauss-vmax", 'V', new Option_Float(float(70)));
    oc.doRegister("krauss-a", 'A', new Option_Float(float(2.6)));
    oc.doRegister("krauss-b", 'B', new Option_Float(float(4.5)));
    oc.doRegister("krauss-length", 'L', new Option_Float(float(5)));
    oc.doRegister("krauss-eps", 'E', new Option_Float(float(0.5)));
    // register the report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("continue-on-unbuild", new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());
    // register the data processing options
    oc.doRegister("unsorted", new Option_Bool(false));
    oc.doRegister("move-on-short", new Option_Bool(false));
    oc.doRegister("stats-period", new Option_Integer(-1));
    oc.doRegister("max-alternatives", new Option_Integer(5));
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
    std::ofstream tst(oc.getString("o").c_str());
    if(!tst.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The output file '") + oc.getString("o")
            + string("' can not be build."));
        return false;
    }
    //
    if(oc.getInt("max-alternatives")<2) {
        MsgHandler::getErrorInstance()->inform(
            string("At least two alternatives should be enabled"));
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

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


