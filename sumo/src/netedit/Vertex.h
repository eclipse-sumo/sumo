/****************************************************************************/
/// @file    Vertex.h
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
#ifndef Vertex_h
#define Vertex_h
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

#include <vector>
#include "Edge.h"
#include "math.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class Vertex
{

private:

    //Koordinaten, Grad, Eingangsgrad und Ausgangsgrad des Knoten
    int x,y,id,degree,inDegree,outDegree;

    //Gauß-Krüger Koordinaten und GPS-Koordinaten(Longitude,Latitude)
    SUMOReal gkx,gky,lon,lat;

    // Dynamisches Array, welches alle Pointer auf Nachfolgerknoten enthält
    std::vector <Vertex*> nachfolger;
    // Dynamisches Array, welches alle Pointer auf Vorgaengerknoten enthält
    std::vector <Vertex*> vorgaenger;

public:

    //Konstruktoren (Koordinaten)
    Vertex(int i, int j);

    //Füge einen Pointer auf einen Knoten in das Nachfolgerarray ein
    void AddNachfolger(Vertex* v);
    //Füge einen Pointer auf einen Knoten in das Vorgaengerarray ein
    void AddVorgaenger(Vertex* v);

    //Lösche den Pointer an Stelle k aus dem Nachfolgerarray
    void DelNachfolger(int k);
    //Lösche den Pointer an Stelle k aus dem Vorgaengerarray
    void DelVorgaenger(int k);

    //Hole den Pointer an Stelle i in dem Nachfolgerarray
    Vertex* GetNachfolgeVertex(int i);
    //Hole den Pointer an Stelle i in dem Vorgaengerarray
    Vertex* GetVorgaengerVertex(int i);

    //Lösche die Arrays(1.Nachfolgerarray / 2.Vorgaengerarray / 3.Array der Inzidenten Kanten)
    void DelNachfolgeArray();
    void DelVorgaengerArray();

    //Liefert die Anzahl der Elemente im Array zurück
    size_t GetNachfolger();
    size_t GetVorgaenger();

    //Liefert die Koordinaten des Knoten
    int GetX();
    int GetY();

    //Liefert den Index des Knotens
    int GetId();

    //Liefert den Grad (Gesamt/Eingangsgrad/Ausgangsgrad) des Knotens
    int GetDegree();
    int GetInDegree();
    int GetOutDegree();

    //Setzt die Koordinaten des Knoten
    void SetX(int i);
    void SetY(int j);

    //Setzt den Index des Knotens
    void SetId(int n);

    //Erhöhung der Grade des Knotens
    void InkrementDegree();
    void InkrementInDegree();
    void InkrementOutDegree();

    //Verringerung der Grade des Knotens
    void DekrementDegree();
    void DekrementInDegree();
    void DekrementOutDegree();


    //Wandelt die Koordinaten des Knotens in GPS-Koordinaten um
    void px2gps(int scale, int gkr, int gkh);
    //Hole den Longitude-Wert der GPS-Koordinaten
    SUMOReal GetGPSLon();
    //Hole den Latitude-Wert der GPS-Koordinaten
    SUMOReal GetGPSLat();

};


#endif

/****************************************************************************/

