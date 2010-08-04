#ifndef AGSCHOOL_H
#define AGSCHOOL_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include "AGPosition.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGSchool
{
public:
	AGSchool(int capacity_, AGPosition pos, int beginAge, int endAge, int open, int close) :
		beginAge(beginAge),
		endAge(endAge),
		  capacity(capacity_),
		  initCapacity(capacity_),
		  location(pos),
		  opening(open),
		  closing(close)
	  {};
	void print();
	int getPlaces();
	bool addNewChild();
	bool removeChild();
	int getBeginAge();
	int getEndAge();
	bool acceptThisAge(int age);
	AGPosition getPosition();
	int getClosingHour();
	int getOpeningHour();

private:
	int beginAge, endAge;
	int capacity;
	int initCapacity;
	AGPosition location;
	int opening, closing;
};

#endif

/****************************************************************************/
