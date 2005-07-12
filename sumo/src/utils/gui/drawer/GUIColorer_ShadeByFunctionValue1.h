#ifndef GUIColorer_ShadeByFunctionValue1_h
#define GUIColorer_ShadeByFunctionValue1_h
//---------------------------------------------------------------------------//
//                        GUIColorer_ShadeByFunctionValue1.h -
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
// Revision 1.3  2005/07/12 12:49:08  dkrajzew
// code style adapted
//
// Revision 1.3  2005/06/14 11:29:50  dksumo
// documentation added
//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIBaseColorer.h"
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <utils/gui/div/GUIGlobalSelection.h>

template<class _T, class _P, class _PC>
class GUIColorer_ShadeByFunctionValue1 : public GUIBaseColorer<_T> {
public:
    /// Type of the function to execute.
    typedef double ( _T::* Operation )(_PC) const;

    GUIColorer_ShadeByFunctionValue1(double min, double max,
        const RGBColor &minC, const RGBColor &maxC,
        Operation operation, _P param)
        : myMin(min), myMax(max), myMinColor(minC), myMaxColor(maxC),
            myOperation(operation), myParameter(param)
    {
        myScale = 1.0 / (myMax-myMin);
    }

	virtual ~GUIColorer_ShadeByFunctionValue1() { }

	void setGlColor(const _T& i) const {
        double val = (i.*myOperation)((_PC) myParameter) - myMin;
        if(val==-1) {
            glColor3f(0.5, 0.5, 0.5);
        } else {
            if(val<myMin) {
                val = myMin; // !!! Aua!!!
            } else if(val>myMax) {
                val = myMax; // !!! Aua!!!
            }
            val = val * myScale;
            RGBColor c =
                (myMinColor * (1.0 - val)) + (myMaxColor * val);
            glColor3d(c.red(), c.green(), c.blue());
        }
	}

	void setGlColor(double val) const {
        if(val<myMin) {
            val = myMin; // !!! Aua!!!
        } else if(val>myMax) {
            val = myMax; // !!! Aua!!!
        }
        val = val * myScale;
        RGBColor c =
            (myMinColor * (1.0 - val)) + (myMaxColor * val);
        glColor3d(c.red(), c.green(), c.blue());
    }


protected:
    double myMin, myMax, myScale;
    RGBColor myMinColor, myMaxColor;

    /// The object's operation to perform.
    Operation myOperation;

    ///
    _P myParameter;

};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
