#ifndef AGACTIVITIES_H
#define AGACTIVITIES_H

#include <list>
#include "AGTrip.h"
#include "../city/AGCity.h"
#include "../city/AGBusLine.h"
#include "../city/AGHousehold.h"

class AGActivities
{
public:
	AGActivities(AGCity *city, int days) :
		myCity(city),
		nbrDays(days)
	{};
	void addTrip(AGTrip t, std::list<AGTrip> *tripSet);
	void addTrips(std::list<AGTrip> t, std::list<AGTrip> *tripSet);
	void generateActivityTrips();

	/**
	 * trips contains trips as well for one day as for every day,
	 * these trips will be regenerated with small variations
	 * by ActivityGen at the end of the simulation
	 * before generating the trip file
	 */
	std::list<AGTrip> trips;

private:
	bool generateTrips(AGHousehold &hh);
	bool generateBusTraffic(AGBusLine bl);
	bool generateInOutTraffic();

	/**
	 * generates names for incoming cars, given there unique number
	 */
	std::string generateIncomingName(int i);
	
	AGCity *myCity;

	int nbrDays;

};

#endif

