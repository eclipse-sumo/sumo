#ifndef MSMoveReminder_H
#define MSMoveReminder_H

/**
 * @file   MSMoveReminder.h
 * @author Christian Roessel
 * @date   Wed May 21 10:40:22 2003
 *
 * $Revision$ $Date$ $Author$
 *
 * @brief  Base class for all move-reminders
 *
 */


//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

// $Log$
// Revision 1.5  2003/07/16 15:28:00  dkrajzew
// MSEmitControl now only simulates lanes which do have vehicles; the edges do not go through the lanes, the EdgeControl does
//
// Revision 1.4  2003/06/10 13:43:00  roessel
// Added documentation.
//
// Revision 1.3  2003/06/05 09:58:00  roessel
// Added method getLane() and member laneM.
//
// Revision 1.2  2003/05/22 12:44:47  roessel
// Changed void activateByEmit... to bool isActivatedByEmit. Not activated
// reminders will be erased from the vehicles reminder-list.
//
// Revision 1.1  2003/05/21 16:21:45  dkrajzew
// further work detectors
//

class MSVehicle;
class MSLane;

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
     * @param lane Lane on ehich the reminder will work.
     */
    MSMoveReminder( MSLane* lane ) : laneM( lane ){}

    /**
     * Destructor.
     *
     */
    virtual ~MSMoveReminder( void ) {}

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
    virtual bool isStillActive( MSVehicle& veh,
                                double oldPos,
                                double newPos,
                                double newSpeed ) = 0;

    /**
     *  Informs corresponding detector if vehicle leaves reminder
     *  by lanechange.
     *
     * @param veh The leaving vehicle.
     */
    virtual void dismissByLaneChange( MSVehicle& veh ) = 0;

    /**
     * Informs corresponding detector if vehicle enters the reminder
     * by emit or lanechange.
     *
     * @param veh The entering vehilcle.
     *
     * @return True if vehicle enters the reminder.
     */
    virtual bool isActivatedByEmitOrLaneChange( MSVehicle& veh ) = 0;

    /**
     * The lane the reminder works on.
     *
     * @return The lane the reminder works on.
     */
    MSLane* getLane( void )
        {
            return laneM;
        }

protected:
    MSLane* laneM;              /**< Lane on which the reminder works. */
};


#endif

// Local Variables:
// mode:C++
// End:
