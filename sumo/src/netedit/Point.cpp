// Point.cpp: Implementierung der Klasse Point.
//
//////////////////////////////////////////////////////////////////////

// Diese Klasse stellt einen Punkt in der Ebene dar

#include "Point.h"

//Konstruktor
Point::Point(int i, int j)
{
    //Koordinaten setzen
    x=i;
    y=j;
}

//Hole die X-Koordinate des Punktes
int Point::GetX()
{
    return x;
}

//Hole die Y-Koordinate des Punktes
int Point::GetY()
{
    return y;
}

//Setze die X-Koordinate des Punktes
void
Point::SetX(int i)
{
    x=i;
}

//Setze die Y-Koordinate des Punktes
void
Point::SetY(int j)
{
    y=j;
}

