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

	vector <Vertex*> mArray;
	vector <Vertex*>::iterator m;

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
	
	//Entfernt überflüssige Kanten
    void Reduce_Edges();

	//Exportiert eine Knotendatei in XML-Format
    void Export_Vertexes_XML();
	
	//Exportiert eine Kantendatei in XML-Format
    void Export_Edges_XML();

	//Wandelt einen Integer-Wert in einen String um
    char* inttostr(int i);

	//Wandelt einen Double-Wert in einen String um
	char* doubletostr(double i,int count);
	
	//Exportiert Traces
	GetTraces(int cars,int fuel);
	
	//Vereinigt zwei nahe Knoten zu einem
	MergeVertex();

	//Zwei Hilfmethoden für MergeVertex
	DelVertex4Merge(Vertex* v);
	DelNachfolger4Merge(Vertex* v);

};

#endif // _GRAPH_H_
