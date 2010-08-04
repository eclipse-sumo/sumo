// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>
#include "AGStreet.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGStreet::print()
{
	cout << "- AGStreet: Name=" << edgeName << " Length=" << length << " pop=" << popDensity << " work=" << workDensity << endl;
}

float
AGStreet::getLength()
{
	return length;
}

string
AGStreet::getName()
{
	return edgeName;
}

int
AGStreet::getPopDensity()
{
	return this->popDensity;
}

int
AGStreet::getWorkDensity()
{
	return this->workDensity;
}

void
AGStreet::setPopulation(int pop)
{
	popDensity = pop;
}

void
AGStreet::setWorkPositions(int work)
{
	workDensity = work;
}

void
AGStreet::computeLength()
{
	length = net->getEdge(edgeName)->getLength();
}

/****************************************************************************/
