/****************************************************************************/
/// @file    Vertex.cpp
/// @author  Andreas Gaubatz
/// @date    23.11.2004
/// @version $Id$
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Vertex.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// member method definitions
// ===========================================================================

//Konstruktor (Koordinaten)

Vertex::Vertex(int i, int j)
{
    //Setzen der Koordinaten und der Grade
    x=i;
    y=j;
    inDegree=0;
    outDegree=0;

}

//Füge einen Pointer auf einen Knoten in das Nachfolgerarray ein
void
Vertex::AddNachfolger(Vertex* v)
{
    nachfolger.push_back(v);
}

//Füge einen Pointer auf einen Knoten in das Vorgaengerarray ein
void
Vertex::AddVorgaenger(Vertex* v)
{
    vorgaenger.push_back(v);
}

//Lösche den Pointer an Stelle k aus dem Nachfolgerarray
void
Vertex::DelNachfolger(int k)
{
    nachfolger.erase(nachfolger.begin()+k);
}

//Lösche den Pointer an Stelle k aus dem Vorgaengerarray
void
Vertex::DelVorgaenger(int k)
{
    vorgaenger.erase(vorgaenger.begin()+k);
}

//Hole den Pointer an Stelle i in dem Nachfolgerarray
Vertex* Vertex::GetNachfolgeVertex(int i)
{
    return nachfolger[i];
}

//Hole den Pointer an Stelle i in dem Vorgaengerarray
Vertex* Vertex::GetVorgaengerVertex(int i)
{
    return vorgaenger[i];
}

//Lösche die Arrays(1.Nachfolgerarray / 2.Vorgaengerarray )
void
Vertex::DelNachfolgeArray()
{
    nachfolger.clear();
}

void
Vertex::DelVorgaengerArray()
{
    vorgaenger.clear();
}

//Liefert die Anzahl der Elemente im Array zurück

size_t
Vertex::GetNachfolger()
{
    return nachfolger.size();
}

size_t
Vertex::GetVorgaenger()
{
    return vorgaenger.size();
}

//Liefert die Anzahl der Elemente im Array zurück (Ende)

//Liefert die Koordinaten des Knoten
int Vertex::GetX()
{
    return x;
}

int Vertex::GetY()
{
    return y;
}
//Liefert die Koordinaten des Knoten (Ende)

//Liefert den Index des Knotens
int Vertex::GetId()
{
    return id;
}

//Liefert den Grad (Gesamt/Eingangsgrad/Ausgangsgrad) des Knotens
int Vertex::GetDegree()
{
    return inDegree+outDegree;
}

int Vertex::GetInDegree()
{
    return inDegree;
}

int Vertex::GetOutDegree()
{
    return outDegree;
}
//Liefert den Grad (Gesamt/Eingangsgrad/Ausgangsgrad) des Knotens (Ende)

//Setzt die Koordinaten des Knoten
void
Vertex::SetX(int i)
{
    x=i;
}

void
Vertex::SetY(int j)
{
    y=j;
}
//Setzt die Koordinaten des Knoten (Ende)

//Setzt den Index des Knotens
void
Vertex::SetId(int n)
{
    id=n;
}

//Erhöhung der Grade des Knotens
void
Vertex::InkrementDegree()
{
    degree++;
}

void
Vertex::InkrementInDegree()
{
    inDegree++;
}

void
Vertex::InkrementOutDegree()
{
    outDegree++;
}
//Erhöhung der Grade des Knotens (Ende)

//Verringerung der Grade des Knotens
void
Vertex::DekrementDegree()
{
    degree--;
}

void
Vertex::DekrementInDegree()
{
    inDegree--;
}

void
Vertex::DekrementOutDegree()
{
    outDegree--;
}
//Verringerung der Grade des Knotens (Ende)

//Wandelt die Koordinaten des Knotens in GPS-Koordinaten um

void
Vertex::px2gps(int scale, int gkr, int gkh)
{
    //Berechnet zu x,y-Koordinaten die GK-Koordinaten
    //bei zwei gegebenen GPS-Eckpunkten(der betrachteten Karte)
    SUMOReal rm, e2, c, bI, bII, bf, co, g2, g1, t, fa, dl, gb, gl;
    int mKen;
    SUMOReal gkx=(SUMOReal) gkr+x*scale;//2601000.25+x*2;
    SUMOReal gky=(SUMOReal) gkh+x*scale;//5711999.75-y*2;
    const SUMOReal rho = (SUMOReal)(180/3.1415926535897932384626433832795);
    e2 = (SUMOReal) 0.0067192188;
    c = (SUMOReal) 6398786.849;
    mKen = (int)(gkx / 1000000);
    rm = gkx - mKen * 1000000 - 500000;
    bI = gky / (SUMOReal) 10000855.7646;
    bII = bI * bI;
    bf = (SUMOReal) 325632.08677 * bI *(((((((SUMOReal) 0.00000562025 * bII - (SUMOReal) 0.00004363980) * bII + (SUMOReal) 0.00022976983) * bII - (SUMOReal) 0.00113566119) * bII + (SUMOReal) 0.00424914906) * bII - (SUMOReal) 0.00831729565) * bII + 1);
    bf = bf / 3600 / rho;
    co = cos(bf);
    g2 = e2 * (co * co);
    g1 = c / sqrt(1 + g2);
    t = sin(bf) / cos(bf); // tan(bf)
    fa = rm / g1;
    gb = bf - fa * fa * t * (1 + g2) / 2 + fa * fa * fa * fa * t * (5 + 3 * t * t + 6 * g2 - 6 * g2 * t * t) / 24;
    gb = gb * rho;
    dl = fa - fa * fa * fa * (1 + 2 * t * t + g2) / 6 + fa * fa * fa * fa * fa * (1 + 28 * t * t + 24 * t * t * t * t) / 120;
    gl = dl * rho / co + mKen * 3;
    lat = gb;
    lon = gl;
}

//Hole den Longitude-Wert der GPS-Koordinaten
SUMOReal Vertex::GetGPSLon()
{
    return lon;
}

//Hole den Latitude-Wert der GPS-Koordinaten
SUMOReal Vertex::GetGPSLat()
{
    return lat;
}



/****************************************************************************/

