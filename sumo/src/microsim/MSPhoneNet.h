/****************************************************************************/
/// @file    MSPhoneNet.h
/// @author  Eric Nicolay
/// @date    2006
/// @version $Id$
///
// The cellular network (GSM)
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
#ifndef MSPhoneNet_h
#define MSPhoneNet_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>
#include <map>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class MSPhoneCell;
class MSPhoneLA;


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
    void writeOutput(SUMOTime time);

    /// Adapts the previously loaded statistics
    void setCellStatData(SUMOTime time);


    void addLAChange(const std::string &pos_id);

private:
    void CallReminding(SUMOTime &t);

    /// A map of phone cells
    std::map< int, MSPhoneCell* > _mMSPhoneCells;

    /// A map of location areas
    std::map< int, MSPhoneLA* > _mMSPhoneLAs;

    /// A map from cells to location areas
    std::map< int, int > _mCell2LA;

    /// The used adaptation interval for location areas (!!! fix)
    unsigned int _LAIntervall;

    /// The used adaptation interval for cells (!!! fix)
    unsigned int _CellIntervall;

    std::map< std::string, int > myLAChanges;

    bool percentModus;

};


#endif

/****************************************************************************/

