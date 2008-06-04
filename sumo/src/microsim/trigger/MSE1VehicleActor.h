/****************************************************************************/
/// @file    MSE1VehicleActor.h
/// @author  Daniel Krajzewicz
/// @date    23.03.2006
/// @version $Id$
///
// An actor which changes a vehicle's state
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
#ifndef MSE1VehicleActor_h
#define MSE1VehicleActor_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <deque>
#include <map>
#include <functional>
#include <microsim/MSMoveReminder.h>
#include <microsim/trigger/MSTrigger.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <microsim/devices/MSDevice_CPhone.h>


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class MSE1VehicleActor
 */
class MSLane;
class GUIDetectorWrapper;
class GUIGlObjectStorage;
class GUILaneWrapper;

enum ActorType {
    LA,
    CELL
};

extern std::map<MSVehicle *, MSPhoneCell*> LastCells;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 */
class MSE1VehicleActor : public MSMoveReminder, public MSTrigger
{
public:
    /// Type of the dictionary where all MSE1VehicleActor are registered.

    /**
     * Constructor. Adds object into a SingletonDictionary. Sets old-data
     * removal event. Adds reminder to MSLane.
     *
     * @param id Unique id.
     * @param lane Lane where detector woks on.
     * @param position Position of the detector within the lane.
     */
    MSE1VehicleActor(const std::string& id, MSLane* lane,
                     SUMOReal positionInMeters, unsigned int laid, unsigned int cellid, unsigned int type) throw();


    /// Destructor. Clears containers.
    ~MSE1VehicleActor() throw();


    /// @name Methods inherited from MSMoveReminder.
    /// @{

    /**
     * Indicator if the reminders is still active for the passed
     * vehicle/parameters. If false, the vehicle will erase this
     * reminder from it's reminder-container. This method will
     * determine the entry- and leave-time of the counted vehicle and
     * pass this information to the methods enterDetectorByMove() and
     * eaveDetectorByMove().
     *
     * @param veh Vehicle that asks this remider.
     * @param oldPos Position before move.
     * @param newPos Position after move with newSpeed.
     * @param newSpeed Moving speed.
     *
     * @return True if vehicle hasn't passed the detector completely.
     *
     * @see MSMoveReminder
     * @see MSMoveReminder::isStillActive
     * @see enterDetectorByMove
     * @see leaveDetectorByMove
     */
    bool isStillActive(MSVehicle& veh,
                       SUMOReal oldPos,
                       SUMOReal newPos,
                       SUMOReal newSpeed) throw();


    /**
     *  Informs corresponding detector via leaveDetectorByLaneChange()
     *  if vehicle leaves by lanechange.
     *
     * @param veh The leaving vehicle.
     *
     * @see leaveDetectorByLaneChange
     */
    void dismissByLaneChange(MSVehicle& veh) throw();

    /**
     * Informs corresponding detector if vehicle enters the reminder
     * by emit or lanechange. Only vehicles that are completely in
     * front of the detector will return true.
     *
     * @param veh The entering vehilcle.
     *
     * @return True if vehicle is on or in front of the detector.
     */
    bool isActivatedByEmitOrLaneChange(MSVehicle& veh) throw();
    /// @}

    /// Returns the number of vehicles that have passed this actor
    unsigned int getPassedVehicleNumber() const {
        return myPassedVehicleNo;
    }

    /// Returns the number of mobile phones that have passed this actor
    unsigned int getPassedCPhoneNumber() const {
        return myPassedCPhonesNo;
    }

    /// Returns the number of mobile phones that have passed this actor being in connected mode
    unsigned int getPassedConnectedCPhoneNumber() const {
        return myPassedConnectedCPhonesNo;
    }

protected:
    const SUMOReal posM;          /**< Detector's position on lane [cells]. */
    //const ActorType myType;
    unsigned int myLAId;
    unsigned int myAreaId;
    unsigned int myActorType;

    /// The number of vehicles that have passed this actor
    unsigned int myPassedVehicleNo;

    /// The number of mobile phones that have passed this actor
    unsigned int myPassedCPhonesNo;

    /// The number of mobile phones that have passed this actor being in connected mode
    unsigned int myPassedConnectedCPhonesNo;

    bool percentOfActivity;


private:
    /// @brief Invalidated copy constructor.
    MSE1VehicleActor(const MSE1VehicleActor&);

    /// @brief Invalidated assignment operator.
    MSE1VehicleActor& operator=(const MSE1VehicleActor&);


};


#endif

/****************************************************************************/

