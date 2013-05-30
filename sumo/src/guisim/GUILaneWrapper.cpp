/****************************************************************************/
/// @file    GUILaneWrapper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 25 Nov 2002
/// @version $Id$
///
// A MSLane extended for visualisation purposes.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#ifdef HAVE_OSG
#include <osg/Geometry>
#endif
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/geom/PositionVector.h>
#include <microsim/MSNet.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include "GUILaneWrapper.h"
#include "GUIEdge.h"
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <guisim/GUINet.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/gui/div/GLHelper.h>
#include <gui/GUIViewTraffic.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <guisim/GUIVehicle.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/common/HelpersHarmonoise.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUILaneWrapper::GUILaneWrapper(MSLane& lane, const PositionVector& shape, unsigned int index) :
    GUIGlObject(GLO_LANE, lane.getID()),
    myLane(lane),
    myShape(shape),
    myIndex(index)
#ifdef HAVE_OSG
    , myGeom(0)
#endif
{
    myShapeRotations.reserve(myShape.size() - 1);
    myShapeLengths.reserve(myShape.size() - 1);
    int e = (int) myShape.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myShape[i];
        const Position& s = myShape[i + 1];
        myShapeLengths.push_back(f.distanceTo2D(s));
        myShapeRotations.push_back(RAD2DEG(atan2(s.x() - f.x(), f.y() - s.y())));
    }
    //
    myHalfLaneWidth = (SUMOReal)(myLane.getWidth() / 2.);
    myQuarterLaneWidth = (SUMOReal)(myLane.getWidth() / 4.);
}


GUILaneWrapper::~GUILaneWrapper() {}


bool
GUILaneWrapper::forLane(const MSLane& lane) const {
    return (&myLane) == (&lane);
}








/****************************************************************************/

