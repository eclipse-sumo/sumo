/****************************************************************************/
/// @file    GUIVehicleType.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17. Jun 2004
/// @version $Id$
///
// GUI-version of a MSVehicleType (extended by a color)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIVehicleType_h
#define GUIVehicleType_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <microsim/MSVehicleType.h>
#include <utils/common/RGBColor.h>
#include <utils/gui/div/GUIColorSetter.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIVehicleType
 * A plain MSVehicleType, only extended by a color.
 */
class GUIVehicleType : public MSVehicleType
{
public:
    /// Constructor
    GUIVehicleType(const RGBColor &c, const std::string &id,
                   SUMOReal length, SUMOReal maxSpeed,
                   SUMOReal accel, SUMOReal decel, SUMOReal dawdle,
                   SUMOReal tau, SUMOVehicleClass vclass);

    /// Destructor.
    ~GUIVehicleType();

    /// Returns the color
    const RGBColor &getColor() const;

    /// Sets the color in openGL
    inline void setColor() const {
        mglColor(myColor);
    }

private:
    /// The color
    RGBColor myColor;

private:
    /// Invalidated copy constructor.
    GUIVehicleType(const GUIVehicleType&);

    /// Invalidated assignment operator.
    GUIVehicleType& operator=(const GUIVehicleType&);

};


#endif

/****************************************************************************/

