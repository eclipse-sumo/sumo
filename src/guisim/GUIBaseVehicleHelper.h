/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIBaseVehicleHelper.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
/// @version $Id$
///
// Functions used in GUIBaseVehicleHelper and GNEVehicle
/****************************************************************************/
#ifndef GUIBaseVehicleHelper_h
#define GUIBaseVehicleHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <set>
#include <string>
#include <fx.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/settings/GUIPropertySchemeStorage.h>
#include <microsim/MSVehicle.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;
class GUIGLObjectPopupMenu;
class MSDevice_Vehroutes;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIBaseVehicleHelper
 * @brief A MSVehicle extended by some values for usage within the gui
 */
class GUIBaseVehicleHelper {
public:

    /// @brief draw poly
    static void drawPoly(const double* poses, const double offset);

    /// @brief draw vehicle as a Box
    static void drawAction_drawVehicleAsBoxPlus(const double width, const double length);

    /// @brief draw vehicle as a triangle
    static void drawAction_drawVehicleAsTrianglePlus(const double width, const double length);

    /// @brief draw vehicle as a polygon
    static bool drawAction_drawVehicleAsPoly(const GUIBaseVehicle *baseVehicle, const GUIVisualizationSettings& s, const SUMOVehicleShape shape, const double width, const double length);

    /**@brief try to draw vehicle as raster image and return true if sucessful
     * @param[in] length The custom length of the vehicle (defaults to the * length specified in the vehicle type if -1 is passed)
    */
    static bool drawAction_drawVehicleAsImage(const GUIVisualizationSettings& s, const std::string& file, const GUIGlObject* o, const double width, double length = -1);
};


#endif

/****************************************************************************/

