/****************************************************************************/
/// @file    Graph.h
/// @author  unknown_author
/// @date    Fri, 29.04.2005
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
#ifndef Graph_h
#define Graph_h
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
#include "Vertex.h"
#include "time.h"
#include "stdlib.h"
#include "ConfigDialog.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class Graph
{

private:

    vector <Vertex*> vArray;
    vector <Vertex*>::iterator v;

    vector <Edge*> eArray;
    vector <Edge*>::iterator e;

    vector <Vertex*> pfad;
    vector <Vertex*>::iterator p;

    vector <Vertex*> mArray;
    vector <Vertex*>::iterator m;

public:

    Graph();

    //Methoden für Knoten

    Vertex* AddVertexByXY(int x, int y);

    Vertex* SearchVertex(int x, int y);

    void DelVertex(int x, int y);

    void PushVertex(int index, int xNew, int yNew);

    vector<Vertex*> GetVArray();
    void SetVArray(vector<Vertex*> myarray);
    vector<Vertex*> GetPfadArray();

    vector<Edge*> GetEArray();

    //Methoden für die Kanten

    void AddEdgeByVertex(Vertex* v, Vertex* w);

    Edge* SearchEdge(Vertex* v, Vertex* w);

    void DelEdge(Vertex* v, Vertex* w);

    void DelDoubleEdge(Vertex* v, Vertex* w);

    void ChangeLength(int index_v, int index_w);

    void GetNachfolger(int i);

    //Liefert die Anzahl der Elemente im vArray
    int Number_of_Vertex();

    //Liefert die Anzahl der Elemente im eArray
    int Number_of_Edges();

    //Liefert das Element an Index i im vArray
    Vertex* GetVertex(int i);

    //Liefert das Element an Index i im eArray
    Edge* GetEdge(int i);

    //Holt sich entlang der Kanten eine beliebige Knotenfolge
    void Drive(int i);

    int GetPfad();

    //Entfernt Knoten, die kolineare Kanten verbinden
    void Reduce();
    //Entfernt Knoten, die kolineare Kanten verbinden (besser)
    void Reduce_plus(ConfigDialog* my);

    int GetIndex(Vertex* v);

    //Entfernt überflüssige Kanten
    void Reduce_Edges();

    //Exportiert eine Knotendatei in XML-Format
    void Export_Vertexes_XML();

    //Exportiert eine Kantendatei in XML-Format
    void Export_Edges_XML();

    //Wandelt einen Integer-Wert in einen String um
    char* inttostr(int i);


    //Wandelt einen Double-Wert in einen String um
    char* SUMORealtostr(SUMOReal i,int count);

    //Exportiert Traces
    void GetTraces(int cars,int fuel, ConfigDialog* myDialog);

    //Vereinigt zwei nahe Knoten zu einem
    void MergeVertex();

    //Zwei Hilfmethoden für MergeVertex
    void DelVertex4Merge(Vertex* v);
    void DelNachfolger4Merge(Vertex* v);

};


#endif

/****************************************************************************/

