/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDottedContourThread.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2019
///
// The thread that performs the calculation of contours around shapes
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <netbuild/NBFrame.h>
#include <netbuild/NBNetBuilder.h>
#include <netimport/NIFrame.h>
#include <netimport/NILoader.h>
#include <netwrite/NWFrame.h>
#include <utils/common/MsgRetrievingFunction.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SystemFrame.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/xml/XMLSubSys.h>

#include "GNEDottedContourThread.h"
#include "GNENet.h"


// ===========================================================================
// member method definitions
// ===========================================================================
GNEDottedContourThread::GNEDottedContourThread(GNENet *net) :
    FXThread(),
    myNet(net),
    myLockNetElementsQueue(false),
    myLockAdditionalsQueue(false),
    myLockShapeQueue(false),
    myLockDemandElementsQueue(false) {
    // start thread
    start();
}


GNEDottedContourThread::~GNEDottedContourThread() {
    // stop thread
    cancel();
}


void 
GNEDottedContourThread::updateNetElementDottedContour(GNENetElement *netElement) {
    myLockNetElementsQueue = true;
    myNetElements.push(netElement);
    myLockNetElementsQueue = false;
}


FXint
GNEDottedContourThread::run() {
    while (true) {
        // check net elements
        if (myLockNetElementsQueue == false) {
            if (myNetElements.size() > 0) {
                myNetElements.pop();
            }
        }
        // check additionals
        if (myLockAdditionalsQueue == false) {
            if (myAdditionals.size() > 0) {
                myAdditionals.pop();
            }
        }
        // check shape
        if (myLockShapeQueue == false) {
            if (myShapes.size() > 0) {
                myShapes.pop();
            }
        }
        // check demand elements
        if (myLockDemandElementsQueue == false) {
            if (myDemandElements.size() > 0) {
                myDemandElements.pop();
            }
        }
        // sleep
        sleep(100);
    }
    return 0;
}


/****************************************************************************/
