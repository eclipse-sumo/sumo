/***************************************************************************
                          GUIVehicleTransfer.cpp  -
 The gui-version of a mover of vehicles that got stucked due to grid locks
                             -------------------
    begin                : Sep 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2003/11/20 14:41:02  dkrajzew
// class documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <gui/GUIGlObjectStorage.h>
#include <microsim/MSVehicle.h>
#include <guisim/GUINet.h>
#include "GUIVehicle.h"
#include "GUIVehicleTransfer.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIVehicleTransfer::GUIVehicleTransfer()
{
}


void
GUIVehicleTransfer::removeVehicle(const std::string &id)
{
    MSVehicle *veh = MSVehicle::dictionary(id);
    static_cast<GUIVehicle*>(veh)->setRemoved();
    static_cast<GUINet*>(MSNet::getInstance())->getIDStorage().remove(
        static_cast<GUIVehicle*>(veh)->getGlID());
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

