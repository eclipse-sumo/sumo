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
// Revision 1.7  2006/01/09 11:53:00  dkrajzew
// bus stops implemented
//
// Revision 1.6  2005/11/09 06:35:03  dkrajzew
// Emitters reworked
//
// Revision 1.5  2005/10/07 11:37:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/22 13:39:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/09/15 11:06:03  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 07:56:53  dkrajzew
// level 3 warnings removed
//
// Revision 1.1  2004/07/02 08:39:56  dkrajzew
// visualisation of vss' added
//
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

#include <string>
#include <fstream>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/trigger/MSTrigger.h>
#include <guisim/GUILaneSpeedTrigger.h>
#include <guisim/GUIEmitter.h>
#include <guisim/GUITriggeredRerouter.h>
#include <guisim/GUIBusStop.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include "GUITriggerBuilder.h"

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




MSEmitter *
GUITriggerBuilder::buildLaneEmitTrigger(MSNet &net,
                                        const std::string &id,
                                        MSLane *destLane,
                                        SUMOReal pos,
                                        const std::string &file)
{
    return new GUIEmitter(id, net, destLane, pos, file);
}



MSTriggeredRerouter *
GUITriggerBuilder::buildRerouter(MSNet &net, const std::string &id,
                                 std::vector<MSEdge*> &edges,
                                 SUMOReal prob, const std::string &file)
{
    return new GUITriggeredRerouter(id, net, edges, prob, file);
}


MSBusStop*
GUITriggerBuilder::buildBusStop(MSNet &net, const std::string &id,
                                const std::vector<std::string> &lines,
                                MSLane *lane,
                                SUMOReal frompos, SUMOReal topos)
{
    return new GUIBusStop(id, net, lines, *lane, frompos, topos);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
