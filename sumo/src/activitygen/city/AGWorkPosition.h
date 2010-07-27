#ifndef AGWORKPOSITION_H
#define AGWORKPOSITION_H

#include <iostream>
#include "AGStreet.h"
#include "AGPosition.h"
#include "AGDataAndStatistics.h"
#include "AGAdult.h"

class AGAdult;

class AGWorkPosition
{
public:
	AGWorkPosition(AGStreet *inStreet, int opening, int closing, AGDataAndStatistics* ds) :
			location(inStreet),
			openingTime(opening),
			closingTime(closing),
			taken(false),
			ds(ds)
		{
			ds->workPositions++;
		};
	AGWorkPosition(AGStreet *inStreet, AGDataAndStatistics* ds) :
			location(inStreet),
			openingTime(generateOpeningTime(ds)),
			closingTime(generateClosingTime(ds)),
			taken(false),
			ds(ds)
		{
			ds->workPositions++;
		};
	AGWorkPosition(AGStreet *inStreet, float pos, AGDataAndStatistics* ds) :
			location(inStreet, pos),
			openingTime(generateOpeningTime(ds)),
			closingTime(generateClosingTime(ds)),
			taken(false),
			ds(ds)
		{
			ds->workPositions++;
		};
	~AGWorkPosition() {
		let();
	}
	void print();
	int generateOpeningTime(AGDataAndStatistics* ds);
	int generateClosingTime(AGDataAndStatistics* ds);
	bool take(AGAdult* ad);
	bool let();
	bool isTaken();
	AGPosition getPosition();
	int getOpening();
	int getClosing();

private:
	AGDataAndStatistics* ds;
	AGAdult* adult;
	AGPosition location;
	int openingTime;
	int closingTime;
	bool taken;
};

#endif

