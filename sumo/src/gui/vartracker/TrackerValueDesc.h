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
#include <microsim/logging/DoubleValueRetriever.h>
#include <microsim/logging/DoubleFunctionBinding.h>


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
class TrackerValueDesc : public DoubleValueRetriever {
public:
    /// Constructor
    TrackerValueDesc(const std::string &name, const RGBColor &col,
        GUIGlObject *o);

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

    /// Retunrs the name of the value
    const std::string &getName() const;

    /// Adds a new value to the list
    void addValue(double value);

    /// Releases the locking after the values have been drawn
    void unlockValues();


private:
    /// The name of the value
    std::string myName;

    /// The object to retrieve the information from
    GUIGlObject *myObject;

    /// Values collected
    std::vector<float> myValues;

    /// The color to use when the value is set as "active"
	RGBColor myActiveCol;

    /// The color to use when the value is set as "inactive"
	RGBColor myInactiveCol;

    /// Information whether the value is active (!!! not used by now)
	bool myAmActive;

    /// The minimum and the maximum of the value
	double myMin, myMax;

    // Mutex to avoid parallel drawing and insertion of new items
    NewQMutex myLock;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "TrackerValueDesc.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

