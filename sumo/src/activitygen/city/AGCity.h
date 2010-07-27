#ifndef AGCITY_H
#define AGCITY_H

#include <iostream>
#include <vector>
#include <list>
#include <router/RONet.h>
#include "AGPosition.h"
#include "AGDataAndStatistics.h"
#include "AGSchool.h"
#include "AGBusLine.h"
#include "AGWorkPosition.h"
#include "AGHousehold.h"

class AGHousehold;

class AGCity
{
public:
	AGCity() :
	  statData()
	  {};

	/**
	 * generates streets: complete the "streets" vector using the DataAndStat's map edges.
	 */
	void completeStreets();
	void generateWorkPositions();
	void completeBusLines();
	//void generateSchools();
	void generatePopulation();
	void schoolAllocation();
	void workAllocation();
	void carAllocation();

	/**
	 * manipulation functions
	 */
	AGStreet* getStreet(std::string edge);
	/**
	 * returns a random street
	 */
	AGStreet* getRandomStreet();

	AGDataAndStatistics statData;
	std::vector<AGStreet> streets;
	std::vector<AGWorkPosition> workPositions;
	std::list<AGSchool> schools;
	std::list<AGBusLine> busLines;
	std::list<AGHousehold> households;
	std::list<AGPosition> cityGates;
	std::list<AGAdult> peopleIncoming;

private:
	AGSchool closestSchoolTo(AGPosition pos);
	/**
	 * generates workpositions on the city's gates (entrances) for the outgoing work traffic.
	 */
	void generateOutgoingWP();
	/**
	 * generates people from outside the city for incoming traffic generation
	 */
	void generateIncomingPopulation();

	int NrStreets;
	int nbrCars;
};

#endif
