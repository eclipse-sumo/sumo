/****************************************************************************/
/// @file    TrackerValueDesc.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Representation of a timeline of floats with their names and moments
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TrackerValueDesc_h
#define TrackerValueDesc_h
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <string>
#include <vector>
#include <utils/foxtools/MFXMutex.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ValueRetriever.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TrackerValueDesc
 * @brief Representation of a timeline of floats with their names and moments
 *
 * This class contains the information needed to display a time line of
 *  float values.
 */
class TrackerValueDesc : public ValueRetriever<SUMOReal> {
public:
    /// Constructor
    TrackerValueDesc(const std::string& name, const RGBColor& col,
                     SUMOTime recordBegin);

    /// Destructor
    ~TrackerValueDesc();

    /// returns the maximum value range
    SUMOReal getRange() const;

    /// Returns the values minimum
    SUMOReal getMin() const;

    /// Returns the values maximum
    SUMOReal getMax() const;

    /// Returns the center of the value
    SUMOReal getYCenter() const;

    /// Returns the color to use to display the value
    const RGBColor& getColor() const;

    /** @brief returns the vector of collected values
        The values will be locked - no further addition will be perfomed until
        the method "unlockValues" will be called */
    const std::vector<SUMOReal>& getValues();

    /** @brief returns the vector of aggregated values
        The values will be locked - no further addition will be perfomed until
        the method "unlockValues" will be called */
    const std::vector<SUMOReal>& getAggregatedValues();

    /// Returns the name of the value
    const std::string& getName() const;

    /// Adds a new value to the list
    void addValue(SUMOReal value);

    /// Releases the locking after the values have been drawn
    void unlockValues();

    /// set the aggregation amount
    void setAggregationSpan(SUMOTime as);

    /// get the aggregation amount
    SUMOTime getAggregationSpan() const;

    /// Returns the timestep the recording started
    SUMOTime getRecordingBegin() const;


private:
    /// The name of the value
    std::string myName;

    /// The color to use when the value is set as "active"
    RGBColor myActiveCol;

    /// The color to use when the value is set as "inactive"
    RGBColor myInactiveCol;

    /// Values collected
    std::vector<SUMOReal> myValues;

    /// Collected values in their aggregated form
    std::vector<SUMOReal> myAggregatedValues;

    /// The minimum and the maximum of the value
    SUMOReal myMin, myMax;

    // Mutex to avoid parallel drawing and insertion of new items
    MFXMutex myLock;

    /// The aggregation interval in simulation steps
    int myAggregationInterval;

    /// Values like this shall not be counted on aggregation
    SUMOReal myInvalidValue;

    /// Counter for valid numbers within the current aggregation interval
    int myValidNo;

    /// The time step the values are added from
    SUMOTime myRecordingBegin;

    /// Temporary storage for the last aggregation interval
    SUMOReal myTmpLastAggValue;

};


#endif

/****************************************************************************/

