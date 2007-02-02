/****************************************************************************/
/// @file    Edge.cpp
/// @author  Andreas Gaubatz
/// @date    23.11.2004
/// @version $Id: $
///
// Implement the Edge-class
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

//
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

#include "Edge.h"
#include "Vertex.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

// ===========================================================================
// member method definitions
// ===========================================================================

// Konstruktoren (1.leer / 2. Zwei Pointer auf Start- und Endknoten)
Edge::Edge()
{}

Edge::Edge(Vertex* v, Vertex* w)
{
    //Setze Start- und Endknoten
    starting=v;
    ending=w;
    length=Setlength();
}

// Liefert einen Pointer auf den Startknoten der Kante
Vertex* Edge::GetStartingVertex()
{
    return starting;
}

// Liefert einen Pointer auf den Endknoten der Kante
Vertex* Edge::GetEndingVertex()
{
    return ending;
}

// ???
int Edge::GetStart()
{
    return start;
}

// ???
int Edge::GetEnd()
{
    return end;
}

// Liefert die Anzahl der Fahrspuren
int Edge::GetTracks()
{
    return tracks;
}

// Liefert die zulässige Höchstgeschwindigkeit
int Edge::GetSpeed()
{
    return speed;
}

// ???
void
Edge::SetStart(int s)
{
    start=s;
}

// ???
void
Edge::SetEnd(int e)
{
    end=e;
}

// Setzt die Anzahl der Fahrspuren
void
Edge::SetTracks(int t)
{
    tracks=t;
}

// Setzt die zulässige Höchstgeschwindigkeit
void
Edge::SetSpeed(int sp)
{
    speed=sp;
}

SUMOReal Edge::Setlength()
{
    SUMOReal ergebnis;

    Vertex* ptemp = starting;
    Vertex* qtemp = ending;
    int x1 = ptemp->GetX();
    int y1 = ptemp->GetY();
    int x2 = qtemp->GetX();
    int y2 = qtemp->GetY();
    int distanz_x=0;
    int distanz_y=0;

    if (x2>x1) {
        distanz_x=x2-x1;
    } else distanz_x=x1-x2;



    if (y2>y1) {
        distanz_y=y2-y1;
    } else distanz_y=y1-y2;

    ergebnis=sqrt((SUMOReal)((distanz_x * distanz_x) + (distanz_y *distanz_y)));


    return ergebnis;
}

SUMOReal Edge::GetLength()
{
    return length;
}



/****************************************************************************/

