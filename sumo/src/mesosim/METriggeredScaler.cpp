/****************************************************************************/
/// @file    METriggeredScaler.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: METriggeredScaler.cpp 102 2007-06-12 06:17:01Z behr_mi $
///
// }
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_MESOSIM

#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include "MELoop.h"
#include "METriggeredScaler.h"
#include "MESegment.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================

METriggeredScaler::METriggeredScaler(const std::string &id,
                                     MESegment *edge,
                                     SUMOReal scale)
        : MSTrigger(id),
        mySegment(edge),
        myAggregatedPassedQ(0),
        myRemovedNo(0),
        myScale(scale)
{
    mySegment->addDetectorData(this, 1, 0);
}


METriggeredScaler::~METriggeredScaler()
{}




SUMOTime
METriggeredScaler::execute(SUMOTime currentTime)
{
    // get current simulation values (valid for the last simulation second)
    MSLaneMeanDataValues &meanData = mySegment->getDetectorData(this, currentTime);
    SUMOReal isQ = meanData.nSamples;
    myAggregatedPassedQ += isQ;
    mySegment->flushDetectorData(this, currentTime);
    while (myRemovedNo<myAggregatedPassedQ*myScale) {
        MEVehicle *erased = mySegment->eraseRandomCar2();
        if (erased!=0) {
            myRemovedNo++;
            MSGlobals::gMesoNet->eraseCar(erased);
        } else {
            break;
        }
    }
    return 1;
}


#endif // HAVE_MESOSIM

/****************************************************************************/

