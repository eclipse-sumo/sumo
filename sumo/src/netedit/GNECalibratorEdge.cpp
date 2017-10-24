/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNECalibratorEdge.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2017
/// @version $Id$
///
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

#include <string>
#include <iostream>
#include <utility>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNECalibratorEdge.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNERouteProbe.h"
#include "GNECalibratorDialog.h"
#include "GNECalibratorFlow.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorEdge::GNECalibratorEdge(const std::string& id, GNEEdge* edge, GNEViewNet* viewNet, double pos,
                             double frequency, const std::string& output, const std::vector<GNECalibratorRoute*>& calibratorRoutes,
                             const std::vector<GNECalibratorFlow*>& calibratorFlows, const std::vector<GNECalibratorVehicleType*>& calibratorVehicleTypes) :
    GNECalibrator(id, viewNet, SUMO_TAG_CALIBRATOR, pos / edge->getLanes().at(0)->getLaneParametricLength(), frequency, output, calibratorRoutes, calibratorFlows, calibratorVehicleTypes, edge, NULL)  {
    // this additional ISN'T movable
    myMovable = false;
}


GNECalibratorEdge::~GNECalibratorEdge() {
    // delete Calibrator flows
    for (auto i : myCalibratorFlows) {
        delete i;
    }
    // delete Calibrator routes
    for (auto i : myCalibratorRoutes) {
        delete i;
    }
    // delete Calibrator vehicle types
    for (auto i : myCalibratorVehicleTypes) {
        delete i;
    }
}