/****************************************************************************/
/// @file    MSPhoneNet.h
/// @author  Eric Nicolay
/// @date    2006
/// @version $Id$
///
// The cellular network (GSM)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSPhoneNet_h
#define MSPhoneNet_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>
#include <map>
#include <string>
#include "MSPhoneCell.h"

// ===========================================================================
// class declarations
// ===========================================================================
class MSPhoneLA;
extern std::map<MSVehicle *, MSPhoneCell*> LastCells;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPhoneNet
 * @brief The cellular network (GSM)
 */
class MSPhoneNet
{
public:
    /// Constructor
    MSPhoneNet();

    // Destructor
    ~MSPhoneNet();

    /// Returns the named location area
    MSPhoneLA* getMSPhoneLA(int id);

    /// Adds a named location area
    void addMSPhoneLA(int id , int dir);

    /// Removes a named location area
    void remMSPhoneLA(int id);

    /// Returns the named cell
    MSPhoneCell* getMSPhoneCell(int id);

    /// Adds a named cell
    void addMSPhoneCell(int id);

    /// Adds a named cell (!!!?)
    void addMSPhoneCell(int id , int la);

    /// Removes a named cell
    void remMSPhoneCell(int id);


    /** @brief Returns the cell in which the named vehicle is located
     *
     * The cell is determined by checking whether the call of the vehicle
     * (named as the vehicle) is within the cell
     */
    MSPhoneCell* getCurrentVehicleCell(const std::string &id);

    /** @brief Returns the location area in which the named vehicle is located
     *
     * The cell is determined by checking whether the call of the vehicle
     * (named as the vehicle) is within the location area
     */
    MSPhoneLA* getCurrentVehicleLA(const std::string &id);

    /// ?!!!
    void connectLA2Cell(int cell_ID, int la_ID);

    /// ?!!!
    SUMOTime writeCellOutput(SUMOTime time);
    SUMOTime writeLAOutput(SUMOTime time);
    void setDynamicCalls(SUMOTime time);


    void addLAChange(const std::string &pos_id);

    void removeVehicle(MSVehicle &veh, SUMOTime t) {
        if (LastCells.find(&veh)!=LastCells.end()) {
            MSPhoneCell *cell = LastCells[&veh];
            LastCells.erase(LastCells.find(&veh));
            assert(cell!=0);
            if (cell!=0) {
                cell->removeVehicle(veh, t);
            }
        }
    }

private:
    void CallReminding(SUMOTime &t);

    /// A map of phone cells
    std::map< int, MSPhoneCell* > myMMSPhoneCells;

    /// A map of location areas
    std::map< int, MSPhoneLA* > myMMSPhoneLAs;

    /// A map from cells to location areas
    std::map< int, int > myMCell2LA;

    std::map< std::string, int > myLAChanges;
    SUMOTime lastTime;
    bool percentModus;


};


#endif

/****************************************************************************/

