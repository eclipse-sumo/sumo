#include <iostream>
#include <vector>
#include "AGChild.h"
#include "AGSchool.h"
#include <cfloat>

using namespace std;

void
AGChild::print()
{
	cout << "- Child: Age=" << age << " School=" << school << endl;
}

bool
AGChild::setSchool(AGSchool *school)
{
	if(school == NULL)
		return false;
	bool enoughPlace = school->addNewChild();
	if(enoughPlace)
		this->school = school;
	return enoughPlace;
}

bool
AGChild::alocateASchool(list<AGSchool> *schools, AGPosition housePos)
{
	float minDist = FLT_MAX;
	AGSchool *sch = NULL;
	if(schools->size() == 0)
		return false;
	list<AGSchool>::iterator it = schools->begin();

	for(it = schools->begin() ; it!=schools->end() ; ++it)
	{
		if(it->acceptThisAge(age) && it->getPlaces()>0 && housePos.getDistanceTo(it->getPosition()) < minDist)
		{
			minDist = housePos.getDistanceTo(it->getPosition());
			sch = &(*it);
		}
	}
	return setSchool(sch);
}

bool
AGChild::leaveSchool()
{
	if(school != NULL)
		if(!school->removeChild())
			return false;
	school = NULL;
	return true;
}

bool
AGChild::haveASchool(){
	if(school == NULL)
		return false;
	return true;
}

AGPosition
AGChild::getSchoolLocation()
{
	return school->getPosition();
}

int
AGChild::getSchoolClosing()
{
	return school->getClosingHour();
}

int
AGChild::getSchoolOpeining()
{
	return school->getOpeningHour();
}
