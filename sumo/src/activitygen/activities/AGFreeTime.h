#ifndef AGFREETIME_H
#define AGFREETIME_H

#include "../city/AGHousehold.h"
#include "AGActivity.h"

class AGFreeTime : public AGActivity
{
public:
	AGFreeTime(AGHousehold *hh, AGDataAndStatistics *das, std::list<AGTrip>* prevTrips) :
		AGActivity(hh, das, prevTrips, 2),
		freqOut(das->freeTimeActivityRate),
		nbrDays(1)
	{};
	AGFreeTime(AGHousehold *hh, AGDataAndStatistics *das, std::list<AGTrip>* prevTrips, int days) :
		AGActivity(hh, das, prevTrips, 2),
		freqOut(das->freeTimeActivityRate),
		nbrDays(days)
	{};

	/**
	 * @Overwrite
	 */
	bool generateTrips();

	/**
	 * returns the type of trip that will be done on this household
	 * there is also the case: no free time trip
	 * 0 = non
	 * 1 = during the day
	 * 2 = in the evening
	 * 4 = during the night
	 * (combinations using '+' are available for possibleTypeOfTrip())
	 */
	int possibleTypeOfTrip();
	int decideTypeOfTrip();
	/**
	 * The different type of trips that are available:
	 * one function is called among all the following
	 */
	bool typeFromHomeDay(int day);
	bool typeFromHomeEvening(int day);
	bool typeFromHomeNight(int day);

	/**
	 * returns the moment when everybody is back home and ready
	 * to do some thing else using everyday trips
	 */
	int whenBackHome();
	/**
	 * moment when everybody is back home using all trips applicable for the given day
	 * if no car is used: return 0
	 */
	int whenBackHomeThisDay(int day);
	/**
	 * moment when the first person takes the car to go somewhere
	 * id no car is used: return 2400 (midnight at the end of the given day)
	 */
	int whenBeginActivityNextDay(int day);

private:
	/**
	 * number of days for the simulation
	 * households are likely to go out some days but not others
	 */
	int nbrDays;
	/**
	 * time ready to do something else
	 * everybody is back home
	 */
	int tReady;
	/**
	 * possible type of trips for this household
	 */
	int possibleType;
	/**
	 * frequency of going out or see family is assumed to be once a week (in mean)
	 */
	float freqOut;

	static const int DAY;// = 1;
	static const int EVENING;// = 2;
	static const int NIGHT;// = 4;

	static const int TB_DAY;// = 800;
	static const int TE_DAY;// = 1800;
	static const int TB_EVENING;// = 1900;
	static const int TE_EVENING;// = 2400;
	static const int TB_NIGHT;// = 2300;
	static const int TE_NIGHT;// = 500;

};

#endif
