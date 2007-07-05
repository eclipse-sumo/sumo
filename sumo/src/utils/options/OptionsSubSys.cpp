/****************************************************************************/
/// @file    OptionsSubSys.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 23.06.2003
/// @version $Id$
///
// Static methods for options initialisation and parsing
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
#include <fstream>
#include <algorithm>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/StringTokenizer.h>
#include "OptionsSubSys.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
OptionsCont OptionsSubSys::myOptions;


// ===========================================================================
// method definitions
// ===========================================================================
OptionsCont &
OptionsSubSys::getOptions()
{
    return myOptions;
}


void
OptionsSubSys::close()
{
    myOptions.clear();
}


bool
OptionsSubSys::helper_CSVOptionMatches(const std::string &optionName,
                                       const std::string &itemName)
{
    if (myOptions.isSet(optionName)) {
        vector<string> values = myOptions.getStringVector(optionName);
        return find(values.begin(), values.end(), itemName)!=values.end();
    }
    return false;
}



/****************************************************************************/

