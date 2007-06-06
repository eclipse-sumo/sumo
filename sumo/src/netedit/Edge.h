/****************************************************************************/
/// @file    Edge.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// missing_desc
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
#ifndef Edge_h
#define Edge_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class Vertex;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class Edge
{

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
    void SetEnd(int e);

    // Setzt die Anzahl der Fahrspuren
    void SetTracks(int t);

    // Setzt die zulässige Höchstgeschwindigkeit
    void SetSpeed(int sp);

    //Setze die Länge der Kante
    SUMOReal Setlength();

    //Hole die Länge der Kante
    SUMOReal GetLength();
};


#endif

/****************************************************************************/

