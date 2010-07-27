#include <iostream>
#include <sstream>
#include <string>
#include "AGCar.h"
#include "AGAdult.h"

using namespace std;

string AGCar::createName()
{
	std::ostringstream os;
	os << this;
	return "c" + os.str();
}

bool AGCar::associateTo(AGAdult *pers)
{
	if(currentUser == NULL)
	{
		currentUser = pers;
		return true;
	}
	return false;
}

bool AGCar::isAssociated()
{
	return (currentUser != NULL);
}

string
AGCar::getName()
{
	return idName;
}
