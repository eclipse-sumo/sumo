#include <iostream>
#include <cstdlib>
#include <ctime>
#include "AGPerson.h"

using namespace std;

void
AGPerson::print()
{
	cout << "- Person: Age=" << age << endl;
}

int
AGPerson::getAge()
{
	return age;
}

bool
AGPerson::decision(float proba)
{
	if(rand() % 1000 < (int)(1000 * proba))
		return true;
	else
		return false;
}

bool
AGPerson::isAdult()
{
	return adult;
}
