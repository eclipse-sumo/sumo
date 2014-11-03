/****************************************************************************/
/// @file    AGDataAndStatistics.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Contains various data, statistical values and functions from input used
// by various objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef AGDATAANDSTATISTICS_H
#define AGDATAANDSTATISTICS_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <map>
#include "AGSchool.h"
#include "AGPosition.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGDataAndStatistics {
public:
    /**
     * parameters
     */
    SUMOReal speedTimePerKm;							//number of seconds for one km
    SUMOReal carPreference;							//prob. to choose the car (compared to the bus)

    /**
     * from General element from XML file.
     */
    int inhabitants;
    int households;
    int limitAgeChildren;
    int limitAgeRetirement;
    int limitEndAge;
    SUMOReal carRate;
    SUMOReal unemployement;
    SUMOReal maxFootDistance;
    int incomingTraffic;
    int outgoingTraffic;
    SUMOReal freeTimeActivityRate;
    SUMOReal uniformRandomTrafficRate;
    SUMOReal departureVariation;

    std::map<int, SUMOReal> beginWorkHours;				//<hour, probability> (number only used in term of PROPORTION: it should be normalized)
    std::map<int, SUMOReal> endWorkHours;				//<hour, probability> (number only used in term of PROPORTION: it should be normalized)
    std::map<int, AGPosition> busStations;				//<id, position>
    //std::map<SchoolType, int> schoolCapacity;
    std::map<int, SUMOReal> population;						//<bracket's end age, number> (number only used in term of PROPORTION: it should be normalized)
    //std::map<int, SUMOReal> childrenAccompagniment;		//<bracket's end age, probability>
    /**
     * number of incoming or outgoing people through the given city gates
     * PROPORTION: it should be normalized
     */
    std::map<int, SUMOReal> incoming;
    std::map<int, SUMOReal> outgoing;

    /**
     * computed values used during processing
     */
    int workPositions;
    SUMOReal factorInhabitants;
    SUMOReal factorWorkPositions;

    /**
     * data used for household generation
     * has to be computed before.
     */
    //probability of picking an old-people household (compared to working and have children adults)
    SUMOReal oldAgeHhProb;
    //probability of having a second adult (old in case of old householders) in the household.
    SUMOReal secondPersProb;
    //this s the mean number of children pro household (not old). This value is used in the Poisson-lay for determining the number of children in each family
    SUMOReal meanNbrChildren;

    /**
     * numbers not needed but interesting for statistics and TESTING
     */
    //int childrenNbr;
    //int oldPeopleNbr;
    int AdultNbr;
    int householdsNbr;
    //households far from public transports
    int hhFarFromPT;

    static AGDataAndStatistics& getDataAndStatistics();

    /**
     * function returning a random number between the two given numbers: [n;m[ (m cannot occur)
     * it returns 0 if m < n
     */
    int getRandom(int n, int m);
    /**
     * function returning a random age between the two numbers satisfying the previous constrains
     * this number is in relation to the distribution of the population through the brackets (population's list)
     * if the given numbers are both greater than limitEndAge, it returns 0
     * if m is greater than limitEndAge, m=limitEndAge
     * returns -1 if conditions are not satisfied
     */
    int getRandomPopDistributed(int n, int m);
    /**
     * function evaluating the POISSON's lay (probability lay)
     * it returns the number of children with a probability of POISSON in a household
     * is given the mean of the distribution
     *
     * TODO Consider reimplementation
     */
    int getPoissonsNumberOfChildren(SUMOReal mean);
    /**
     * these functions return the number of people having more (or less) than the given age
     * (inclusive for getPeopleOlderThan; exclusive for getPeopleYoungerThan)
     * getPeopleOlderThan(n) + getPeopleYoungerThan(n) = inhabitants
     * these first two function are based on the third one.
     */
    int getPeopleOlderThan(int age);
    int getPeopleYoungerThan(int age);
    SUMOReal getPropYoungerThan(int age);
    /**
     * function consolidating statistics:
     * normalizes the maps with probabilities
     * completes data which have to be computed before use
     */
    void consolidateStat();
    /**
     * function returning a random value corresponding to this distribution:
     * -mean is given
     * -max variation of the mean (|possible value - mean| <= maxVar
     * -the mean is the most probable
     * -the probability distribution function is a "scaled exponential" distribution
     * from mean-maxVar to mean and from mean to mean+maxVar
     */
    SUMOReal getInverseExpRandomValue(SUMOReal mean, SUMOReal maxVar);
    /**
     * function returning a random city gate corresponding to
     * the distribution of the incoming/outgoing traffic
     * In fact, it returns the position of a city gate in the citygate vector.
     */
    int getRandomCityGateByIncoming();
    int getRandomCityGateByOutgoing();

private:
    AGDataAndStatistics() {}

    /**
     * returns the POISSON's probability (exp(-m)*m^k/k!)
     * @arg: mean of the distribution
     * @arg: number of occurrences
     *
     * TODO Create a Poisson distribution class for this. Or is it in Boost?
     */
    SUMOReal poisson(SUMOReal mean, int occ);
    /**
     * recursive mathematical function returning the factorial of n: n!
     */
    int factorial(int n);

    //function normalizing the map's probabilities: Sum(floats) = 1
    void normalizeMapProb(std::map<int, SUMOReal>* myMap);
};

#endif

/****************************************************************************/
