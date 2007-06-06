/****************************************************************************/
/// @file    GUIColorer_ByDeviceState.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 2. Jan 2006
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
#ifndef GUIColorer_ByDeviceState_h
#define GUIColorer_ByDeviceState_h


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

template<class _T1, class _P>
class GUIColorer_ByDeviceState : public GUIBaseColorer<_T>
{
public:
    /// Type of the function to execute.
    typedef bool(_T::* HasOperation)(_P) const;
    typedef size_t(_T::* StateOperation1)(_P) const;
    typedef size_t(_T::* StateOperation2)() const;

    GUIColorer_ByDeviceState(HasOperation hasOperation,
                             NumberOperation numberOperation,
                             bool catchNo, const RGBColor &no, _P hasParam, _P stateParam)
            : myHasOperation(hasOperation), myNumberOperation(numberOperation),
            myNoColor(no), myHasParameter(hasParam), myStateParameter(stateParam)
    {}

    virtual ~GUIColorer_ByDeviceState()
    { }

    void setGlColor(const _T& i) const
    {
        if (!(i.*myHasOperation)((_P) myHasParameter)) {
            if (myCatchNo) {
                mglColor(myNoColor);
            } else {
                mglColor(myMinColor);
            }
        } else {
            const MSDevice_CPhone * const phone =
                (const MSDevice_CPhone * const)
                vehicle.getCORNPointerValue(MSCORN::CORN_P_VEH_DEV_CPHONE);
            size_t state = (i.*myNumberOperation)((_P) myStateParameter);
            const RGBColor &c = myColors.find(state)->second;
            glColor3f(c.red(), c.green(), c.blue());
        }
    }

    void addState(size_t state, const RGBColor &c)
    {
        myColors[state] = c;
    }

    void setGlColor(SUMOReal val) const
    {
        glColor3d(1, 1, 0);
    }

    virtual ColorSetType getSetType() const
    {
        return CST_STATIC;
    }

    virtual void resetColor(const RGBColor &min)
    {}

    virtual const RGBColor &getSingleColor() const
    {
        throw 1;
    }

protected:
    HasOperation myHasOperation;
    NumberOperation myNumberOperation;
    bool myCatchNo;
    RGBColor myNoColor;
    _P myHasParameter;
    _P myStateParameter;
    std::map<size_t, RGBColor> myColors;

};


#endif

/****************************************************************************/

