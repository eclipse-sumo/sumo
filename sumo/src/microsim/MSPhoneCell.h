#ifndef MSPHONECELL_H
#define MSPHONECELL_H
/***************************************************************************
                          MSPhoneCell.h  -
    A cell of a cellular network (GSM)
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
// Revision 1.8  2006/12/01 09:14:41  dkrajzew
// debugging cell phones
//
// Revision 1.7  2006/11/28 12:15:39  dkrajzew
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

#include <string>
#include <vector>
#include <map>
#include "MSCORN.h"

enum CallType{
	STATICIN,
	STATICOUT,
	DYNIN,
	DYNOUT
};

struct Call{
	CallType ct;
//	std::string id;
};


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSPhoneCell
 * @brief A cell of a cellular network (GSM)
 */
class MSPhoneCell{
public:
    /// Constructor
    MSPhoneCell(int ID);

    /// Destructor
    ~MSPhoneCell();

    /// Adds a call (?!!!)
    void addCall(const std::string &id, CallType);

    /// Removes a named call from the cell
    void remCall(const std::string &id);

    /// Returns the information whether the named call takes place in this cell
    bool hasCall(const std::string &id);

    /// !!!?
    int  getIntervall();

    /// Sets the number of static calls for the given period
    void setStatParams(int interval, int statcallcount);

    /// Sets the parameter of dynamic calls for the given period
    void setDynParams(int interval, int count, float duration, float deviation);

    /// !!!?
    void writeOutput(SUMOTime time);

    /// !!!?
    void writeSQLOutput(SUMOTime time);

    /// !!!?
    void setnextexpectData(SUMOTime time);

    /// !!!?
    int getExpectDynCallCount() { return dyncallcount; }

    /// !!!?
    void decrementDynCallCount(){ --dyncallcount; }

    /// Returns the number of vehicles calling within this cell
    size_t getVehicleNumber() const { return _Calls.size(); }

private:
    int Cell_Id;
    int Calls_In;
    int Calls_Out;
    int Dyn_Calls_In;
    int Dyn_Calls_Out;
    int Sum_Calls;
    int Intervall;
    int last_time;
	int statcallcount;
	int dyncallcount;
	float dynduration;
	float dyndeviation;

    struct DynParam {
        int count;
        float duration;
        float deviation;
    };

    std::map<std::string, Call> _Calls;
	std::vector<std::pair<int, int > > vexpectStatCount;
	std::vector<std::pair<int, int > >::iterator itCount;
	std::vector<std::pair<int, DynParam> > vexpectDuration;
	std::vector<std::pair<int, DynParam> >::iterator itDuration;
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:


