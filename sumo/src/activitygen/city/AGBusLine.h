#ifndef AGBUSLINE_H
#define AGBUSLINE_H

#include <iostream>
#include <string>
#include <list>
#include "AGBus.h"
#include "AGPosition.h"
#include "AGDataAndStatistics.h"

class AGBusLine
{
public:
	AGBusLine(std::string lineNr) :
		lineNumber(lineNr)
	{};
	void setMaxTripTime(int time);
	void setBusNames();
	int nbrBuses();
	void locateStation(AGPosition pos);
	void generateBuses(int start, int stop, int rate);
	void printBuses();

	std::list<AGPosition> stations;
	std::list<AGBus> buses;
	std::list<AGBus> revBuses;

private:
	/**
	 * @return: a name for a new Bus. unique for the city.
	 */
	std::string createName();

	/**
	 * @param time: time of departure of the bus in one direction (current time)
	 * @return: time at which it will be ready for going in the opposite direction
	 */
	int getReady(int time);

	std::string lineNumber;
	int maxTripTime;
	int busNbr;
};

#endif

