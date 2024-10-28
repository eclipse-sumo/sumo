/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIBusStop.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Johannes Rummel
/// @date    Wed, 07.12.2005
///
// A lane area vehicles can halt at (gui-version)
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <string>
#include <microsim/MSStoppingPlace.h>
#include <utils/common/Command.h>
#include <utils/common/VectorHelper.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/geom/Position.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <gui/GUIManipulator.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;
class GUIManipulator;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIBusStop
 * @brief A lane area vehicles can halt at (gui-version)
 *
 * This gui-version of a bus-stop extends MSStoppingPlace by methods for displaying
 *  and interaction.
 *
 * @see MSStoppingPlace
 * @see GUIGlObject_AbstractAdd
 * @see GUIGlObject
 */
class GUIBusStop : public MSStoppingPlace, public GUIGlObject_AbstractAdd {
public:
    /** @brief Constructor
     * @param[in] idStorage The gl-id storage for giving this object an gl-id
     * @param[in] id The id of the bus stop
     * @param[in] lines Names of the bus lines that halt on this bus stop
     * @param[in] lane The lane the bus stop is placed on
     * @param[in] begPos Begin position of the bus stop on the lane
     * @param[in] endPos End position of the bus stop on the lane
     */
    GUIBusStop(const std::string& id,
               SumoXMLTag element,
               const std::vector<std::string>& lines, MSLane& lane,
               double frompos, double topos, const std::string name,
               int personCapacity, double parkingLength, const RGBColor& color);


    /// @brief Destructor
    ~GUIBusStop();


    /// @brief adds an access point to this stop
    bool addAccess(MSLane* const lane, const double startPos, const double endPos, double length, const MSStoppingPlace::AccessExit exit);

    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app,
                                       GUISUMOAbstractView& parent);

    /** @brief Returns an own parameter window
     *
     * Bus stops have no parameter windows (yet).
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window (always 0 in this case)
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app,
            GUISUMOAbstractView& parent);

    /// @brief return exaggeration associated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const;

    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;

    /// @brief Returns the street name
    const std::string getOptionalName() const;

    /// @brief Formats the last free pos value
    double getCroppedLastFreePos() const;

    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    //@}

private:


    /// @brief init constants for faster rendering
    void initShape(PositionVector& fgShape,
                   std::vector<double>& fgShapeRotations, std::vector<double>& fgShapeLengths,
                   Position& fgSignPos, double& fgSignRot, bool secondaryShape = false);

private:
    /// @brief The rotations of the shape parts
    std::vector<double> myFGShapeRotations;
    std::vector<double> myFGShapeRotations2;

    /// @brief The lengths of the shape parts
    std::vector<double> myFGShapeLengths;
    std::vector<double> myFGShapeLengths2;

    /// @brief The shape
    PositionVector myFGShape;
    PositionVector myFGShape2;

    /// @brief The position of the sign
    Position myFGSignPos;
    Position myFGSignPos2;

    /// @brief The rotation of the sign
    double myFGSignRot;
    double myFGSignRot2;

    /// @brief The visual width of the stoppling place
    double myWidth;

    /// @brief The coordinates of access points
    PositionVector myAccessCoords;


};
