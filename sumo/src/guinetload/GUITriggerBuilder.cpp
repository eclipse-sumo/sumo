/***************************************************************************
                          GUITriggerBuilder.cpp
                          A building helper for triggers
                             -------------------
    begin                : Mon, 26.04.2004
    copyright            : (C) 2004 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
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
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.2  2005/05/04 07:56:53  dkrajzew
// level 3 warnings removed
//
// Revision 1.1  2004/07/02 08:39:56  dkrajzew
// visualisation of vss' added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <fstream>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSTrigger.h>
#include <guisim/GUILaneSpeedTrigger.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include "GUITriggerBuilder.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUITriggerBuilder::GUITriggerBuilder()
{
}


GUITriggerBuilder::~GUITriggerBuilder()
{
}


MSLaneSpeedTrigger *
GUITriggerBuilder::buildLaneSpeedTrigger(MSNet &net,
            const std::string &id, const std::vector<MSLane*> &destLanes,
            const std::string &file)
{
    return new GUILaneSpeedTrigger(id, net, destLanes, file);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
