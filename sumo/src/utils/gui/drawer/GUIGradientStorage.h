/****************************************************************************/
/// @file    GUIGradientStorage.h
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// A storage for gradient definitions
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
#ifndef GUIGradientStorage_h
#define GUIGradientStorage_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <fx.h>
#include <FXGradientBar.h>

#include <utils/common/RGBColor.h>


// ===========================================================================
// class definitions
// ===========================================================================
class GUIGradientStorage {
public:
    /// Gradient enums
    enum GradientName {
        GRADIENT_GREEN_YELLOW_RED = 0,
        GRADIENT_SCIENTIFIC = 1,
        GRADIENT_MAX
    };

    /// Constructor
    GUIGradientStorage(FXComposite *vp);

    /// Destructor
    ~GUIGradientStorage();

    /// Returns the named gradient as a list of colors
    std::vector<RGBColor> getRGBColors(GradientName idx, FXint noCells);

    /// Adds a gradient
    void set(GradientName idx, const std::vector<FXGradient> &gradient);

private:
    /// Definition of a complete gradient class
    typedef std::vector<FXGradient> CompleteGradient;

    /// Definition of a storage for gradients
    typedef std::vector<CompleteGradient> Gradients;

    /// The list of available gradients
    Gradients myGradients;

    /// A virtual parent needed to build gradients
    FXComposite *myVirtualParent;

};


#endif

/****************************************************************************/

