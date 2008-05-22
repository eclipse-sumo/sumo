/****************************************************************************/
/// @file    GUIInternalLane.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 04.09.2003
/// @version $Id$
///
// Representation of a lane over a junction
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIInternalLane_h
#define GUIInternalLane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utility>
#include <microsim/MSInternalLane.h>
#include <microsim/MSEdge.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/foxtools/MFXMutex.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class MSNet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIInternalLane
 * An extended MSInternalLane. A mechanism to avoid concurrent
 * visualisation and simulation what may cause problems when vehicles
 * disappear is implemented using a mutex
 */
class GUIInternalLane : public MSInternalLane
{
public:
    /** @brief Constructor 
     *
     * @param[in] id The lane's id
     * @param[in] maxSpeed The speed allwoed on this lane
     * @param[in] length The lane's length
     * @param[in] edge The edge this lane belongs to
     * @param[in] numericalID The numerical id of the lane
     * @param[in] allowed Vehicle classes that explicitely may drive on this lane
     * @param[in] disallowed Vehicle classes that are explicitaly forbidden on this lane
     * @see SUMOVehicleClass
     * @see MSLane
     * @see MSInternalLane
     */
    GUIInternalLane(const std::string &id, SUMOReal maxSpeed,
        SUMOReal length, MSEdge * const edge, unsigned int numericalID,
        const Position2DVector &shape,
        const std::vector<SUMOVehicleClass> &allowed,
        const std::vector<SUMOVehicleClass> &disallowed) throw();


    /// @brief Destructor
    ~GUIInternalLane() throw();



    /// @name Vehicle emission
    ///@{

    /** @brief Tries to emit the given vehicle with the given state (speed and pos)
     *
     * Locks the lock, calls MSLane::isEmissionSuccess keeping the result,
     *  unlocks the lock and returns the result.
     *
     * @param[in] vehicle The vehicle to emit
     * @param[in] speed The speed with which it shall be emitted
     * @param[in] pos The position at which it shall be emitted
     * @param[in] recheckNextLanes Forces patching the speed for not being too fast on next lanes
     * @return Whether the vehicle could be emitted
     * @see MSLane::isEmissionSuccess
     */
    virtual bool isEmissionSuccess(MSVehicle* vehicle, SUMOReal speed, SUMOReal pos,
        bool recheckNextLanes) throw();
    ///@}



    /// @name Vehicle movement (longitudinal)
    /// @{

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool moveNonCritical();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool moveCritical();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool setCritical(std::vector<MSLane*> &into);

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool integrateNewVehicle();
    ///@}

    /// allows the processing of vehicles for threads
    void releaseVehicles();

    /// returns the vehicles closing their processing for other threads
    const VehCont &getVehiclesSecure();

    GUILaneWrapper *buildLaneWrapper(GUIGlObjectStorage &idStorage);

    void detectCollisions(SUMOTime timestep);
    MSVehicle *removeFirstVehicle();
    MSVehicle *removeVehicle(MSVehicle *remVehicle);

protected:
    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool push(MSVehicle* veh);

    MSVehicle* pop();

    /// moves myTmpVehicles int myVehicles after a lane change procedure
    void swapAfterLaneChange();

private:
    /// The mutex used to avoid concurrent updates of the vehicle buffer
    mutable MFXMutex myLock;

};


#endif

/****************************************************************************/

