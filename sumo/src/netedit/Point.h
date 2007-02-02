/****************************************************************************/
/// @file    Point.h
/// @author  Andreas Gaubatz
/// @date    23.11.2004
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
#ifndef Point_h
#define Point_h
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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class Point
{

private:

    //Koordinaten
    int x;
    int y;

public:

    //Konstruktor
    Point(int i, int j);

    //Hole die X-Koordinate des Punktes
    int GetX();

    //Hole die Y-Koordinate des Punktes
    int GetY();

    //Setze die X-Koordinate des Punktes
    void SetX(int i);

    //Setze die Y-Koordinate des Punktes
    void SetY(int j);
};


#endif

/****************************************************************************/

