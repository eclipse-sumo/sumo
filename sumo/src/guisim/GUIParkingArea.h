/****************************************************************************/
/// @file    GUIParkingArea.h
/// @author  Mirco Sturari
/// @date    Tue, 19.01.2016
/// @version $Id$
///
// A area where vehicles can park next to the road (gui version)
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
#ifndef GUIParkingArea_h
#define GUIParkingArea_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <utils/common/Command.h>
#include <utils/common/VectorHelper.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/MSParkingArea.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/geom/Position.h>
#include <gui/GUIManipulator.h>
#include <utils/foxtools/FXRealSpinDial.h>


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
 * @class GUIParkingArea
 * @brief A lane area vehicles can halt at (gui-version)
 *
 * This gui-version of a parking-area extends MSStoppingPlace by methods for displaying
 *  and interaction.
 *
 * @see MSStoppingPlace
 * @see GUIGlObject_AbstractAdd
 * @see GUIGlObject
 */
class GUIParkingArea : public MSParkingArea, public GUIGlObject_AbstractAdd {
public:

    /** @brief Constructor
     * @param[in] idStorage The gl-id storage for giving this object an gl-id
     * @param[in] id The id of the parking area
     * @param[in] lines Names of the parking lines that halt on this parking area
     * @param[in] lane The lane the parking area is placed on
     * @param[in] begPos Begin position of the parking area on the lane
     * @param[in] endPos End position of the parking area on the lane
     * @param[in] capacity Capacity of the parking area (if > 0 lots are generated, otherwise expected addLotEntry())
     * @param[in] width Default width of the lot rectangle (if = 0 is computed from line.getWidth())
     * @param[in] length Default length of the lot rectangle (if = 0 is computed from endPos-begPos)
     * @param[in] angle Default angle of the lot rectangle relative to lane direction (if = 0 is computed ... TODO)
     */
    GUIParkingArea(const std::string& id,
                   const std::vector<std::string>& lines, MSLane& lane,
                   SUMOReal frompos, SUMOReal topos, unsigned int capacity,
                   SUMOReal width, SUMOReal length, SUMOReal angle);


    /// @brief Destructor
    ~GUIParkingArea();



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
     * Container stops have no parameter windows (yet).
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window (always 0 in this case)
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app,
            GUISUMOAbstractView& parent);


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    //@}


private:
    /// @brief The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// @brief The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;

    /// @brief The position of the sign
    Position mySignPos;

    /// @brief The rotation of the sign
    SUMOReal mySignRot;

};


#endif

/****************************************************************************/
