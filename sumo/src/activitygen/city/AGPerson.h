#ifndef AGPERSON_H
#define AGPERSON_H

#include <iostream>

class AGPerson
{
public:
	AGPerson(int age, bool adult) : age(age), adult(adult) {};
	void print();
	int getAge();

	//decision feature for people in relation to the realisation probability
	bool decision(float probability);
	//returns if an adult
	bool isAdult();

protected:
	int age;
	bool adult;
	//bool driverLicence; //not used
	//char sex; //not used
};

#endif

