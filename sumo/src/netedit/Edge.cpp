// Edge.cpp: Implementierung der Klasse Edge.
//
//////////////////////////////////////////////////////////////////////

// Diese Klasse stellt eine Kante in einem Graphen dar

#include "Edge.h"

// Konstruktoren (1.leer / 2. Zwei Pointer auf Start- und Endknoten)
Edge::Edge(){}

Edge::Edge(Vertex* v, Vertex* w)
{
    //Setze Start- und Endknoten
    starting=v;
    ending=w;
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


