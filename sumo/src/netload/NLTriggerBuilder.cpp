/***************************************************************************
                          NLTriggerBuilder.cpp
                          A building helper for triggers
                             -------------------
    begin                : Thu, 17 Oct 2002
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 1.2  2002/10/22 10:05:17  dkrajzew
// unsupported return value added
//
// Revision 1.1  2002/10/17 10:50:18  dkrajzew
// building of triggers added (initial)
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <fstream>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSTrigger.h>
#include <microsim/MSLaneSpeedTrigger.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>
#include "NLTriggerBuilder.h"


using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
MSTrigger *
NLTriggerBuilder::buildTrigger(MSNet &net, const std::string &id, 
                               const std::string &objecttype, 
                               const std::string &objectid, 
                               const std::string &objectattr,
                               std::string file, std::string base)
{
    // check whether absolute or relative filenames are given
    if(!FileHelpers::isAbsolute(file)) {
        file = FileHelpers::getConfigurationRelative(base, file);
    }
    // check which typ of a trigger shall be build
    if(objecttype=="lane"&&objectattr=="speed") {
        MSLane *lane = MSLane::dictionary(objectid);
        if(lane==0) {
            SErrorHandler::add(
                string("The lane to use within MSLaneSpeedTrigger '")
                + id + string("' is not known."));
            throw ProcessError();
        }
        return buildLaneSpeedTrigger(net, id, *lane, file);
    }
    return 0;
}

MSLaneSpeedTrigger *
NLTriggerBuilder::buildLaneSpeedTrigger(MSNet &net,
                                        const std::string &id, 
                                        MSLane &lane,
                                        const std::string &file)
{
    return new MSLaneSpeedTrigger(id, net, lane, file);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLTriggerBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:
