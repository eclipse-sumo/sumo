#ifndef MSPHONENET_H
#define MSPHONENET_H
/***************************************************************************
                          MSPhoneNet.h  -
    The cellular network (GSM)
                             -------------------
    begin                : 2006
    copyright            : (C) 2006 by DLR http://www.dlr.de
    author               : Eric Nicolay
    email                : Eric.Nicolay@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log: MSPhoneNet.h,v $
// Revision 1.5  2006/11/28 12:15:40  dkrajzew
// documented TOL-classes and made them faster
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

#include <utils/common/SUMOTime.h>
#include <map>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSPhoneCell;
class MSPhoneLA;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSPhoneNet
 * @brief The cellular network (GSM)
 */
class MSPhoneNet{
public:
    /// Constructor
    MSPhoneNet();

    // Destructor
    ~MSPhoneNet();

    /// Returns the named location area
    MSPhoneLA* getMSPhoneLA( int id );

    /// Adds a named location area
    void addMSPhoneLA( int id , int dir);

    /// Removes a named location area
    void remMSPhoneLA( int id );

    /// Returns the named cell
    MSPhoneCell* getMSPhoneCell( int id );

    /// Adds a named cell
    void addMSPhoneCell( int id );

    /// Adds a named cell (!!!?)
	void addMSPhoneCell( int id , int la);

    /// Removes a named cell
    void remMSPhoneCell( int id );


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
	void connectLA2Cell( int cell_ID, int la_ID );

    /// ?!!!
	void writeOutput( SUMOTime time );

    /// Adapts the previously loaded statistics
	void setCellStatData(SUMOTime time);


    void addLAChange( std::string &pos_id );

private:
    void CallReminding( SUMOTime &t);

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

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

