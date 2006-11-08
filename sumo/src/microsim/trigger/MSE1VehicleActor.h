#ifndef MSE1VehicleActor_H
#define MSE1VehicleActor_H
//---------------------------------------------------------------------------//
//                        MSE1VehicleActor.h -
//  An actor which changes a vehicle's state
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 23.03.2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Id$
// $Log$
// Revision 1.3  2006/11/08 16:27:51  ericnicolay
// change code for the cell-actor
//
// Revision 1.2  2006/07/05 11:23:39  ericnicolay
// add code for change state of cphones and register them to the cells and las
//
// Revision 1.1  2006/03/27 07:19:47  dkrajzew
// vehicle actors added
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <deque>
#include <map>
#include <functional>
#include <microsim/MSMoveReminder.h>
#include <microsim/trigger/MSTrigger.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <utils/iodevices/XMLDevice.h>
#include <microsim/devices/MSDevice_CPhone.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;
class GUIDetectorWrapper;
class GUIGlObjectStorage;
class GUILaneWrapper;

enum ActorType{
	LA,
	CELL
};

/* =========================================================================
 * class definitions
 * ======================================================================= */
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
     * @param deleteDataAfterSeconds Dismiss-time for collected data.
     */
    MSE1VehicleActor( const std::string& id, MSLane* lane,
        SUMOReal positionInMeters, unsigned int laid, unsigned int cellid, unsigned int type );


    /// Destructor. Clears containers.
    ~MSE1VehicleActor();


    /**
     * @name MSMoveReminder methods.
     *
     * Methods in this group are inherited from MSMoveReminder. They are
     * called by the moving, entering and leaving vehicles.
     *
     */
    //@{
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
     * @see enterDetectorByMove
     * @see leaveDetectorByMove
     */
    bool isStillActive( MSVehicle& veh,
                        SUMOReal oldPos,
                        SUMOReal newPos,
                        SUMOReal newSpeed );


    /**
     *  Informs corresponding detector via leaveDetectorByLaneChange()
     *  if vehicle leaves by lanechange.
     *
     * @param veh The leaving vehicle.
     *
     * @see leaveDetectorByLaneChange
     */
    void dismissByLaneChange( MSVehicle& veh );

    /**
     * Informs corresponding detector if vehicle enters the reminder
     * by emit or lanechange. Only vehicles that are completely in
     * front of the detector will return true.
     *
     * @param veh The entering vehilcle.
     *
     * @return True if vehicle is on or in front of the detector.
     */
    bool isActivatedByEmitOrLaneChange( MSVehicle& veh );
    //@}

protected:
    const SUMOReal posM;          /**< Detector's position on lane [cells]. */
	//const ActorType _type;
	unsigned int _LAId;
	unsigned int _AreaId;
	unsigned int _ActorType;
private:

    /// Hidden default constructor.
    MSE1VehicleActor();

    /// Hidden copy constructor.
    MSE1VehicleActor( const MSE1VehicleActor& );

    /// Hidden assignment operator.
    MSE1VehicleActor& operator=( const MSE1VehicleActor& );
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

