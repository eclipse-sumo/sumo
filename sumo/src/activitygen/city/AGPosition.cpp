#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <utils/common/RandHelper.h>
#include "AGPosition.h"

using namespace std;

void
AGPosition::print()
{
	cout << "- AGPosition: *Street=" << street << " position=" << position << "/" << street->getLength() << endl;
}

bool
AGPosition::operator==(const AGPosition &pos)
{
	if(street == pos.street && position < pos.position+0.1 && position > pos.position-0.1)
		return true;
	else
		return false;
}

float
AGPosition::getDistanceTo(AGPosition *otherPos)
{
	return pos2d.distanceTo(otherPos->pos2d);
}

float
AGPosition::getDistanceTo(AGPosition otherPos)
{
	return getDistanceTo(&otherPos);
}

float
AGPosition::getDistanceTo(list<AGPosition> *myPositions)
{
	float minDist = std::numeric_limits<float>::infinity();
	float tempDist;
	list<AGPosition>::iterator itt;

	for(itt=myPositions->begin() ; itt!=myPositions->end() ; ++itt)
	{
		tempDist = this->getDistanceTo(&*itt);
		if(tempDist < minDist)
		{
			minDist = tempDist;
		}
	}
	return minDist;
}

float
AGPosition::getDistanceTo(map<int, AGPosition> *myPositions)
{
	float minDist = std::numeric_limits<float>::infinity();
	float tempDist;
	map<int, AGPosition>::iterator itt;

	for(itt=myPositions->begin() ; itt!=myPositions->end() ; ++itt)
	{
		tempDist = this->getDistanceTo(&(itt->second));
		if(tempDist < minDist)
		{
			minDist = tempDist;
		}
	}
	return minDist;
}

float
AGPosition::randomPositionInStreet()
{
	int len = (int)(*street).getLength();
	position = (float)RandHelper::rand(len);//( rand() % len);
	return position;
}

void
AGPosition::compute2dPosition()
{
	// P = From + pos*(To - From) = pos*To + (1-pos)*From
	Position2D From = street->net->getEdge(street->getName())->getFromNode()->getPosition();
	Position2D To = street->net->getEdge(street->getName())->getToNode()->getPosition();

	pos2d.set(0,0);
	pos2d.add(To);
	pos2d.sub(From);
	pos2d.mul(position/street->getLength());
	pos2d.add(From);

}
