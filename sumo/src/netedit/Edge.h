// Edge.h: Schnittstelle für die Klasse Edge.
//
//////////////////////////////////////////////////////////////////////

#ifndef _EDGE_H_
#define _EDGE_H_

// Bekanntmachen der Klasse Vertex
class Vertex;

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
};

#endif
//_EDGE_H

