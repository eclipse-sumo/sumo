// Edge.cpp: Implementierung der Klasse Edge.
//
//////////////////////////////////////////////////////////////////////

// Diese Klasse stellt eine Kante in einem Graphen dar

#include "Edge.h"
#include "Vertex.h"

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

double Edge::Setlength()
{
	double ergebnis;

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

	ergebnis=sqrt((double) ((distanz_x * distanz_x) + (distanz_y *distanz_y)));


	return ergebnis;
}

double Edge::GetLength()
{
	return length;
}
