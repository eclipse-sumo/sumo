//---------------------------------------------------------------------------//
//                        ROJTRFrame.cpp -
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
// Revision 1.1  2004/11/23 10:26:59  dkrajzew
// debugging
//
// Revision 1.2  2004/11/22 12:50:31  dksumo
// added 'flows' synonyme
//
// Revision 1.1  2004/10/22 12:50:29  dksumo
// initial checkin into an internal, standalone SUMO CVS
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
#include "ROJTRFrame.h"
#include <router/ROFrame.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
ROJTRFrame::fillOptions(OptionsCont &oc)
{
    ROFrame::fillOptions(oc);
    addJTROptions(oc);
}


void
ROJTRFrame::addJTROptions(OptionsCont &oc)
{
    oc.doRegister("max-edges-factor", new Option_Float(2.0));
    oc.doRegister("flow-definition", 'f', new Option_FileName());
    oc.addSynonyme("flow-definition", "flows");
    oc.doRegister("turn-definition", 't', new Option_FileName());
    oc.addSynonyme("turn-definition", "turns");
    oc.doRegister("turn-defaults", 'T', new Option_String("30;50;20"));
    oc.doRegister("sinks", 's', new Option_String());
}


bool
ROJTRFrame::checkOptions(OptionsCont &oc)
{
    return ROFrame::checkOptions(oc);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


