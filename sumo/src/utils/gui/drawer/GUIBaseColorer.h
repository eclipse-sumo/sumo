/****************************************************************************/
/// @file    GUIBaseColorer.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
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
#ifndef GUIBaseColorer_h
#define GUIBaseColorer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>


// ===========================================================================
// enumeration definitions
// ===========================================================================
/**
 * @enum ColorSetType
 * @brief differentiation by number of colors that may be set
 *
 * Each colorer describes using this enumeration what kind of a color
 *  set (two colors, each for min/max, palette, gradient, etc.) it uses.
 * It is used to determine which color changing interactions shall be
 *  instatiated.
 */
enum ColorSetType {
    /// A single color is used
    CST_SINGLE,
    /// A gradient is used
    CST_GRADIENT,
    /// A palette is used
    CST_PALETTE,
    /// Two colors are used, one for the minimum value, one for the maximum
    CST_MINMAX,
    /// Almost as CST_MINMAX, but an additional color is used for the case avalue can not be retrieved
    CST_MINMAX_OPT,
    /// A set of colors is used
    CST_SET,
    /// No changeable colors
    CST_STATIC
};


// ===========================================================================
// class definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * definition of GUIBaseColorerInterface
 * ----------------------------------------------------------------------- */
/**
 * @class GUIBaseColorerInterface
 * @brief A somehow completely abstract class which allows to change the colors
 *  the subclass uses.
 *
 * Basically, this class has the same purpose as GUIBaseColorer, but is not
 *  a template what allows some further tweaks...
 * (!!! probably, this should be described better)
 */
class GUIBaseColorerInterface
{
public:
    /// Constructor
    GUIBaseColorerInterface() { }

    /// Destructor
    virtual ~GUIBaseColorerInterface() { }

    /// @name color setting functions
    //@{

    /// Sets the given color as the color to use (valid for CST_SINGLE)
    virtual void resetColor(const RGBColor &) = 0;

    /// Sets the given color as the colors to use (valid for CST_MINMAX)
    virtual void resetColor(const RGBColor &, const RGBColor &) = 0;

    /// Sets the given color as the colors to use (valid for CST_MINMAX_OPT)
    virtual void resetColor(const RGBColor &, const RGBColor &, const RGBColor &) = 0;

    /// Sets the given colors as the colors to use (valid for ?!!!)
    virtual void resetColor(const std::vector<RGBColor> &) = 0;
    //@}


    /// @name color retrieval functions
    //@{

    /// Returns the single color used (valid for CST_SINGLE)
    virtual const RGBColor &getSingleColor() const = 0;

    /// Returns the color used for minimum values (valid for CST_MINMAX)
    virtual const RGBColor &getMinColor() const = 0;

    /// Returns the color used for maximum values (valid for CST_MINMAX)
    virtual const RGBColor &getMaxColor() const = 0;

    /// Returns the color used when a value can not be retrieved (valid for CST_MINMAX_OPT)
    virtual const RGBColor &getFallbackColor() const = 0;

    /// Returns the colors used (valid for ?!!!)
    virtual const std::vector<RGBColor> &getGradient() const = 0;
    //@}

};


/* -------------------------------------------------------------------------
 * definition of GUIBaseColorer
 * ----------------------------------------------------------------------- */
/**
 * @class GUIBaseColorer
 * @brief Base class for coloring. Allows changing the used colors and sets
 *  the used color in dependence to a value or a given structure.
 *
 * We need a template derivation of @class GUIBaseColorerInterface because
 *  they may be used for different things (vehicles, lanes, etc.) and need
 *  to be parametrised with the instances. This was the idea;
 *
 * As @class GUIBaseColorer is still not a "final" class, we maybe should get
 *  rid of it.
 */
template<class T>
class GUIBaseColorer : public GUIBaseColorerInterface
{
public:
    /// Constructor
    GUIBaseColorer() { }

    /// Destructor
    virtual ~GUIBaseColorer() { }

    /// Sets the color using a value from the given instance of T
    virtual void setGlColor(const T& i) const = 0;

    /// Sets the color using the given value
    virtual void setGlColor(SUMOReal val) const {
        glColor3d(val, val, val);
    }

    /// Returns the type of this class (CST_SINGLE)
    virtual ColorSetType getSetType() const {
        return CST_SINGLE;
    }

    /// @name inherited from from GUIBaseColorerInterface
    //@{

    /// Sets the given color as the color to use (valid for CST_SINGLE)
    virtual void resetColor(const RGBColor &) { }
    /// Sets the given color as the colors to use (valid for CST_MINMAX)
    virtual void resetColor(const RGBColor &, const RGBColor &) { }
    /// Sets the given color as the colors to use (valid for CST_MINMAX_OPT)
    virtual void resetColor(const RGBColor &, const RGBColor &, const RGBColor &) { }
    /// Sets the given colors as the colors to use (valid for ?!!!)
    virtual void resetColor(const std::vector<RGBColor> &) { }

    /// Returns the single color used (valid for CST_SINGLE)
    virtual const RGBColor &getSingleColor() const {
        throw 1;
    }
    /// Returns the color used for minimum values (valid for CST_MINMAX)
    virtual const RGBColor &getMinColor() const {
        throw 1;
    }
    /// Returns the color used for maximum values (valid for CST_MINMAX)
    virtual const RGBColor &getMaxColor() const {
        throw 1;
    }
    /// Returns the color used when a value can not be retrieved (valid for CST_MINMAX_OPT)
    virtual const RGBColor &getFallbackColor() const {
        throw 1;
    }
    /// Returns the colors used (valid for ?!!!)
    virtual const std::vector<RGBColor> &getGradient() const {
        throw 1;
    }
    //@}

};


#endif

/****************************************************************************/

