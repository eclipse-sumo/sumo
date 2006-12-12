#ifndef GUIColorer_ShadeByCastedFunctionValue_h
#define GUIColorer_ShadeByCastedFunctionValue_h
//---------------------------------------------------------------------------//
//                        GUIColorer_ShadeByCastedFunctionValue.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 12.12.2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
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
// Revision 1.1  2006/12/12 12:19:26  dkrajzew
// removed simple/full geometry options; everything is now drawn using full geometry
//
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

template<class _T, class _C>
class GUIColorer_ShadeByCastedFunctionValue : public GUIBaseColorer<_T> {
public:
    /// Type of the function to execute.
    typedef _C ( _T::* Operation )() const;

    GUIColorer_ShadeByCastedFunctionValue(SUMOReal min, SUMOReal max,
        const RGBColor &minC, const RGBColor &maxC,
        Operation operation)
        : myMin(min), myMax(max), myMinColor(minC), myMaxColor(maxC),
            myOperation(operation)
    {
        myScale = (SUMOReal) 1.0 / (myMax-myMin);
    }

	virtual ~GUIColorer_ShadeByCastedFunctionValue() { }

	void setGlColor(const _T& i) const {
        SUMOReal val = (SUMOReal) (i.*myOperation)() - myMin;
        std::cout << val << endl;
        if(val==-1) {
            glColor3f(0.8f, 0.8f, 0.8f);
        } else {
            if(val<myMin) {
                val = myMin;
            } else if(val>myMax) {
                val = myMax;
            }
            val = val * myScale;
            RGBColor c =
                (myMinColor * ((SUMOReal) 1.0 - val)) + (myMaxColor * val);
            glColor3d(c.red(), c.green(), c.blue());
        }
	}

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
            RGBColor c =
                (myMinColor * ((SUMOReal) 1.0 - val)) + (myMaxColor * val);
            glColor3d(c.red(), c.green(), c.blue());
        }
    }

    virtual ColorSetType getSetType() const {
        return CST_MINMAX;
    }

    virtual void resetColor(const RGBColor &min, const RGBColor &max) {
        myMinColor = min;
        myMaxColor = max;
    }

    virtual const RGBColor &getMinColor() const {
        return myMinColor;
    }

    virtual const RGBColor &getMaxColor() const {
        return myMaxColor;
    }

protected:
    SUMOReal myMin, myMax, myScale;
    RGBColor myMinColor, myMaxColor;

    /// The object's operation to perform.
    Operation myOperation;


};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
