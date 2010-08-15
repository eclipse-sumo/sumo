/**********************************************************************************************
 * File adapted by Piotr Woznica from the code of RONetHandler.cpp written by Daniel Krajzewicz
 * see RONetHandler.h/cpp - GenericSAXHandler.h/cpp
 **********************************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "AGActivityGenHandler.h"
#include <iostream>
#include <utility>
#include <map>
#include <string>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <router/RONet.h>
#include "city/AGCity.h"
#include "city/AGSchool.h"
#include "city/AGPosition.h"
#include "city/AGBusLine.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
AGActivityGenHandler::AGActivityGenHandler(AGCity &city, RONet *net)
        : SUMOSAXHandler("sumo-stat"), net(net),
        myCity(city), myHaveWarnedAboutDeprecatedVClass(false) {}


AGActivityGenHandler::~AGActivityGenHandler() throw() {}


void
AGActivityGenHandler::myStartElement(SumoXMLTag element, const SUMOSAXAttributes &attrs)
throw(ProcessError) {
    switch (element) {
    case AGEN_TAG_GENERAL:
    	parseGeneralCityInfo(attrs);
        break;
    case AGEN_TAG_STREET:
    	parseStreets(attrs);
    	break;
    case AGEN_TAG_WORKHOURS:
    	parseWorkHours();
    	break;
    case AGEN_TAG_OPENING:
    	parseOpeningHour(attrs);
    	break;
    case AGEN_TAG_CLOSING:
    	parseClosingHour(attrs);
    	break;
    case AGEN_TAG_SCHOOLS:
    	parseSchools();
    	break;
    case AGEN_TAG_SCHOOL:
    	parseSchool(attrs);
    	break;
    case AGEN_TAG_BUSSTATION:
    	parseBusStation(attrs);
    	break;
    case AGEN_TAG_BUSLINE:
    	parseBusLine(attrs);
    	break;
    case AGEN_TAG_STATION:
    	parseStation(attrs);
    	break;
    case AGEN_TAG_FREQUENCY:
    	parseFrequency(attrs);
    	break;
    case AGEN_TAG_POPULATION:
    	parsePopulation();
    	break;
    /*case AGEN_TAG_CHILD_ACOMP:
    	parseChildrenAccompaniment();
    	break;*/
    case AGEN_TAG_BRACKET:
    	parseBracket(attrs);
    	break;
    case AGEN_TAG_PARAM:
    	parseParameters(attrs);
    	break;
    case AGEN_TAG_ENTRANCE:
    	parseCityGates(attrs);
    	break;
    default:
        break;
    }
}


void
AGActivityGenHandler::parseGeneralCityInfo(const SUMOSAXAttributes &attrs)
{
	myCity.statData.inhabitants = attrs.getInt(AGEN_ATTR_INHABITANTS);
	myCity.statData.households = attrs.getInt(AGEN_ATTR_HOUSEHOLDS);
	myCity.statData.limitAgeChildren = attrs.getInt(AGEN_ATTR_CHILDREN);
	myCity.statData.limitAgeRetirement = attrs.getInt(AGEN_ATTR_RETIREMENT);
	myCity.statData.carRate = attrs.getFloat(AGEN_ATTR_CARS);
	myCity.statData.unemployement = attrs.getFloat(AGEN_ATTR_UNEMPLOYEMENT);
	myCity.statData.maxFootDistance = attrs.getInt(AGEN_ATTR_MAX_FOOT_DIST);
	myCity.statData.incomingTraffic = attrs.getInt(AGEN_ATTR_IN_TRAFFIC);
	myCity.statData.outgoingTraffic = attrs.getInt(AGEN_ATTR_OUT_TRAFFIC);
}

void
AGActivityGenHandler::parseParameters(const SUMOSAXAttributes &attrs)
{
	if(attrs.hasAttribute(AGEN_ATTR_CARPREF))
		myCity.statData.carPreference = attrs.getFloat(AGEN_ATTR_CARPREF);
	else
		myCity.statData.carPreference = 0;
	std::cout << "carPreference: " << myCity.statData.carPreference << std::endl;

	if(attrs.hasAttribute(AGEN_ATTR_CITYSPEED))
		myCity.statData.speedTimePerKm = attrs.getFloat(AGEN_ATTR_CITYSPEED);
	else
		myCity.statData.speedTimePerKm = 6;
	std::cout << "time per kilometer in city: " << myCity.statData.speedTimePerKm << std::endl;

	if(attrs.hasAttribute(AGEN_ATTR_FREETIMERATE))
		myCity.statData.freeTimeActivityRate = attrs.getFloat(AGEN_ATTR_FREETIMERATE);
	else
		myCity.statData.freeTimeActivityRate = 0.15;
	std::cout << "free time activity rate: " << myCity.statData.freeTimeActivityRate << std::endl;

	if(attrs.hasAttribute(AGEN_ATTR_UNI_RAND_TRAFFIC))
		myCity.statData.uniformRandomTrafficRate = attrs.getFloat(AGEN_ATTR_UNI_RAND_TRAFFIC);
	else
		myCity.statData.uniformRandomTrafficRate = 0.0;
	std::cout << "uniform random traffic proportion: " << myCity.statData.uniformRandomTrafficRate << std::endl;

	if(attrs.hasAttribute(AGEN_ATTR_DEP_VARIATION))
		myCity.statData.departureVariation = attrs.getFloat(AGEN_ATTR_DEP_VARIATION);
	else
		myCity.statData.departureVariation = 0.0;
	std::cout << "variance in trip departure time variation: " << myCity.statData.departureVariation << std::endl;
}

void
AGActivityGenHandler::parseStreets(const SUMOSAXAttributes &attrs)
{
	int pop = 0;
	int work = 0;

	if(attrs.hasAttribute(AGEN_ATTR_POPULATION))
		pop = attrs.getInt(AGEN_ATTR_POPULATION);
	if(attrs.hasAttribute(AGEN_ATTR_OUT_WORKPOSITION))
		work = attrs.getInt(AGEN_ATTR_OUT_WORKPOSITION);

	AGStreet str(attrs.getString(SUMO_ATTR_EDGE), net, pop, work);
	myCity.streets.push_back(str);
	str.print();
}

void
AGActivityGenHandler::parseCityGates(const SUMOSAXAttributes &attrs)
{
	//streets have to exist
	AGPosition posi(myCity.getStreet(attrs.getString(SUMO_ATTR_EDGE)), attrs.getFloat(SUMO_ATTR_POSITION));
	myCity.cityGates.push_back(posi);
}

void
AGActivityGenHandler::parseWorkHours()
{
	myCurrentObject = "workHours";
	std::cout << myCurrentObject << std::endl;
}

void
AGActivityGenHandler::parseOpeningHour(const SUMOSAXAttributes &attrs)
{
	if(myCurrentObject == "workHours")
		myCity.statData.beginWorkHours[attrs.getInt(AGEN_ATTR_HOUR)] = attrs.getFloat(AGEN_ATTR_PROP);
	//std::cout << " - opening: " << myCity.statData.beginWorkHours.find(attrs.getInt(AGEN_ATTR_HOUR)) << std::endl;
}

void
AGActivityGenHandler::parseClosingHour(const SUMOSAXAttributes &attrs)
{
	if(myCurrentObject == "workHours")
		myCity.statData.endWorkHours[attrs.getInt(AGEN_ATTR_HOUR)] = attrs.getFloat(AGEN_ATTR_PROP);
	//std::cout << " - opening: " << myCity.statData.endWorkHours.find(attrs.getInt(AGEN_ATTR_HOUR)) << std::endl;
}

void
AGActivityGenHandler::parseSchools()
{
	myCurrentObject = "schools";
	std::cout << myCurrentObject << std::endl;
}

void
AGActivityGenHandler::parseSchool(const SUMOSAXAttributes &attrs)
{
	AGPosition posi(myCity.getStreet(attrs.getString(SUMO_ATTR_EDGE)), attrs.getFloat(SUMO_ATTR_POSITION));
	int beginAge = attrs.getInt(AGEN_ATTR_BEGINAGE);
	int endAge = attrs.getInt(AGEN_ATTR_ENDAGE);
	AGSchool sch(attrs.getInt(AGEN_ATTR_CAPACITY), posi, beginAge, endAge, attrs.getInt(AGEN_ATTR_OPENING), attrs.getInt(AGEN_ATTR_CLOSING));
	myCity.schools.push_back(sch);
	//myCity.statData.schoolCapacity[type] += attrs.getInt(AGEN_ATTR_CAPACITY);
	//TODO rajouter le comptage des ecoles en fonction l'age
	//std::cout << "capacity: " << myCity.statData.schoolCapacity[type] << std::endl;
}

void
AGActivityGenHandler::parseBusStation(const SUMOSAXAttributes &attrs)
{
	AGPosition posi(myCity.getStreet(attrs.getString(SUMO_ATTR_EDGE)), attrs.getFloat(SUMO_ATTR_POSITION));
	myCity.statData.busStations.insert(std::pair<int, AGPosition>(attrs.getInt(SUMO_ATTR_ID), posi));
	/*std::cout << "bus station: ";
	posi.print();
	std::cout << std::endl;*/
}

void
AGActivityGenHandler::parseBusLine(const SUMOSAXAttributes &attrs)
{
	myCurrentObject = "busLine";
	std::cout << myCurrentObject << std::endl;
	AGBusLine busL(attrs.getString(SUMO_ATTR_ID));
	busL.setMaxTripTime(attrs.getInt(AGEN_ATTR_MAX_TRIP_DURATION));
	myCity.busLines.push_front(busL);
	currentBusLine = &*myCity.busLines.begin();
}

void
AGActivityGenHandler::parseStation(const SUMOSAXAttributes &attrs)
{
	if(myCurrentObject != "busLine")
		return;
	currentBusLine->locateStation(myCity.statData.busStations.find(attrs.getInt(SUMO_ATTR_REFID))->second);
	//std::cout << "_";
}

void
AGActivityGenHandler::parseFrequency(const SUMOSAXAttributes &attrs)
{
	if(myCurrentObject != "busLine")
		return;
	int beginB = attrs.getInt(SUMO_ATTR_BEGIN);
	int endB = attrs.getInt(SUMO_ATTR_END);
	int rateB = attrs.getInt(AGEN_ATTR_RATE);
	currentBusLine->generateBuses(beginB, endB, rateB);
	//std::cout << "|";
}

void
AGActivityGenHandler::parsePopulation()
{
	myCurrentObject = "population";
	std::cout << myCurrentObject << std::endl;
}

/*void
AGActivityGenHandler::parseChildrenAccompaniment()
{
	myCurrentObject = "childrenAccompaniment";
	std::cout << myCurrentObject << std::endl;
}*/

void
AGActivityGenHandler::parseBracket(const SUMOSAXAttributes &attrs)
{
	int beginAge = attrs.getInt(AGEN_ATTR_BEGINAGE); //included in the bracket
	int endAge = attrs.getInt(AGEN_ATTR_ENDAGE); //NOT included in the bracket
	if(myCurrentObject == "population")
	{
		myCity.statData.population[endAge] = attrs.getInt(AGEN_ATTR_PEOPLENBR);
		std::cout << "- people: [" << beginAge << "-" << endAge << "] = " << attrs.getInt(AGEN_ATTR_PEOPLENBR) << std::endl;
	}
	/*else if(myCurrentObject == "childrenAccompaniment")
	{
		myCity.statData.childrenAccompagniment[endAge] = attrs.getFloat(AGEN_ATTR_RATE);
		std::cout << "- childrenAcomp.Rate: [" << beginAge << "-" << endAge << "] = " << attrs.getFloat(AGEN_ATTR_RATE) << std::endl;
	}*/
}

/****************************************************************************/

