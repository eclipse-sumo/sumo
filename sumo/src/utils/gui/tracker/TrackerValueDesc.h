#ifndef TrackerValueDesc_h
#define TrackerValueDesc_h
//---------------------------------------------------------------------------//
//                        TrackerValueDesc.h -
//  Storage for a tracked value
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:56  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.7  2004/07/02 08:26:11  dkrajzew
// aggregation debugged and saving option added
//
// Revision 1.6  2004/03/19 12:42:59  dkrajzew
// porting to FOX
//
// Revision 1.5  2003/11/12 14:08:24  dkrajzew
// clean up after recent changes
//
// Revision 1.4  2003/11/11 08:43:04  dkrajzew
// synchronisation problems of parameter tracker updates patched
//
// Revision 1.3  2003/07/30 08:50:42  dkrajzew
// tracker debugging (not yet completed)
//
// Revision 1.2  2003/06/05 11:38:47  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <utils/gfx/RGBColor.h>
#include <helpers/ValueRetriever.h>
#include <utils/foxtools/FXMutex.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObject;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class TrackerValueDesc : public ValueRetriever<double> {
public:
    /// Constructor
    TrackerValueDesc(const std::string &name, const RGBColor &col,
        GUIGlObject *o, size_t recordBegin);

    /// Destructor
    ~TrackerValueDesc();

    /// returns the maximum value range
    float getRange() const;

    /// Returns the values minimum
    float getMin() const;

    /// Returns the values maximum
    float getMax() const;

    /// Returns the center of the value
    float getYCenter() const;

    /// Returns the color to use to display the value
    const RGBColor &getColor() const;

    /** @brief returns the vector of collected values
        The values will be locked - no further addition will be perfomed until
        the method "unlockValues" will be called */
    const std::vector<float> &getValues();

    /** @brief returns the vector of aggregated values
        The values will be locked - no further addition will be perfomed until
        the method "unlockValues" will be called */
    const std::vector<float> &getAggregatedValues();

    /// Retunrs the name of the value
    const std::string &getName() const;

    /// Adds a new value to the list
    void addValue(double value);

    /// Releases the locking after the values have been drawn
    void unlockValues();

    /// set the aggregation amount
    void setAggregationSpan(size_t as);

    /// get the aggregation amount
    size_t getAggregationSpan() const;

    /// Returns the timestep the recording started
    size_t getRecordingBegin() const;


private:
    /// The name of the value
    std::string myName;

    /// The object to retrieve the information from
    GUIGlObject *myObject;

    /// Values collected
    std::vector<float> myValues;

    /// Collected values in their aggregated form
    std::vector<float> myAggregatedValues;

    /// The color to use when the value is set as "active"
    RGBColor myActiveCol;

    /// The color to use when the value is set as "inactive"
    RGBColor myInactiveCol;

    /// Information whether the value is active (!!! not used by now)
    bool myAmActive;

    /// The minimum and the maximum of the value
    float myMin, myMax;

    // Mutex to avoid parallel drawing and insertion of new items
    FXEX::FXMutex myLock;

    /// The aggregation interval
    size_t myAggregationInterval;

    /// Values like this shall not be counted on aggregation
    float myInvalidValue;

    /// Temporary storage for the last aggregation interval
    float myTmpLastAggValue;

    /// Counter for valid numbers within the current aggregation interval
    size_t myValidNo;

    /// The time step the values are added from
    size_t myRecordingBegin;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

