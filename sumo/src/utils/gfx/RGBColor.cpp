//---------------------------------------------------------------------------//
//                        RGBColor.cpp -
//  A RGB-color definition
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.3  2003/07/16 15:38:51  dkrajzew
// handling of colors improved
//
// Revision 1.2  2003/02/07 10:50:53  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "RGBColor.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
RGBColor::RGBColor()
    : myRed(-1), myGreen(-1), myBlue(-1)
{
}


RGBColor::RGBColor(double red, double green, double blue)
    : myRed(red), myGreen(green), myBlue(blue)
{
}


RGBColor::~RGBColor()
{
}


double
RGBColor::red() const
{
    return myRed;
}


double
RGBColor::green() const
{
    return myGreen;
}


double
RGBColor::blue() const
{
    return myBlue;
}


std::ostream &
operator<<(std::ostream &os, const RGBColor &col)
{
    os
        << col.myRed << ","
        << col.myGreen << ","
        << col.myBlue;
    return os;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "RGBColor.icc"
//#endif

// Local Variables:
// mode:C++
// End:


