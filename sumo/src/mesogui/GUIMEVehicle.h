/****************************************************************************/
/// @file    GUIMEVehicle.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIMEVehicle_h
#define GUIMEVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <set>
#include <string>
#include <guisim/GUIBaseVehicle.h>
#include <mesosim/MEVehicle.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIMEVehicle
 * @brief A MSVehicle extended by some values for usage within the gui
 *
 * A visualisable MSVehicle. Extended by the possibility to retrieve names
 * of all available vehicles (static) and the possibility to retrieve the
 * color of the vehicle which is available in different forms allowing an
 * easier recognition of done actions such as lane changing.
 */
class GUIMEVehicle : public MEVehicle, public GUIBaseVehicle {
public:
    /** @brief Constructor
     * @param[in] pars The vehicle description
     * @param[in] route The vehicle's route
     * @param[in] type The vehicle's type
     * @param[in] speedFactor The factor for driven lane's speed limits
     * @exception ProcessError If a value is wrong
     */
    GUIMEVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
               const MSVehicleType* type, const SUMOReal speedFactor);


    /// @brief destructor
    ~GUIMEVehicle();


    /** @brief Return current position (x/y, cartesian)
     *
     * If the vehicle's myLane is 0, Position::INVALID.
     * @param[in] offset optional offset in longitudinal direction
     * @return The current position (in cartesian coordinates)
     * @see myLane
     */
    Position getPosition(const SUMOReal offset = 0) const;

    /** @brief Returns the vehicle's direction in degrees
     * @return The vehicle's current angle
     */
    SUMOReal getAngle() const;

    /// @brief gets the color value according to the current scheme index
    SUMOReal getColorValue(size_t activeScheme) const;

    /// @brief draws the given guiShape if it has distinc carriages/modules and eturns true if so
    bool drawAction_drawCarriageClass(const GUIVisualizationSettings& s, SUMOVehicleShape guiShape, bool asImage) const;

    /** @brief Returns the time since the last lane change in seconds
     * @see MSVehicle::myLastLaneChangeOffset
     * @return The time since the last lane change in seconds
     */
    SUMOReal getLastLaneChangeOffset() const;

    /** @brief Draws the route
     * @param[in] r The route to draw
     */
    void drawRouteHelper(const MSRoute& r, SUMOReal exaggeration) const;

    /// @brief retrieve information about the current stop state
    std::string getStopInfo() const;

    /// @brief adds the blocking foes to the current selection
    void selectBlockingFoes() const;

    inline void setSegment(MESegment* s, size_t idx = 0) {
        MEVehicle::setSegment(s, idx);
        myPos = Position::INVALID;
    }

    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);


    /// @brief set position and angle for drawing
    void setPositionAndAngle(const Position& pos, SUMOReal angle) {
        myPos = pos;
        myAngle = angle;
    }

private:

    /// @brief the position for drawing
    Position myPos;

    /// @brief the angle for drawing
    SUMOReal myAngle;

};


#endif

/****************************************************************************/

