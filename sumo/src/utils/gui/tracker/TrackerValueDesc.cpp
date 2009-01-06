/****************************************************************************/
/// @file    TrackerValueDesc.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Storage for a tracked value
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/common/RGBColor.h>
#include <utils/gui/globjects/GUIGlObject.h>
//#include <guisim/GUINet.h>
#include "TrackerValueDesc.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
TrackerValueDesc::TrackerValueDesc(const std::string &name,
                                   const RGBColor &col,
                                   GUIGlObject *o,
                                   size_t recordBegin)
        : myName(name), myObject(o),
        myActiveCol(col), myInactiveCol(col),
        myMin(0), myMax(0),
        myAggregationInterval(1), myInvalidValue(-1), myValidNo(0),
        myRecordingBegin(recordBegin), myTmpLastAggValue(0)
{}


TrackerValueDesc::~TrackerValueDesc()
{
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


void
TrackerValueDesc::addValue(SUMOReal value)
{
    if (myValues.size()==0) {
        myMin = (SUMOReal) value;
        myMax = (SUMOReal) value;
    } else {
        myMin = (SUMOReal) value < myMin ? (SUMOReal) value : myMin;
        myMax = (SUMOReal) value > myMax ? (SUMOReal) value : myMax;
    }
    myLock.lock();
    myValues.push_back((SUMOReal) value);
    if (value!=myInvalidValue) {
        myTmpLastAggValue += (SUMOReal) value;
        myValidNo++;
    }
    // check what to do with aggregated values
    if (myValues.size()!=0&&myValues.size()%myAggregationInterval==0) {
        // ok, a new aggregation is filled completely. Set.
        if (myValidNo!=0) {
            myAggregatedValues.push_back(
                myTmpLastAggValue / (SUMOReal) myValidNo);
        } else {
            myAggregatedValues.push_back(0);
        }
        myTmpLastAggValue = 0;
        myValidNo = 0;
    } else {
        // remove the one previously set
        if (myAggregatedValues.size()!=0) {
            myAggregatedValues.erase(myAggregatedValues.end()-1);
        }
        // append newly computed
        if (myValidNo!=0) {
            myAggregatedValues.push_back(
                myTmpLastAggValue / (SUMOReal) myValidNo);
        } else {
            myAggregatedValues.push_back(0);
        }
    }
    myLock.unlock();
}


SUMOReal
TrackerValueDesc::getRange() const
{
    getMin();
    getMax();
    return myMax - myMin;
}


SUMOReal
TrackerValueDesc::getMin() const
{
    return myMin;
}


SUMOReal
TrackerValueDesc::getMax() const
{
    return myMax;
}


SUMOReal
TrackerValueDesc::getYCenter() const
{
    getMin();
    getMax();
    return (myMin + myMax) / 2.0f;
}


const RGBColor &
TrackerValueDesc::getColor() const
{
    return myActiveCol;
}


const std::vector<SUMOReal> &
TrackerValueDesc::getValues()
{
    myLock.lock();
    return myValues;
}


const std::vector<SUMOReal> &
TrackerValueDesc::getAggregatedValues()
{
    myLock.lock();
    return myAggregatedValues;
}


const std::string &
TrackerValueDesc::getName() const
{
    return myName;
}

void
TrackerValueDesc::unlockValues()
{
    myLock.unlock();
}


void
TrackerValueDesc::setAggregationSpan(size_t as)
{
    myLock.lock();
    if (myAggregationInterval!=as) {
        // ok, the aggregation has changed,
        //  let's recompute the list of aggregated values
        myAggregatedValues.clear();
        std::vector<SUMOReal>::iterator i;
        for (i=myValues.begin(); i!=myValues.end();) {
            SUMOReal value = 0;
            myValidNo = 0;
            for (size_t j=0; j<as&&i!=myValues.end(); j++, ++i) {
                if ((*i)!=myInvalidValue) {
                    value += (*i);
                    myValidNo++;
                }
            }
            if (myValidNo==0) {
                myAggregatedValues.push_back(0);
                myTmpLastAggValue = 0;
            } else {
                myAggregatedValues.push_back(value / (SUMOReal) myValidNo);
                myTmpLastAggValue = value / (SUMOReal) myValidNo;
            }
        }
    }
    myAggregationInterval = as;
    myLock.unlock();
}


size_t
TrackerValueDesc::getAggregationSpan() const
{
    return myAggregationInterval;
}


size_t
TrackerValueDesc::getRecordingBegin() const
{
    return myRecordingBegin;
}



/****************************************************************************/

