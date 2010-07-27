#ifndef POSITION_H
#define POSITION_H

#include <iostream>
#include <list>
#include <map>
#include "AGStreet.h"
#include "../../utils/geom/Position2D.h"

class AGPosition
{
public:
	AGPosition(AGStreet *str, float pos) :
	  street(str), position(pos), pos2d()
	  {
		compute2dPosition();
	  };
	AGPosition(AGStreet *str) :
	  street(str), position((float)randomPositionInStreet()), pos2d()
	  {
		compute2dPosition();
	  };
	
	bool operator==(const AGPosition &pos);

	void print();
	float getDistanceTo(AGPosition *otherPos);
	float getDistanceTo(AGPosition otherPos);
	float getDistanceTo(std::list<AGPosition> *positions);
	float getDistanceTo(std::map<int, AGPosition> *myPositions);

	AGStreet *street;
	float position;
	Position2D pos2d;

private:
	float randomPositionInStreet();
	void compute2dPosition();


};

#endif

