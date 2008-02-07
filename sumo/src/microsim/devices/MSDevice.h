/****************************************************************************/
/// @file    MSDevice.h
/// @author  Michael Behrisch, Daniel Krajzewicz
/// @date    Tue, 04 Dec 2007
/// @version $Id$
///
// Abstract in-vehicle device
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
#ifndef MSDevice_h
#define MSDevice_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <microsim/MSVehicle.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice
 * @brief Abstract in-vehicle device
 */
class MSDevice
{
public:
    MSDevice(MSVehicle &holder) throw()
            : myHolder(holder) {
    }


    virtual ~MSDevice() throw() { }

    MSVehicle &getHolder() const throw() {
        return myHolder;
    }

    const std::string &getID() {
        if (myID=="") {
            buildID();
        }
        return myID;
    }

    /** Update of members if vehicle enters a new lane in the move step.
        @param Pointer to the entered Lane. */
    virtual void enterLaneAtMove(MSLane* enteredLane, SUMOReal driven,
                                 bool inBetweenJump=false) { }

    /** Update of members if vehicle enters a new lane in the emit step.
        @param Pointer to the entered Lane. */
    virtual void enterLaneAtEmit(MSLane* enteredLane, const MSVehicle::State &state) { }

    /** Update of members if vehicle enters a new lane in the laneChange step.
        @param Pointer to the entered Lane. */
    virtual void enterLaneAtLaneChange(MSLane* enteredLane) { }

    /** Update of members if vehicle leaves a new lane in the move step. */
    virtual void leaveLaneAtMove(SUMOReal driven) { }

    /** Update of members if vehicle leaves a new lane in the
        laneChange step. */
    virtual void leaveLaneAtLaneChange(void) { }

    virtual void onTripEnd() { }

    SUMOReal getEffort(const MSEdge * const e, SUMOTime t) const {
        return -1;
    }

protected:
    virtual std::string buildID() = 0;

    MSVehicle &myHolder;

private:
    std::string myID;

private:
    /// @brief Invalidated copy constructor.
    MSDevice(const MSDevice&);

    /// @brief Invalidated assignment operator.
    MSDevice& operator=(const MSDevice&);

};


#endif

/****************************************************************************/

