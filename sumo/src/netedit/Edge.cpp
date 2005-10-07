//---------------------------------------------------------------------------//
//                        Edge.cpp -
//  Implement the Edge-class
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
// Revision 1.6  2005/10/07 11:38:33  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/23 06:01:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:03:02  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/09/09 12:51:48  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.5  2005/05/30 08:18:26  dksumo
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "Edge.h"
#include "Vertex.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

/* =========================================================================
 * member method definitions
 * ======================================================================= */

// Konstruktoren (1.leer / 2. Zwei Pointer auf Start- und Endknoten)
Edge::Edge(){}

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

	if(x2>x1){
		distanz_x=x2-x1;
	}
	else distanz_x=x1-x2;



	if(y2>y1){
		distanz_y=y2-y1;
	}
	else distanz_y=y1-y2;

	ergebnis=sqrt((SUMOReal) ((distanz_x * distanz_x) + (distanz_y *distanz_y)));


	return ergebnis;
}

SUMOReal Edge::GetLength()
{
	return length;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
