//---------------------------------------------------------------------------//
//                        RODFFrame.cpp -
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
// Revision 1.5  2006/01/16 13:37:47  dkrajzew
// debugging
//
// Revision 1.4  2006/01/16 13:21:28  dkrajzew
// computation of detector types validated for the 'messstrecke'-scenario
//
// Revision 1.3  2006/01/16 10:46:24  dkrajzew
// some initial work on  the dfrouter
//
// Revision 1.2  2005/12/21 12:48:39  ericnicolay
// *** empty log message ***
//
// Revision 1.1  2005/12/08 12:59:41  ericnicolay
// *** empty log message ***
//
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
#include "RODFFrame.h"
#include <router/ROFrame.h>
#include <utils/common/DevHelper.h>
#include <utils/common/RandHelper.h>


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
RODFFrame::fillOptions(OptionsCont &oc)
{
    // register input-options
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("configuration-file", "configuration");

    oc.doRegister("routes-input", new Option_FileName());

    oc.doRegister("detectors-file", 'd', new Option_FileName());
    oc.addSynonyme("detectors-file", "detectors");

    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");

    oc.doRegister("flow-definitions", 'f', new Option_FileName());
    oc.addSynonyme("flow-definitions", "flows");

    // register output options
    oc.doRegister("routes-output", new Option_FileName());
    oc.doRegister("detectors-output", new Option_FileName());


    // register processing options
    oc.doRegister("build-calibrators", 'C', new Option_Bool(false));
    oc.doRegister("revalidate-routes", new Option_Bool(false));
    oc.doRegister("revalidate-detectors", new Option_Bool(false));


    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.doRegister("end", 'e', new Option_Integer(864000));

    // register vehicle type defaults
    oc.doRegister("krauss-vmax", 'V', new Option_Float(SUMOReal(70)));
    oc.doRegister("krauss-a", 'A', new Option_Float(SUMOReal(2.6)));
    oc.doRegister("krauss-b", 'B', new Option_Float(SUMOReal(4.5)));
    oc.doRegister("krauss-length", 'L', new Option_Float(SUMOReal(5)));
    oc.doRegister("krauss-eps", 'E', new Option_Float(SUMOReal(0.5)));


    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());

    DevHelper::insertDevOptions(oc);
    RandHelper::insertRandOptions(oc);
}


bool
RODFFrame::checkOptions(OptionsCont &oc)
{
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


