#ifndef MSLaneStateReminder_H
#define MSLaneStateReminder_H

/**
 * @file   MSLaneStateReminder.h
 * @author Christian Roessel
 * @date   Started Wed May 21 10:50:44 2003
 *
 * $Revision$ from $Date$ by $Author$.
 *
 * @brief Declarations of class MSLaneStateReminder.
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
// Revision 1.7  2003/06/10 13:24:07  roessel
// Added documentation.
//
// Revision 1.6  2003/06/04 16:20:04  roessel
// Moved modified code from .h to .cpp.
//
// Revision 1.5  2003/05/28 11:19:51  roessel
// Changed ctor argument from reference to pointer.
// Added private default-ctor, copy-ctor, assignement-operator declarations.
//
// Revision 1.4  2003/05/27 17:27:15  dkrajzew
// unworking default constructor overriding removed
//
// Revision 1.3  2003/05/22 13:45:34  roessel
// Added condition in isActivatedByEmitOrLaneChange.
//
// Revision 1.2  2003/05/22 12:44:10  roessel
// Changed void activateByEmit... to bool isActivatedByEmit. Not activated
// reminders will be erased from the vehicles reminder-list.
//
// Revision 1.1  2003/05/21 16:21:45  dkrajzew
// further work detectors
//

#include "MSMoveReminder.h"
class MSLaneState;
class MSLane;

/**
 * A kind of a MSMoveReminder that reminds all vehicles on the reminder to
 * pass data to the corresponding MSLaneState detector.
 * 
 */
class MSLaneStateReminder : public MSMoveReminder
{
public:
    /** 
     * Sole constructor.
     * 
     * @param startPos Start-position of the reminder.
     * @param endPos  End-position of the reminder.
     * @param ls Corresponding MSLaneState object.
     * @param l MSLane the reminder works on
     */
    MSLaneStateReminder( double startPos,
                         double endPos,
                         MSLaneState* ls,
                         MSLane* l ) :
        MSMoveReminder( l ),
        startPosM( startPos ),
        endPosM( endPos ),
        laneStateM( ls )
        {}

    /// Destructor.
    ~MSLaneStateReminder( void )
        {}
    /** 
     * Indicator if the reminders is still active for the passed
     * vehicle/parameters. If false, the vehicle will erase this reminder
     * from it's reminder-container. This method will pass all neccessary
     * data to laneStateM. 
     * 
     * @param veh Vehicle that asks this remider.
     * @param oldPos Position before move.
     * @param newPos Position after move with newSpeed.
     * @param newSpeed Moving speed.
     * 
     * @return True if vehicle hasn't passed the detector completely.
     */
    bool isStillActive( MSVehicle& veh,
                        double oldPos,
                        double newPos,
                        double newSpeed );
    /** 
     * Informs laneStateM if vehicle leaves reminder/detector by lanechange.
     * 
     * @param veh The leaving vehicle.
     */
    void dismissByLaneChange( MSVehicle& veh );
    
    /** 
     * Informs laneStateM if vehicle enters the reminder/detector by emit
     * or lanechange.
     * 
     * @param veh The entering vehilcle.
     * 
     * @return True if vehicle enters reminder/detector in front of endPosM.
     */
    bool isActivatedByEmitOrLaneChange( MSVehicle& veh );
    
private:
    double startPosM;           /**< Reminders start-position. */
    
    double endPosM;             /**< Reminders end-position. */
    
    MSLaneState* laneStateM;    /**< Corresponding MSLaneState */

    /// Default constructor.
    MSLaneStateReminder(); 

    /// Hidden copy constructor.      
    MSLaneStateReminder( const MSLaneStateReminder& );

    /// Hidden assignment operator.
    MSLaneStateReminder& operator=( const MSLaneStateReminder& );
};



#endif

// Local Variables:
// mode:C++
// End:
