#ifndef _EDGE_H_
#define _EDGE_H_
//---------------------------------------------------------------------------//
//                        EDGE.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// $Log$
// Revision 1.4  2005/09/23 06:01:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/09/15 12:03:02  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/06/14 11:22:27  dksumo
// documentation added
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * class declarations
 * ======================================================================= */

class Vertex;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class Edge{

private:
    // Pointer auf Start- und Endknoten
    Vertex* starting;
    Vertex* ending;

    // ???
    int start;
    int end;

    // Anzahl der Fahrspuren
    int tracks;
    // Zulässige Höchstgeschwindigkeit
    int speed;
	//Länge der Kante(Euklidischer Abstand von Start- und Endknoten)
	SUMOReal length;

public:

    // Konstruktoren (1.leer / 2. Zwei Pointer auf Start- und Endknoten)
    Edge();
    Edge(Vertex* v, Vertex* w);

    // Liefert einen Pointer auf den Startknoten der Kante
    Vertex* GetStartingVertex();
    // Liefert einen Pointer auf den Endknoten der Kante
    Vertex* GetEndingVertex();

    // ???
    int GetStart();
    int GetEnd();

    // Liefert die Anzahl der Fahrspuren
    int GetTracks();

    // Liefert die zulässige Höchstgeschwindigkeit
    int GetSpeed();

    // ???
    void SetStart(int s);
    void SetEnd (int e);

    // Setzt die Anzahl der Fahrspuren
    void SetTracks (int t);

    // Setzt die zulässige Höchstgeschwindigkeit
    void SetSpeed(int sp);

	//Setze die Länge der Kante
	SUMOReal Setlength();

	//Hole die Länge der Kante
	SUMOReal GetLength();
};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
