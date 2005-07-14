// Point.h: Schnittstelle für die Klasse Point.
//
//////////////////////////////////////////////////////////////////////

#ifndef _POINT_H_
#define _POINT_H_

class Point
{

private:

    //Koordinaten
    int x;
    int y;

public:

    //Konstruktor
    Point(int i, int j);

    //Hole die X-Koordinate des Punktes
    int GetX();

    //Hole die Y-Koordinate des Punktes
    int GetY();

    //Setze die X-Koordinate des Punktes
    void SetX(int i);

    //Setze die Y-Koordinate des Punktes
    void SetY(int j);
};

#endif
//_POINT_H_
