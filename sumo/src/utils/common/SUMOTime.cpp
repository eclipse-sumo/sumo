/****************************************************************************/
/// @file    SUMOTime.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Variables, methods, and tools for internal time representation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <sstream>
#include "SUMOTime.h"
#include "TplConvert.h"


// ===========================================================================
// type definitions
// ===========================================================================
#ifdef HAVE_SUBSECOND_TIMESTEPS
SUMOTime DELTA_T = 1000;
#endif


// ===========================================================================
// method definitions
// ===========================================================================
SUMOTime
string2time(const std::string &r) throw(EmptyData, NumberFormatException) {
    size_t idx = r.find('.');
    if (idx==std::string::npos) {
        // no sub-seconds
        return TplConvert<char>::_2int(r.c_str())*1000;
    }
    int secs = idx>0 ? TplConvert<char>::_2int(r.substr(0, idx).c_str()) : 0;
    int subsecs = TplConvert<char>::_2int((r.substr(idx+1)+"0000").substr(0,3).c_str()) * 10;
    do {
        subsecs = subsecs / 10;
    } while (idx<r.length()&&r[idx]=='0');
    return secs*1000 + subsecs;
}


std::string
time2string(SUMOTime t) throw() {
    std::ostringstream oss;
    oss << ((SUMOReal) t / 1000.);
    std::string ret = oss.str();
    size_t idx = ret.find('.');
    if (idx==std::string::npos) {
        return ret + ".00";
    }
    return (ret+"00").substr(0, idx+3);
}


/****************************************************************************/

