#ifndef GUIColorer_ByDeviceNumber_h
#define GUIColorer_ByDeviceNumber_h
//---------------------------------------------------------------------------//
//                        GUIColorer_ByDeviceNumber.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 2. Jan 2006
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
// Revision 1.1  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
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

template<class _T, class _P>
class GUIColorer_ByDeviceNumber : public GUIBaseColorer<_T> {
public:
    /// Type of the function to execute.
    typedef bool ( _T::* HasOperation )(_P) const;
    typedef SUMOReal ( _T::* NumberOperation )(_P) const;

    GUIColorer_ByDeviceNumber(HasOperation hasOperation,
        NumberOperation numberOperation,
        bool catchNo, SUMOReal min, SUMOReal max,
        const RGBColor &no, const RGBColor &minColor, const RGBColor &maxColor,
        _P param)
        : myHasOperation(hasOperation), myNumberOperation(numberOperation),
        myNoColor(no), myMinColor(minColor), myMaxColor(maxColor),
        myMin(min), myMax(max), myParameter(param)
    {
        myScale = (SUMOReal) 1.0 / (myMax-myMin);
    }

	virtual ~GUIColorer_ByDeviceNumber() { }

	void setGlColor(const _T& i) const {
        if(!(i.*myHasOperation)((_P) myParameter)) {
            if(myCatchNo) {
                mglColor(myNoColor);
            } else {
                mglColor(myMinColor);
            }
        } else {
            SUMOReal val = (i.*myNumberOperation)((_P) myParameter) - myMin;
            if(val<myMin) {
                val = myMin; // !!! Aua!!!
            } else if(val>myMax) {
                val = myMax; // !!! Aua!!!
            }
            val = val * myScale;
            RGBColor c =
                (myMinColor * ((SUMOReal) 1.0 - val)) + (myMaxColor * val);
            glColor3d(c.red(), c.green(), c.blue());
        }
	}

	void setGlColor(SUMOReal val) const {
        glColor3d(1, 1, 0);
    }

    virtual ColorSetType getSetType() const {
        return CST_STATIC;
    }

    virtual void resetColor(const RGBColor &min) {
    }

    virtual const RGBColor &getSingleColor() const {
        throw 1;
    }

protected:
    HasOperation myHasOperation;
    NumberOperation myNumberOperation;
    bool myCatchNo;
    RGBColor myNoColor, myMinColor, myMaxColor;
    SUMOReal myMin, myMax, myScale;
    _P myParameter;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
