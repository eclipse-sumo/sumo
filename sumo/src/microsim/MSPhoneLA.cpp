/****************************************************************************/
/// @file    MSPhoneLA.cpp
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include "MSPhoneLA.h"
#include "MSNet.h"
#include <utils/common/StringUtils.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSPhoneLA::MSPhoneLA(int pos_id, int d)
        : last_time(0), position_id(pos_id), dir(d),
        sum_changes(0), quality_id(0), intervall(0)
{}


MSPhoneLA::~MSPhoneLA()
{}


void
MSPhoneLA::addCall()
{
    assert(true);
    sum_changes++;
}


void
MSPhoneLA::addCall(const std::string &id)
{
    _Calls[id] = 1;
    ++sum_changes;
}


void
MSPhoneLA::remCall(const std::string &id)
{
    std::map<std::string, int>::iterator icalls = _Calls.find(id);
    if (icalls!=_Calls.end()) {
        _Calls.erase(icalls);
    }
}


bool
MSPhoneLA::hasCall(const std::string &id)
{
    return _Calls.find(id)!=_Calls.end();
}


bool
MSPhoneLA::operator ==(MSPhoneLA * other)
{
    if (position_id==other->position_id && dir==other->dir) {
        return true;
    } else {
        return false;
    }
}


bool
MSPhoneLA::operator !=(MSPhoneLA * other)
{
    if (position_id!=other->position_id || dir!=other->dir) {
        return true;
    } else {
        return false;
    }
}


void
MSPhoneLA::writeOutput(SUMOTime t)
{
    intervall = t - last_time;
    {
        OutputDevice *od = MSNet::getInstance()->getOutputDevice(MSNet::OS_LA_TO_SS2);
        if (od!=0) {
            std::string timestr= OptionsCont::getOptions().getString("device.cell-phone.sql-date");
            timestr = timestr + " " + StringUtils::toTimeString(t);
            od->getOStream()
            << "03;" << ';' << timestr << ';' << position_id << ';' << dir << ';' << sum_changes
            << ';' << quality_id << ';' << intervall << "\n";
        }
    }
    {
        OutputDevice *od = MSNet::getInstance()->getOutputDevice(MSNet::OS_LA_TO_SS2_SQL);
        if (od!=0) {
            std::string timestr= OptionsCont::getOptions().getString("device.cell-phone.sql-date");
            timestr = timestr + " " + StringUtils::toTimeString(t);
            if (od->getBoolMarker("hadFirstCall")) {
                od->getOStream() << "," << endl;
            } else {
                od->setBoolMarker("hadFirstCall", true);
            }
            od->getOStream()
            << "(NULL, \' \', '" << timestr << "'," << position_id << ',' << dir << ',' << sum_changes << ','
            << quality_id << ',' << intervall << ")";
        }
    }
    last_time = t;
    sum_changes = 0;
}



/****************************************************************************/

