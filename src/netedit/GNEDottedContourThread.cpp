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
#include "GNEViewNet.h"


// ===========================================================================
// member method definitions
// ===========================================================================
GNEDottedContourThread::GNEDottedContourThread(GNEViewNet *viewNet) :
    FXThread(),
    myViewNet(viewNet) {
    // start thread
    start();
}


GNEDottedContourThread::~GNEDottedContourThread() {
    // stop thread
    cancel();
}


FXint
GNEDottedContourThread::run() {
    return 0;
}


/****************************************************************************/
