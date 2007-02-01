/****************************************************************************/
/// @file    Point.cpp
/// @author  unknown_author
/// @date    Tue, 29.05.2005
/// @version $Id: $
///
//
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

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



/****************************************************************************/

