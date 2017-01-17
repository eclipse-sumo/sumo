/****************************************************************************/
/// @file    GUIInductLoop.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Aug 2003
/// @version $Id$
///
// The gui-version of the MSInductLoop, together with the according
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
#ifndef GUIInductLoop_h
#define GUIInductLoop_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/foxtools/MFXMutex.h>
#include <microsim/output/MSInductLoop.h>
#include <utils/geom/Position.h>
#include "GUIDetectorWrapper.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIInductLoop
 * @brief The gui-version of the MSInductLoop.
 *
 * Allows the building of a wrapper (also declared herein) which draws the
 *  detector on the gl-canvas. Uses a mutex to avoid parallel read/write operations.
 *  The mutex is only set within methods that change MSInductLoop-internal state
 *  and within "collectVehiclesOnDet". All other reading operations should be performed
 *  via the simulation loop only.
 */
class GUIInductLoop : public MSInductLoop {
public:
    /**
     * @brief Constructor.
     * @param[in] id Unique id
     * @param[in] lane Lane where detector woks on
     * @param[in] position Position of the detector within the lane
     * @param[in] vTypes which vehicle types are considered
     */
    GUIInductLoop(const std::string& id, MSLane* const lane, SUMOReal position, const std::string& vTypes);


    /// @brief Destructor
    ~GUIInductLoop();


    /** @brief Resets all generated values to allow computation of next interval
     *
     * Locks the internal mutex before calling MSInductLoop::reset()
     * @see MSInductLoop::reset()
     */
    void reset();


    /** @brief Returns this detector's visualisation-wrapper
     * @return The wrapper representing the detector
     */
    virtual GUIDetectorWrapper* buildDetectorGUIRepresentation();


    /** @brief Returns vehicle data for vehicles that have been on the detector starting at the given time
     *
     * This method uses a mutex to prevent parallel read/write access to the vehicle buffer
     *
     * @param[in] t The time from which vehicles shall be counted
     * @param[in] leaveTime Whether entryTime or leaveTime shall be compared against t
     *            (the latter gives a more complete picture but may include vehicles in multiple steps even if they did not stay on the detector)
     * @return The list of vehicles
     * @see MSInductLoop::collectVehiclesOnDet()
     */
    std::vector<VehicleData> collectVehiclesOnDet(SUMOTime t, bool leaveTime = false) const;


protected:
    /// @name Methods that add and remove vehicles from internal container
    /// @{

    /** @brief Introduces a vehicle to the detector's map myVehiclesOnDet.
     *
     * Locks the internal mutex before calling MSInductLoop::enterDetectorByMove()
     * @see MSInductLoop::enterDetectorByMove()
     * @param veh The entering vehicle.
     * @param entryTimestep Timestep (not necessary integer) of entrance.
     * @see MSInductLoop::enterDetectorByMove()
     */
    void enterDetectorByMove(SUMOVehicle& veh, SUMOReal entryTimestep);


    /** @brief Processes a vehicle that leaves the detector
     *
     * Locks the internal mutex before calling MSInductLoop::leaveDetectorByMove()
     * @see MSInductLoop::leaveDetectorByMove()
     * @param veh The leaving vehicle.
     * @param leaveTimestep Timestep (not necessary integer) of leaving.
     * @see MSInductLoop::leaveDetectorByMove()
     */
    void leaveDetectorByMove(SUMOVehicle& veh, SUMOReal leaveTimestep);


    /** @brief Removes a vehicle from the detector's map myVehiclesOnDet.
     *
     * Locks the internal mutex before calling MSInductLoop::leaveDetectorByLaneChange()
     * @see MSInductLoop::leaveDetectorByLaneChange()
     * @param veh The leaving vehicle.
     * @param lastPos The last position of the leaving vehicle.
     */
    void leaveDetectorByLaneChange(SUMOVehicle& veh, SUMOReal lastPos);
    /// @}




public:
    /**
     * @class GUIInductLoop::MyWrapper
     * @brief A MSInductLoop-visualiser
     */
    class MyWrapper : public GUIDetectorWrapper {
    public:
        /// @brief Constructor
        MyWrapper(GUIInductLoop& detector, SUMOReal pos);

        /// @brief Destructor
        ~MyWrapper();


        /// @name inherited from GUIGlObject
        //@{

        /** @brief Returns an own parameter window
         *
         * @param[in] app The application needed to build the parameter window
         * @param[in] parent The parent window needed to build the parameter window
         * @return The built parameter window
         * @see GUIGlObject::getParameterWindow
         */
        GUIParameterTableWindow* getParameterWindow(
            GUIMainWindow& app, GUISUMOAbstractView& parent);


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


        /// @brief Returns the detector itself
        GUIInductLoop& getLoop();


    private:
        /// @brief The wrapped detector
        GUIInductLoop& myDetector;

        /// @brief The detector's boundary
        Boundary myBoundary;

        /// @brief The position in full-geometry mode
        Position myFGPosition;

        /// @brief The rotation in full-geometry mode
        SUMOReal myFGRotation;

        /// @brief The position on the lane
        SUMOReal myPosition;

    private:
        /// @brief Invalidated copy constructor.
        MyWrapper(const MyWrapper&);

        /// @brief Invalidated assignment operator.
        MyWrapper& operator=(const MyWrapper&);

    };


    /// @brief Mutex preventing parallel read/write access to internal MSInductLoop state
    mutable MFXMutex myLock;

};


#endif

/****************************************************************************/

