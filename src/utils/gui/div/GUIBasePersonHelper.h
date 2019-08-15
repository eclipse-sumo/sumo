/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIBasePersonHelper.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2019
/// @version $Id$
///
// Functions used in GUIPerson and GNEPerson for drawing persons
/****************************************************************************/
#ifndef GUIBasePersonHelper_h
#define GUIBasePersonHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIVisualizationSettings;
class GUIGlObject;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIBasePersonHelper
 * @brief A list of functions used for drawing persons in GUI
 */
class GUIBasePersonHelper {
public:

    static void drawAction_drawAsTriangle(const double angle, const double lenght, const double width);
    static void drawAction_drawAsCircle(const double lenght, const double width);
    static void drawAction_drawAsPoly(const double angle, const double lenght, const double width);
    static void drawAction_drawAsImage(const double angle, const double lenght, const double width, const std::string& file,
                                       const SUMOVehicleShape guiShape, const double exaggeration);
};


#endif

/****************************************************************************/

