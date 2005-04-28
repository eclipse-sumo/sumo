//---------------------------------------------------------------------------//
//                        GeomConvHelper.h -
//  Some helping functions for geometry parsing
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
// Revision 1.2  2003/12/09 11:32:50  dkrajzew
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
#include <utils/geom/Position2DVector.h>
#include <utils/common/StringTokenizer.h>
#include <utils/convert/TplConvert.h>
#include "GeomConvHelper.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
Position2DVector
GeomConvHelper::parseShape(const std::string &shpdef)
{
    StringTokenizer st(shpdef, " ");
    Position2DVector shape;
    while(st.hasNext()) {
        StringTokenizer pos(st.next(), ",");
        float x = TplConvert<char>::_2float(pos.next().c_str());
        float y = TplConvert<char>::_2float(pos.next().c_str());
        shape.push_back(
            Position2D(x, y));
    }
    return shape;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

