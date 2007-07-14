/****************************************************************************/
/// @file    GUIColorer_ShadeByCastedFunctionValue.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 12.12.2006
/// @version $Id$
///
//
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
#ifndef GUIColorer_ShadeByCastedFunctionValue_h
#define GUIColorer_ShadeByCastedFunctionValue_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUIBaseColorer.h"
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include <utils/gui/div/GUIGlobalSelection.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIColorer_ShadeByCastedFunctionValue
 * @brief Colors by using a retrieved value which is cased to SUMOReal
 */
template<class T, class D>
class GUIColorer_ShadeByCastedFunctionValue : public GUIBaseColorer<T>
{
public:
    /// Type of the function to execute.
    typedef D(T::* Operation)() const;

    /// Constructor
    GUIColorer_ShadeByCastedFunctionValue(SUMOReal min, SUMOReal max,
                                          const RGBColor &minC, const RGBColor &maxC,
                                          Operation operation)
            : myMin(min), myMax(max), myMinColor(minC), myMaxColor(maxC),
            myOperation(operation)
    {
        myScale = (SUMOReal) 1.0 / (myMax-myMin);
    }

    /// Destructor
    virtual ~GUIColorer_ShadeByCastedFunctionValue()
    { }


    /// @name inherited from GUIBaseColorer
    //@{
    /// Sets the color using a value from the given instance of T
    void setGlColor(const T& i) const
    {
        SUMOReal val = (SUMOReal)(i.*myOperation)() - myMin;
        if (val==-1) {
            glColor3f(0.8f, 0.8f, 0.8f);
        } else {
            if (val<myMin) {
                val = myMin;
            } else if (val>myMax) {
                val = myMax;
            }
            val = val * myScale;
            RGBColor c =
                (myMinColor * ((SUMOReal) 1.0 - val)) + (myMaxColor * val);
            glColor3d(c.red(), c.green(), c.blue());
        }
    }

    /// Sets the color using the given value
    void setGlColor(SUMOReal val) const
    {
        if (val==-1) {
            glColor3f(0.8f, 0.8f, 0.8f);
        } else {
            if (val<myMin) {
                val = myMin;
            } else if (val>myMax) {
                val = myMax;
            }
            val = val * myScale;
            RGBColor c =
                (myMinColor * ((SUMOReal) 1.0 - val)) + (myMaxColor * val);
            glColor3d(c.red(), c.green(), c.blue());
        }
    }

    /// Returns the type of this class (CST_MINMAX)
    virtual ColorSetType getSetType() const
    {
        return CST_MINMAX;
    }
    //@}


    /// @name inherited from from GUIBaseColorerInterface
    //@{
    /// Sets the given color as the colors to use
    virtual void resetColor(const RGBColor &minC, const RGBColor &maxC)
    {
        myMinColor = minC;
        myMaxColor = maxC;
    }

    /// Returns the color used for minimum values
    virtual const RGBColor &getMinColor() const
    {
        return myMinColor;
    }

    /// Returns the color used for maximum values
    virtual const RGBColor &getMaxColor() const
    {
        return myMaxColor;
    }
    //@}

protected:
    /// The ranges and the resulting scale
    SUMOReal myMin, myMax, myScale;

    /// The colors to use
    RGBColor myMinColor, myMaxColor;

    /// The object's operation to perform.
    Operation myOperation;

};


#endif

/****************************************************************************/

