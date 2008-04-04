/****************************************************************************/
/// @file    MSPhoneLA.h
/// @author  Eric Nicolay
/// @date    2006
/// @version $Id$
///
// A location area of a cellular network (GSM)
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
#ifndef MSPhoneLA_h
#define MSPhoneLA_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSCORN.h"
#include <string>
#include <vector>
#include <map>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPhoneLA
 * @brief A location area of a cellular network (GSM)
 */
class MSPhoneLA
{
public:
    /// Constructor
    MSPhoneLA(int pos_id, int dir);

    /// Destructor
    ~MSPhoneLA();

    /// !!!?
    void addCall();

    /// Adds (starts) a named vehicle call
    void addCall(const std::string &id);

    /// Returns the information whether the named call takes place in this la
    bool hasCall(const std::string &id);

    /// Removes a named call
    void remCall(const std::string &id);

    /// !!!?
    void writeOutput(SUMOTime t);

    /// comparison operator
    bool operator==(MSPhoneLA*);

    /// comparison operator
    bool operator!=(MSPhoneLA*);

    int getPositionId() {
        return position_id;
    };
    /// Returns the number of vehicles calling within this cell
    size_t getVehicleNumber() const {
        return myCalls.size();
    }

private:
    SUMOTime last_time;
    int position_id;
    int dir; //direction
    int sum_changes;
    int quality_id;
    SUMOTime intervall;
    std::map<std::string, int> myCalls;
};


#endif

/****************************************************************************/

