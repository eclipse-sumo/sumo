//---------------------------------------------------------------------------//
//                        Point.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                :
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
// Revision 1.2  2005/09/15 12:03:02  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/09/09 12:51:48  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.3  2005/05/30 08:18:26  dksumo
// comments added
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Point.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

//Konstruktor
Point::Point(int i, int j)
{
    //Koordinaten setzen
    x=i;
    y=j;
}

//Hole die X-Koordinate des Punktes
int Point::GetX()
{
    return x;
}

//Hole die Y-Koordinate des Punktes
int Point::GetY()
{
    return y;
}

//Setze die X-Koordinate des Punktes
void
Point::SetX(int i)
{
    x=i;
}

//Setze die Y-Koordinate des Punktes
void
Point::SetY(int j)
{
    y=j;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
