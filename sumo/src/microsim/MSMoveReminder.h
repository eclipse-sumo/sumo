/****************************************************************************/
/// @file    MSMoveReminder.h
/// @author  Christian Roessel
/// @date    2003-05-21
/// @version $Id$
///
//	»missingDescription«
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
#ifndef MSMoveReminder_h
#define MSMoveReminder_h



// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <cassert>
#include "MSLane.h"
class MSVehicle;


/**
 * Base class of all move-reminders. During move, the vehicles call
 * isStillActive(), if they enter the reminder during emit and
 * lanechange isActivatedByEmitOrLaneChange() and if they leave the
 * reminder by lanechange they call dismissByLaneChange(). The
 * reminder knows whom to tell about move, emit and lanechange. The
 * vehicles will remove the reminder that is not isStillActive() from
 * their reminder container.
 *
 */
class MSMoveReminder
{
public:
    /**
     * Sole constructor.
     *
     * @param lane Lane on which the reminder will work.
     */
    MSMoveReminder(MSLane* lane) :
            laneM(lane)
    {
        // add reminder to lane
        laneM->addMoveReminder(this);
    }

    /**
     * Destructor.
     *
     */
    virtual ~MSMoveReminder(void)
    {}

    /**
     * Indicator if the reminders is still active for the passed
     * vehicle/parameters. If false, the vehicle will erase this reminder
     * from it's reminder-container. This method will pass all neccessary
     * data to the corresponding detector.
     *
     * @param veh Vehicle that asks this remider.
     * @param oldPos Position before move.
     * @param newPos Position after move with newSpeed.
     * @param newSpeed Moving speed.
     *
     * @return True if vehicle hasn't passed the detector completely.
     */
    virtual bool isStillActive(MSVehicle& veh,
                               SUMOReal oldPos,
                               SUMOReal newPos,
                               SUMOReal newSpeed) = 0;

    /**
     *  Informs corresponding detector if vehicle leaves reminder
     *  by lanechange.
     *
     * @param veh The leaving vehicle.
     */
    virtual void dismissByLaneChange(MSVehicle& veh) = 0;

    /**
     * Informs corresponding detector if vehicle enters the reminder
     * by emit or lanechange.
     *
     * @param veh The entering vehilcle.
     *
     * @return True if vehicle enters the reminder.
     */
    virtual bool isActivatedByEmitOrLaneChange(MSVehicle& veh) = 0;


    /**
     * The lane the reminder works on.
     *
     * @return The lane the reminder works on.
     */
    const MSLane* getLane(void) const
    {
        return laneM;
    }

protected:
    MSLane* laneM;              /**< Lane on which the reminder works. */

};


#endif

/****************************************************************************/

