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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2004/08/02 11:53:57  dkrajzew
// gradients added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIGradientStorage.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIGradientStorage::GUIGradientStorage(FXComposite *vp)
    : myVirtualParent(vp)
{
    // build defaults
    FXGradient tmpG;
    std::vector<FXGradient> tmpGB;
    // red-yellow-green
    tmpG.lower = 0;                     /// Lower value
    tmpG.middle = .25;                  /// Middle value
    tmpG.upper = .5;                    /// Upper value
    tmpG.lowerColor = FXRGB(0, 255, 0); /// Lower color
    tmpG.upperColor = FXRGB(230, 136, 0); /// Upper color
    tmpG.blend = GRADIENT_BLEND_LINEAR; /// Blend method
    tmpGB.push_back(tmpG);
    tmpG.lower = 0.5;                     /// Lower value
    tmpG.middle = .75;                  /// Middle value
    tmpG.upper = 1;                    /// Upper value
    tmpG.lowerColor = FXRGB(230, 136, 0); /// Lower color
    tmpG.upperColor = FXRGB(0, 255, 0); /// Upper color
    tmpGB.push_back(tmpG);
    myGradients.push_back(tmpGB);
}


GUIGradientStorage::~GUIGradientStorage()
{
}


std::vector<RGBColor>
GUIGradientStorage::getRGBColors(GradientName idx, size_t noCells)
{
    size_t i;
    FXGradientBar *bar =
        new FXGradientBar(myVirtualParent);
    FXGradient *gradients;
    FXMALLOC(&gradients, FXGradient, myGradients[idx].size());
    for(i=0; i<myGradients[idx].size(); i++) {
        gradients[i] = myGradients[idx][i];
    }
    bar->setGradients(gradients, myGradients[idx].size());

    FXColor *ramp;
    FXMALLOC(&ramp, FXColor, noCells);
    bar->gradient(ramp, noCells);
    std::vector<RGBColor> ret;
    for(i=0; i<noCells; i++) {
        ret.push_back(
            RGBColor(
                ((double) FXREDVAL(ramp[i]))/255.0,
                ((double) FXGREENVAL(ramp[i]))/255.0,
                ((double) FXBLUEVAL(ramp[i]))/255.0));
    }
    FXFREE(&ramp);
    delete bar;
    return ret;
}


void
GUIGradientStorage::set(GradientName idx,
                        const std::vector<FXGradient> &gradient)
{
    myGradients[idx] = gradient;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
