#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include "Vertex.h"
#include "time.h"
#include "stdlib.h"

using namespace std;

class Graph{

private:

    vector <Vertex*> vArray;
    vector <Vertex*>::iterator v;

    vector <Edge*> eArray;
    vector <Edge*>::iterator e;

    vector <Vertex*> pfad;
    vector <Vertex*>::iterator p;

public:

    Graph();

    //Methoden für Knoten

    void AddVertexByXY(int x, int y);

    Vertex* SearchVertex(int x, int y);

    void DelVertex(int x, int y);

    void PushVertex(int index, int xNew, int yNew);

    vector<Vertex*> GetVArray();

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
    void drive(int i);

    int GetPfad();

    //Entfernt Knoten, die kolineare Kanten verbinden
    void Reduce();
    //Entfernt Knoten, die kolineare Kanten verbinden (besser)
    void Reduce_plus();

    int GetIndex(Vertex* v);

    void Reduce_Edges();

    void Export_Vertexes_XML();

    void Export_Edges_XML();

    char* inttostr(int i);
};

#endif // _GRAPH_H_
