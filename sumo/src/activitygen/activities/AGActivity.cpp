#include <utils/common/RandHelper.h>
#include "AGActivity.h"
#include "../city/AGTime.h"

using namespace std;

bool
AGActivity::isGenerated()
{
	return genDone;
}

bool
AGActivity::generateTrips() {
	return true;
}

int
AGActivity::possibleTranspMean(AGPosition destination)
{
	int FOOT = 1;
	int BUS = 2;
	int CAR = 4;

	int transp = 0;

	if(destination.getDistanceTo(hh->getPosition()) <= ds->maxFootDistance)
	{
		transp = FOOT;
		if(hh->getCarNbr() != 0)
			transp += CAR;
		if(destination.getDistanceTo(&(ds->busStations)) <= ds->maxFootDistance
				&& hh->getPosition().getDistanceTo(&(ds->busStations)) <= ds->maxFootDistance)
			transp += BUS;
	}
	else if(hh->getCarNbr() == 0)
	{
		float d1 = destination.getDistanceTo(hh->getPosition());
		float d2 = destination.getDistanceTo(&(ds->busStations)) + hh->getPosition().getDistanceTo(&(ds->busStations));

		if(d1 > d2)
		{
			transp = BUS;
		}
		else
		{
			transp = FOOT;
		}
	}
	else if(hh->getCarNbr() != 0) //all other cases
	{
		if(destination.getDistanceTo(&(ds->busStations)) > ds->maxFootDistance
				|| hh->getPosition().getDistanceTo(&(ds->busStations)) > ds->maxFootDistance)
		{
			transp = CAR;
		}
		else
		{
			transp = CAR + BUS;
		}
	}
	return transp;
}

int
AGActivity::availableTranspMeans(AGPosition from, AGPosition to)
{
	int FOOT = 1;
	int BUS = 2;

	int available = 0;

	if(from.getDistanceTo(to) <= ds->maxFootDistance)
	{
		available += FOOT;
	}
	if(from.getDistanceTo(&(ds->busStations)) <= ds->maxFootDistance
			&& to.getDistanceTo(&(ds->busStations)) <= ds->maxFootDistance)
	{
		available += BUS;
	}
	return available;
}

int
AGActivity::timeToDrive(AGPosition from, AGPosition to)
{
	float dist = from.getDistanceTo(to);
	return (int) (timePerKm * dist / 1000.0);
}

int
AGActivity::depHour(AGPosition from, AGPosition to, int arrival)
{
	// ?? departure.addDays(1); // in case of negative time: arrival < timeToDrive
	//departure.setDay(0); // days are set to 0 because we want the time in the current day
	return (arrival - timeToDrive(from, to));
}

int
AGActivity::arrHour(AGPosition from, AGPosition to, int departure)
{
	return (departure + timeToDrive(from, to));
}

int
AGActivity::randomTimeBetween(int begin, int end)
{
	if( 0 > begin || begin > end)
		return -1;
	if(begin == end)
		return begin;
	int tAlea = RandHelper::rand(end - begin);
	return (begin + tAlea);
}







