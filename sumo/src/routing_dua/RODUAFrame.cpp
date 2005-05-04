//---------------------------------------------------------------------------//
//                        RODUAFrame.cpp -
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
// Revision 1.2  2005/05/04 08:57:12  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/11/23 10:26:27  dkrajzew
// debugging
// Revision 1.2  2005/02/17 09:27:35  dksumo
// code beautifying;
// some warnings removed;
// compileable under linux
//
// Revision 1.1  2004/10/22 12:50:28  dksumo
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
#include "RODUAFrame.h"
#include <router/ROFrame.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
RODUAFrame::fillOptions_basicImport(OptionsCont &oc)
{
	ROFrame::fillOptions(oc);
	addDUAOptions(oc);
}


void
RODUAFrame::fillOptions_fullImport(OptionsCont &oc)
{
	ROFrame::fillOptions(oc);
	addImportOptions(oc);
	addDUAOptions(oc);
}


void
RODUAFrame::addImportOptions(OptionsCont &oc)
{
    oc.doRegister("cell-input", new Option_FileName());
    oc.doRegister("artemis-input", new Option_FileName());
    oc.doRegister("flow-definition", 'f', new Option_FileName());
    oc.doRegister("sumo-input", 's', new Option_FileName());
    oc.doRegister("trip-defs", 't', new Option_FileName());
    oc.addSynonyme("flow-definition", "flows");
    oc.addSynonyme("artemis", "artemis-input");
    oc.addSynonyme("cell", "cell-input");
    oc.addSynonyme("sumo", "sumo-input");
    oc.addSynonyme("trips", "trip-defs");
    oc.doRegister("save-cell-rindex", new Option_Bool(false));
    oc.doRegister("intel-cell", new Option_Bool(false));
    oc.doRegister("no-last-cell", new Option_Bool(false));
}


void
RODUAFrame::addDUAOptions(OptionsCont &oc)
{
    oc.doRegister( "supplementary-weights", 'S', new Option_FileName() );
    oc.addSynonyme("supplementary-weights", "add");
    oc.doRegister("scheme", 'x', new Option_String("traveltime"));
    // register Gawron's DUE-settings
    oc.doRegister("gBeta", new Option_Float(float(0.3)));
    oc.doRegister("gA", new Option_Float(0.05f));
}


bool
RODUAFrame::checkOptions(OptionsCont &oc)
{
	return ROFrame::checkOptions(oc);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


