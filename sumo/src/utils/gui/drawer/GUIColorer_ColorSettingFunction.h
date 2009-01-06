/****************************************************************************/
/// @file    GUIColorer_ColorSettingFunction.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 22. Dec. 2005
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
#ifndef GUIColorer_ColorSettingFunction_h
#define GUIColorer_ColorSettingFunction_h


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
 * @class GUIColorer_ColorSettingFunction
 * @brief Does not set a color directly, but calls a function that does it
 */
template<class T>
class GUIColorer_ColorSettingFunction : public GUIBaseColorer<T>
{
public:
    /// Type of the function to execute.
    typedef void(T::* Operation)() const;

    /// Constructor
    GUIColorer_ColorSettingFunction(Operation operation)
            : myOperation(operation) {}

    /// Destructor
    virtual ~GUIColorer_ColorSettingFunction() { }

    /// @name inherited from GUIBaseColorer
    //@{

    /// Sets the color using a value from the given instance of T
    void setGlColor(const T& i) const {
        (i.*myOperation)();
    }

    /// Sets the color using the given value
    void setGlColor(SUMOReal) const {
        glColor3d(1, 1, 0);
    }

    /// Returns the type of this class (CST_STATIC)
    virtual ColorSetType getSetType() const {
        return CST_STATIC;
    }
    //@}

    /// @name inherited from from GUIBaseColorerInterface
    //@{

    virtual void resetColor(const RGBColor &) {}

    virtual const RGBColor &getSingleColor() const {
        throw 1;
    }
    //@}

protected:
    /// The operation that sets the color
    Operation myOperation;

};


#endif

/****************************************************************************/

