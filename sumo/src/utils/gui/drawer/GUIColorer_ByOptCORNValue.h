/****************************************************************************/
/// @file    GUIColorer_ByOptCORNValue.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 12.12.2006
/// @version $Id: $
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
#ifndef GUIColorer_ByOptCORNValue_h
#define GUIColorer_ByOptCORNValue_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIColorer_ByOptCORNValue
 * @brief Colors by first asking whether a value can be retrieved, then
 *  retrieving it
 */
template<class _T, class _P>
class GUIColorer_ByOptCORNValue : public GUIBaseColorer<_T>
{
public:
    /// Type of the function to execute to get the information whether a value is existing
    typedef bool(_T::* HasOperation)(_P) const;
    /// Type of the function to execute to get the value
    typedef SUMOReal(_T::* NumberOperation)(_P) const;

    /// Constructor
    GUIColorer_ByOptCORNValue(HasOperation hasOperation,
                              NumberOperation numberOperation, bool catchNo, SUMOReal min, SUMOReal max,
                              const RGBColor &no, const RGBColor &minColor, const RGBColor &maxColor,
                              _P param)
            : myHasOperation(hasOperation), myNumberOperation(numberOperation),
            myNoColor(no), myMinColor(minColor), myMaxColor(maxColor),
            myMin(min), myMax(max), myParameter(param)
    {
        myScale = (SUMOReal) 1.0 / (myMax-myMin);
    }

    /// Destructor
    virtual ~GUIColorer_ByOptCORNValue()
    { }

    ///{ from GUIBaseColorer
    /// Sets the color using a value from the given instance of _T
    void setGlColor(const _T& i) const
    {
        // check whether a value can be retrieved...
        if (!(i.*myHasOperation)((_P) myParameter)) {
            // ... no, check whether the min color shall be used
            if (myCatchNo) {
                // ... no, use the fallback color
                glColor3d(myNoColor.red(), myNoColor.green(), myNoColor.blue());
            } else {
                // ... yes, use the min color
                glColor3d(myMinColor.red(), myMinColor.green(), myMinColor.blue());
            }
        } else {
            // ... yes, then retrieve the value
            SUMOReal val = (i.*myNumberOperation)((_P) myParameter) - myMin;
            if (val<myMin) {
                val = myMin;
            } else if (val>myMax) {
                val = myMax;
            }
            val = val * myScale;
            RGBColor c = (myMinColor * ((SUMOReal) 1.0 - val)) + (myMaxColor * val);
            glColor3d(c.red(), c.green(), c.blue());
        }
    }

    /// Sets the color using the given value
    void setGlColor(SUMOReal val) const
    {
        if (val<myMin) {
            val = myMin;
        } else if (val>myMax) {
            val = myMax;
        }
        val = val * myScale;
        RGBColor c = (myMinColor * ((SUMOReal) 1.0 - val)) + (myMaxColor * val);
        glColor3d(c.red(), c.green(), c.blue());
    }

    /// Returns the type of this class (CST_MINMAX_OPT)
    virtual ColorSetType getSetType() const
    {
        return CST_MINMAX_OPT;
    }
    ///}


    ///{ from GUIBaseColorerInterface
    /// Sets the given color as the colors to use
    virtual void resetColor(const RGBColor &minC,
                            const RGBColor &maxC, const RGBColor &fallBackC)
    {
        myMinColor = minC;
        myMaxColor = maxC;
        myNoColor = fallBackC;
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

    /// Returns the color used when a value can not be retrieved
    virtual const RGBColor &getFallbackColor() const
    {
        return myNoColor;
    }
    ///}

protected:
    /// The function that returns the information whether a value can be determined
    HasOperation myHasOperation;

    /// The function that returns the value to use as
    NumberOperation myNumberOperation;

    /// The information whether not existing values shall be treated as min-values
    bool myCatchNo;

    /// The colors to use
    RGBColor myNoColor, myMinColor, myMaxColor;

    /// The ranges and the resulting scale
    SUMOReal myMin, myMax, myScale;

    /// The parameter to use to retrieve the information/value
    _P myParameter;

};


#endif

/****************************************************************************/

