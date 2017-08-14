/****************************************************************************/
/// @file    GNEShape.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
/// @version $Id$
///
// Abstract class for Shapes uses in netedit
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

#include "GNEShape.h"
#include "GNENet.h"

// ===========================================================================
// method definitions
// ===========================================================================


GNEShape::GNEShape(GNENet* net, SumoXMLTag tag, GUIIcon icon) :
    GNEAttributeCarrier(tag, icon),
    myNet(net),
    myBlockMovement(false),
    myBlockShape(false) {
}


GNEShape::~GNEShape() {}


GNENet*
GNEShape::getNet() const {
    return myNet;
}


bool 
GNEShape::isMovementBlocked() const {
    return myBlockMovement;
}


bool 
GNEShape::isShapeBlocked() const {
    return myBlockShape;
}


/****************************************************************************/
