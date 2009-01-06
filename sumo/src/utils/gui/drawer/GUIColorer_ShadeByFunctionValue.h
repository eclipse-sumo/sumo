/****************************************************************************/
/// @file    GUIColorer_ShadeByFunctionValue.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef GUIColorer_ShadeByFunctionValue_h
#define GUIColorer_ShadeByFunctionValue_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUIBaseColorer.h"
#include <utils/common/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include <utils/gui/div/GUIGlobalSelection.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIColorer_ShadeByFunctionValue
 * @brief Colors by using a retrieved value which is casted to SUMOReal
 */
template<class T, class D>
class GUIColorer_ShadeByFunctionValue : public GUIBaseColorer<T>
{
public:
    /// Type of the function to execute.
    typedef D(T::* Operation)() const;

    /// Constructor
    GUIColorer_ShadeByFunctionValue(SUMOReal min, SUMOReal max,
                                    const RGBColor &minC, const RGBColor &maxC,
                                    Operation operation)
            : myMin(min), myMax(max), myMinColor(minC), myMaxColor(maxC),
            myOperation(operation) {
    }

    /// Destructor
    virtual ~GUIColorer_ShadeByFunctionValue() { }


    /// @name inherited from GUIBaseColorer
    //@{

    /// Sets the color using a value from the given instance of T
    void setGlColor(const T& i) const {
        SUMOReal val = (SUMOReal)(i.*myOperation)();
        if (val==-1) {
            glColor3f(0.8f, 0.8f, 0.8f);
        } else {
            RGBColor c = RGBColor::interpolate(myMinColor, myMaxColor, (val-myMin)/(myMax-myMin));
            glColor3d(c.red(), c.green(), c.blue());
        }
    }

    /// Returns the type of this class (CST_MINMAX)
    virtual ColorSetType getSetType() const {
        return CST_MINMAX;
    }
    //@}


    /// @name inherited from from GUIBaseColorerInterface
    //@{

    /// Sets the given color as the colors to use
    virtual void resetColor(const RGBColor &min, const RGBColor &max) {
        myMinColor = min;
        myMaxColor = max;
    }

    /// Returns the color used for minimum values
    virtual const RGBColor &getMinColor() const {
        return myMinColor;
    }

    /// Returns the color used for maximum values
    virtual const RGBColor &getMaxColor() const {
        return myMaxColor;
    }
    //@}

protected:
    /// The ranges and the resulting scale
    SUMOReal myMin, myMax;

    /// The colors to use
    RGBColor myMinColor, myMaxColor;

    /// The object's operation to perform.
    Operation myOperation;

};


#endif

/****************************************************************************/

