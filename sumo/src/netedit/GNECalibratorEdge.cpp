/****************************************************************************/
/// @file    GNECalibratorEdge.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2017
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <string>
#include <iostream>
#include <utility>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
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


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorEdge::GNECalibratorEdge(const std::string& id, GNEEdge* edge, GNEViewNet* viewNet, double pos,
                             double frequency, const std::string& output, const std::vector<GNECalibratorRoute>& calibratorRoutes,
                             const std::vector<GNECalibratorFlow>& calibratorFlows, const std::vector<GNECalibratorVehicleType>& calibratorVehicleTypes) :
    GNECalibrator(id, viewNet, SUMO_TAG_CALIBRATOR, pos / edge->getLanes().at(0)->getLaneParametricLength(), frequency, output, calibratorRoutes, calibratorFlows, calibratorVehicleTypes)  {
    // This additional belong to an edge
    myEdge = edge;
    // this additional ISN'T movable
    myMovable = false;
    // Update geometry;
    updateGeometry();
    // Center view in the position of calibrator
    myViewNet->centerTo(getGlID(), false);
}


GNECalibratorEdge::~GNECalibratorEdge() {}