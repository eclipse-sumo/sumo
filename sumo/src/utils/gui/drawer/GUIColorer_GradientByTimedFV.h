#ifndef GUIColorer_GradientByTimedFV_h
#define GUIColorer_GradientByTimedFV_h
//---------------------------------------------------------------------------//
//                        GUIColorer_GradientByTimedFV.h -
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
#include <config.h>
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
 *
 */
template<class _T>
class GUIColorer_GradientByTimedFV : public GUIBaseColorer<_T> {
public:
    /// Type of the function to execute.
    typedef double ( _T::* Operation )() const;

    GUIColorer_GradientByTimedFV(double min, double max,
        const std::vector<RGBColor> &gradient, SUMOTime timeGiver,
        Operation operation)
        : myMin(min), myMax(max), myGradient(gradient),
            myTimeGiver(timeGiver), myOperation(operation)
    {
        myScale = 1.0 / (myMax-myMin);
    }

	virtual ~GUIColorer_GradientByTimedFV() { }

	void setGlColor(const _T& i) const {
        double val = (i.*myOperation)() - myMin;
        val = val * myScale;
        if(val==-1) {
            glColor3f(0.5, 0.5, 0.5);
        } else {
            if(val<0) {
                val = 0; // !!! Aua!!!
            } else if(val>1) {
                val = 1; // !!! Aua!!!
            }
            int idx = (int) (val * (double) (myGradient.size()-1));
            assert(idx<myGradient.size());
            const RGBColor &c = myGradient[idx];
            glColor3f(c.red(), c.green(), c.blue());
	}

	void setGlColor(double val) const {
        if(val<0) {
            val = 0; // !!! Aua!!!
        } else if(val>1) {
            val = 1; // !!! Aua!!!
        }
        int idx = (int) (val * (double) (myGradient.size()-1));
        assert(idx<(int) myGradient.size());
        const RGBColor &c = myGradient[idx];
        glColor3d(c.red(), c.green(), c.blue());
    }

protected:
    double myMin, myMax, myScale;
    std::vector<RGBColor> myGradient;

    /// The object's operation to perform.
    Operation myOperation;


};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
