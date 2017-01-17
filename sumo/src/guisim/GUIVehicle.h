/****************************************************************************/
/// @file    GUIVehicle.h
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
#ifndef GUIVehicle_h
#define GUIVehicle_h


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
#include <utils/geom/PositionVector.h>
#include <microsim/MSVehicle.h>
#include "GUIBaseVehicle.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIVehicle
 * @brief A MSVehicle extended by some values for usage within the gui
 *
 * A visualisable MSVehicle. Extended by the possibility to retrieve names
 * of all available vehicles (static) and the possibility to retrieve the
 * color of the vehicle which is available in different forms allowing an
 * easier recognition of done actions such as lane changing.
 */
class GUIVehicle : public MSVehicle, public GUIBaseVehicle {
public:
    /** @brief Constructor
     * @param[in] pars The vehicle description
     * @param[in] route The vehicle's route
     * @param[in] type The vehicle's type
     * @param[in] speedFactor The factor for driven lane's speed limits
     * @exception ProcessError If a value is wrong
     */
    GUIVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
               const MSVehicleType* type, const SUMOReal speedFactor);


    /// @brief destructor
    ~GUIVehicle();

    /** @brief Return current position (x/y, cartesian)
     *
     * @note implementation of abstract method does not work otherwise
     */
    Position getPosition(const SUMOReal offset = 0) const {
        return MSVehicle::getPosition(offset);
    }

    /** @brief Return current angle
     *
     * @note implementation of abstract method does not work otherwise
     */
    SUMOReal getAngle() const {
        return MSVehicle::getAngle();
    }

    /** @brief Draws the route
     * @param[in] r The route to draw
     */
    void drawRouteHelper(const MSRoute& r, SUMOReal exaggeration) const;

    void drawAction_drawVehicleBlinker(SUMOReal length) const;
    void drawAction_drawVehicleBrakeLight(SUMOReal length, bool onlyOne = 1) const;
    void drawAction_drawPersonsAndContainers(const GUIVisualizationSettings& s) const;
    void drawAction_drawLinkItems(const GUIVisualizationSettings& s) const;
    void drawAction_drawVehicleBlueLight() const;

    /** @brief Returns the time since the last lane change in seconds
     * @see MSVehicle::myLastLaneChangeOffset
     * @return The time since the last lane change in seconds
     */
    SUMOReal getLastLaneChangeOffset() const;


    /** @brief Draws the vehicle's best lanes
     */
    void drawBestLanes() const;
    /// @}

    /// @brief adds the blocking foes to the current selection
    void selectBlockingFoes() const;

    /// @brief gets the color value according to the current scheme index
    SUMOReal getColorValue(int activeScheme) const;

    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /** @brief Returns an own type parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     */
    GUIParameterTableWindow* getTypeParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

protected:
    /// @brief register vehicle for drawing while outside the network
    void drawOutsideNetwork(bool add);

private:

    /* @brief draw train with individual carriages. The number of carriages is
     * determined from defaultLength of carriages and vehicle length
     * passengerSeats are computed beginning at firstPassengerCarriage */
    void drawAction_drawRailCarriages(const GUIVisualizationSettings& s, SUMOReal defaultLength, SUMOReal carriageGap,
                                      int firstPassengerCarriage, bool asImage) const;
    /// @}

    /// @brief draws the given guiShape if it has distinct carriages/modules and returns true if so
    bool drawAction_drawCarriageClass(const GUIVisualizationSettings& s, SUMOVehicleShape guiShape, bool asImage) const;

    /* @brief return the previous lane in this vehicles route including internal lanes
     * @param[in] current The lane of which the predecessor should be returned
     * @param[in,out] routeIndex The index of the current or previous non-internal edge in the route
     */
    MSLane* getPreviousLane(MSLane* current, int& furtherIndex) const;

    /// @brief return the number of passengers
    int getNumPassengers() const;

    /// @brief add seats to mySeatPositions and update requiredSeats
    void computeSeats(const Position& front, const Position& back, int& requiredSeats) const;

    /// @brief retrieve information about the current stop state
    std::string getStopInfo() const;

};


#endif

/****************************************************************************/

