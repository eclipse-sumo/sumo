//---------------------------------------------------------------------------//
//                        GfxConvHelper.cpp -
//  Some helper functions to help when parsing graphical information
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2005/04/28 09:02:48  dkrajzew
// level3 warnings removed
//
// Revision 1.2  2003/12/09 11:35:00  dkrajzew
// documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/StringTokenizer.h>
#include <utils/convert/TplConvert.h>
#include "GfxConvHelper.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
RGBColor
GfxConvHelper::parseColor(const std::string &coldef)
{
    StringTokenizer st(coldef, ",");
    double r = TplConvert<char>::_2float(st.next().c_str());
    double g = TplConvert<char>::_2float(st.next().c_str());
    double b = TplConvert<char>::_2float(st.next().c_str());
    return RGBColor(r, g, b);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

