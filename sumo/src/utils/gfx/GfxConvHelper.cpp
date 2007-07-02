/****************************************************************************/
/// @file    GfxConvHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2003
/// @version $Id$
///
// Some helper functions to help when parsing graphical information
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

#include <string>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include "GfxConvHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RGBColor
GfxConvHelper::parseColor(const std::string &coldef)
{
    StringTokenizer st(coldef, ",");
    if(st.size()<3) {
        throw EmptyData();
    }
    SUMOReal r = TplConvert<char>::_2SUMOReal(st.next().c_str());
    SUMOReal g = TplConvert<char>::_2SUMOReal(st.next().c_str());
    SUMOReal b = TplConvert<char>::_2SUMOReal(st.next().c_str());
    return RGBColor(r, g, b);
}



/****************************************************************************/

