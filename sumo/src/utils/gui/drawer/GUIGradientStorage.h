#ifndef GUIGradientStorage_h
#define GUIGradientStorage_h
//---------------------------------------------------------------------------//
//                        GUIGradientStorage.h -
//  A storage for gradient definitions
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.1  2004/11/23 10:38:30  dkrajzew
// debugging
//
// Revision 1.2  2004/11/22 12:59:50  dksumo
// 'scientific' gradient added
//
// Revision 1.1  2004/10/22 12:50:48  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.1  2004/08/02 11:53:57  dkrajzew
// gradients added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <fx.h>
#include <FXGradientBar.h>

#include <utils/gfx/RGBColor.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
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
    std::vector<RGBColor> getRGBColors(GradientName idx, size_t noCells);

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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
