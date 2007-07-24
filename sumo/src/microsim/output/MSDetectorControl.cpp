/****************************************************************************/
/// @file    MSDetectorControl.cpp
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
//  »missingDescription«
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

#include <iostream>
#include "MSDetectorControl.h"
#include "MSDetector2File.h"
#include "MSInductLoop.h"
#include <microsim/MSUnit.h>
#include <microsim/MSUpdateEachTimestepContainer.h>
#include "MSDetectorHaltingContainerWrapper.h"
#include "MSDetectorOccupancyCorrection.h"
#include "e2_detectors/MS_E2_ZS_CollectorOverLanes.h"
#include "meandata/MSMeanData_Net.h"
#include "meandata/MSMeanData_Net_Utils.h"
#include <microsim/output/e2_detectors/MSE2Collector.h>
#include <microsim/output/e2_detectors/MS_E2_ZS_CollectorOverLanes.h>
#include <microsim/output/e3_detectors/MSE3Collector.h>
#include <microsim/output/MSInductLoop.h>
#include "MSInductLoop.h"
#include <mesosim/MEInductLoop.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
MSDetectorControl::MSDetectorControl()
{
    MSUnit::create(1.0, 1.0);
}


MSDetectorControl::~MSDetectorControl()
{
    myDetector2File.close();
    myMesoLoops.clear();
    myLoops.clear();
    myE2Detectors.clear();
    myE3Detectors.clear();
    myE2OverLanesDetectors.clear();

    delete MSUpdateEachTimestepContainer< DetectorContainer::UpdateHaltings >::getInstance();
    delete MSUpdateEachTimestepContainer< Detector::UpdateE2Detectors >::getInstance();
    delete MSUpdateEachTimestepContainer< Detector::UpdateOccupancyCorrections >::getInstance();
    delete MSUpdateEachTimestepContainer< MSE3Collector >::getInstance();
    delete MSUnit::getInstance();
}


MSInductLoop *
MSDetectorControl::findInductLoop(const std::string &id)
{
    return myLoops.get(id);
}


void
MSDetectorControl::add(MSInductLoop *il,
                           OutputDevice *device,
                           int splInterval)
{
    // insert object into dictionary
    if (! myLoops.add(il->getID(), il)) {
        throw ProcessError("induct loop '" + il->getID() + "' could not be build;"
                           + "\n (declared twice?)");
    }
    myDetector2File.addDetectorAndInterval(il, device, splInterval); // !!! test
    // !!!! a command may have been added
}


void
MSDetectorControl::add(MSE2Collector *e2,
                           OutputDevice *device,
                           int splInterval)
{
    // insert object into dictionary
    if (! myE2Detectors.add(e2->getID(), e2)) {
        throw ProcessError("e2-detector '" + e2->getID() + "' could not be build;"
                           + "\n (declared twice?)");
    }
    myDetector2File.addDetectorAndInterval(e2, device, splInterval); // !!! test
}


void
MSDetectorControl::add(MS_E2_ZS_CollectorOverLanes *e2ol,
                           OutputDevice *device,
                           int splInterval)
{
    // insert object into dictionary
    if (! myE2OverLanesDetectors.add(e2ol->getID(), e2ol)) {
        throw ProcessError("e2-overlanes-detector '" + e2ol->getID() + "' could not be build;"
                           + "\n (declared twice?)");
    }
    myDetector2File.addDetectorAndInterval(e2ol, device, splInterval); // !!! test
}


void
MSDetectorControl::add(MSE2Collector *e2)
{
    // insert object into dictionary
    if (! myE2Detectors.add(e2->getID(), e2)) {
        throw ProcessError("e2-detector '" + e2->getID() + "' could not be build;"
                           + "\n (declared twice?)");
    }
}


void
MSDetectorControl::add(MS_E2_ZS_CollectorOverLanes *e2ol)
{
    // insert object into dictionary
    if (! myE2OverLanesDetectors.add(e2ol->getID(), e2ol)) {
        throw ProcessError("e2-overlanes-detector '" + e2ol->getID() + "' could not be build;"
                           + "\n (declared twice?)");
    }
}


void
MSDetectorControl::add(MSE3Collector *e3,
                           OutputDevice *device,
                           int splInterval)
{
    // insert object into dictionary
    if (! myE3Detectors.add(e3->getID(), e3)) {
        throw ProcessError("e3-detector '" + e3->getID() + "' could not be build;"
                           + "\n (declared twice?)");
    }
    myDetector2File.addDetectorAndInterval(e3, device, splInterval); // !!! test
}


void
MSDetectorControl::add(MEInductLoop *meil,
                           OutputDevice *device,
                           int splInterval)
{
    // insert object into dictionary
    if (! myMesoLoops.add(meil->getID(), meil)) {
        throw ProcessError("meso-induct loop '" + meil->getID() + "' could not be build;"
                           + "\n (declared twice?)");
    }
    myDetector2File.addDetectorAndInterval(meil, device, splInterval); // !!! test
}


MSDetector2File &
MSDetectorControl::getDet2File()
{
    return myDetector2File;
}

MSDetectorControl::LoopVect
MSDetectorControl::getLoopVector() const
{
    return myLoops.getTempVector();
}


MSDetectorControl::E2Vect
MSDetectorControl::getE2Vector() const
{
    return myE2Detectors.getTempVector();
}


MSDetectorControl::E3Vect
MSDetectorControl::getE3Vector() const
{
    return myE3Detectors.getTempVector();
}


MSDetectorControl::E2ZSOLVect
MSDetectorControl::getE2OLVector() const
{
    return myE2OverLanesDetectors.getTempVector();
}


void
MSDetectorControl::resetInterval(MSDetectorFileOutput *il,
                                 SUMOTime interval)
{
    myDetector2File.resetInterval(il, interval);
}



/****************************************************************************/

