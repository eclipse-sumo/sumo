#ifndef AGCAR_H
#define AGCAR_H

#include <iostream>
#include <string>
#include "AGAdult.h"

class AGCar
{
public:
	AGCar(std::string name) :
		idName(name)
	{};
	AGCar() :
		idName(createName())
	{};
	bool associateTo(AGAdult *pers);
	bool isAssociated();
	std::string getName();

private:
	std::string createName();

	std::string idName;
	AGAdult* currentUser;

};

#endif
 
