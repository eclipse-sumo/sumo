#ifndef GUIColorer_ShadeByFunctionValue_h
#define GUIColorer_ShadeByFunctionValue_h
//---------------------------------------------------------------------------//
//                        GUIColorer_ShadeByFunctionValue.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.10  2006/12/13 07:22:20  dkrajzew
// removed unneeded coloring options
//
// Revision 1.9  2006/12/12 12:19:23  dkrajzew
// removed simple/full geometry options; everything is now drawn using full geometry
//
// Revision 1.8  2006/06/22 07:18:01  dkrajzew
// handling of fase values added
//
// Revision 1.7  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.6  2005/10/07 11:45:09  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/23 06:07:54  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:19:10  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 12:49:08  dkrajzew
// code style adapted
//
// Revision 1.4  2005/06/14 11:29:50  dksumo
// documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "GUIBaseColorer.h"
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include <utils/gui/div/GUIGlobalSelection.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIColorer_ShadeByFunctionValue
 * @brief Colors by using a retrieved value; the function must return a SUMOReal
 */
template<class _T>
class GUIColorer_ShadeByFunctionValue : public GUIBaseColorer<_T> {
public:
    /// Type of the function to execute.
    typedef SUMOReal ( _T::* Operation )() const;

    /// Constructor
    GUIColorer_ShadeByFunctionValue(SUMOReal min, SUMOReal max,
        const RGBColor &minC, const RGBColor &maxC,
        Operation operation)
        : myMin(min), myMax(max), myMinColor(minC), myMaxColor(maxC),
            myOperation(operation)
    {
        myScale = (SUMOReal) 1.0 / (myMax-myMin);
    }

    /// Destructor
    virtual ~GUIColorer_ShadeByFunctionValue() { }


    ///{ from GUIBaseColorer
    /// Sets the color using a value from the given instance of _T
    void setGlColor(const _T& i) const {
        SUMOReal val = (i.*myOperation)() - myMin;
        if(val==-1) {
            glColor3f(0.8f, 0.8f, 0.8f);
        } else {
            if(val<myMin) {
                val = myMin;
            } else if(val>myMax) {
                val = myMax;
            }
            val = val * myScale;
            RGBColor c = (myMinColor * ((SUMOReal) 1.0 - val)) + (myMaxColor * val);
            glColor3d(c.red(), c.green(), c.blue());
        }
    }

    /// Sets the color using the given value
    void setGlColor(SUMOReal val) const {
        if(val==-1) {
            glColor3f(0.8f, 0.8f, 0.8f);
        } else {
            if(val<myMin) {
                val = myMin;
            } else if(val>myMax) {
                val = myMax;
            }
            val = val * myScale;
            RGBColor c = (myMinColor * ((SUMOReal) 1.0 - val)) + (myMaxColor * val);
            glColor3d(c.red(), c.green(), c.blue());
        }
    }

    /// Returns the type of this class (CST_MINMAX)
    virtual ColorSetType getSetType() const {
        return CST_MINMAX;
    }
    ///}


    ///{ from GUIBaseColorerInterface
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
    ///}

protected:
    /// The ranges and the resulting scale
    SUMOReal myMin, myMax, myScale;

    /// The colors to use
    RGBColor myMinColor, myMaxColor;

    /// The object's operation to perform.
    Operation myOperation;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
