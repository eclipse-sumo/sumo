#ifndef MSPHONELA_H
#define MSPHONELA_H
/***************************************************************************
                          MSPhoneLA.h  -
    A location area of a cellular network (GSM)
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
// $Log$
// Revision 1.7  2006/12/01 09:14:41  dkrajzew
// debugging cell phones
//
// Revision 1.6  2006/11/28 12:15:40  dkrajzew
// documented TOL-classes and made them faster
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

#include "MSCORN.h"
#include <string>
#include <vector>
#include <map>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSPhoneLA
 * @brief A location area of a cellular network (GSM)
 */
class MSPhoneLA{
public:
    /// Constructor
    MSPhoneLA(int pos_id, int dir);

    /// Destructor
    ~MSPhoneLA();

    /// !!!?
    void addCall( );

    /// Adds (starts) a named vehicle call
    void addCall(const std::string &id);

    /// Returns the information whether the named call takes place in this la
    bool hasCall(const std::string &id);

    /// Removes a named call
    void remCall(const std::string &id);

    /// !!!?
    void writeOutput( SUMOTime t );

    /// !!!?
    void writeSQLOutput( SUMOTime t );

    /// comparison operator
    bool operator==( MSPhoneLA* );

    /// comparison operator
    bool operator!=( MSPhoneLA* );

    /// Returns the number of vehicles calling within this cell
    size_t getVehicleNumber() const { return _Calls.size(); }

private:
    int last_time;
    int position_id;
    int dir; //direction
    int sum_changes;
    int quality_id;
    int intervall;
	std::map<std::string, int> _Calls;
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:


