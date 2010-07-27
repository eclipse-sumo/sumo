#ifndef AGCHILD_H
#define AGCHILD_H

#include <iostream>
#include <vector>
#include "AGPerson.h"
#include "AGPosition.h"
#include "AGSchool.h"

class AGChild : public AGPerson
{
public:
	AGChild(int age) :
		AGPerson(age, false),
		school(NULL)
	{};
	void print();
	bool setSchool(AGSchool *school);
	/**
	 * @param schools: school vector from City object
	 * @param housepos: Position of the households habitation
	 * @return if a school was found corresponding to the child's age.
	 */
	bool alocateASchool(std::list<AGSchool> *schools, AGPosition housePos);
	/**
	 * @return if the child is now without any school
	 */
	bool leaveSchool();
	bool haveASchool();
	AGPosition getSchoolLocation();
	int getSchoolOpeining();
	int getSchoolClosing();

private:
	AGSchool *school;
};

#endif

