#ifndef GUIColorer_ColorSettingFunction_h
#define GUIColorer_ColorSettingFunction_h
//---------------------------------------------------------------------------//
//                        GUIColorer_ColorSettingFunction.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 22. Dec. 2005
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
// Revision 1.3  2006/12/18 08:25:24  dkrajzew
// consolidation of setting colors
//
// Revision 1.2  2006/11/16 12:30:54  dkrajzew
// warnings removed
//
// Revision 1.1  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
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
 * @class GUIColorer_ColorSettingFunction
 * @brief Does not set a color directly, but calls a function that does it
 */
template<class _T>
class GUIColorer_ColorSettingFunction : public GUIBaseColorer<_T> {
public:
    /// Type of the function to execute.
    typedef void ( _T::* Operation )() const;

    /// Constructor
    GUIColorer_ColorSettingFunction(Operation operation)
        : myOperation(operation)
    {
    }

    /// Destructor
	virtual ~GUIColorer_ColorSettingFunction() { }

    ///{ from GUIBaseColorer
    /// Sets the color using a value from the given instance of _T
	void setGlColor(const _T& i) const {
        (i.*myOperation)();
    }

    /// Sets the color using the given value
	void setGlColor(SUMOReal ) const {
        glColor3d(1, 1, 0);
    }

    /// Returns the type of this class (CST_STATIC)
    virtual ColorSetType getSetType() const {
        return CST_STATIC;
    }
    ///}

    ///{ from GUIBaseColorerInterface
    virtual void resetColor(const RGBColor &) {
    }

    virtual const RGBColor &getSingleColor() const {
        throw 1;
    }
    ///}

protected:
    /// The operation that sets the color
    Operation myOperation;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
