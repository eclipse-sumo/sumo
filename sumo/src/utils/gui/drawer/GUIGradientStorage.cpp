/****************************************************************************/
/// @file    GUIGradientStorage.cpp
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// }
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUIGradientStorage.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
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

    // scientific
    tmpGB.clear();
    tmpG.lower = 0;                     /// Lower value
    tmpG.middle = .125;                  /// Middle value
    tmpG.upper = .25;                    /// Upper value
    tmpG.lowerColor = FXRGB(0, 0, 255); /// Lower color
    tmpG.upperColor = FXRGB(0, 255, 255); /// Upper color
    tmpG.blend = GRADIENT_BLEND_LINEAR; /// Blend method
    tmpGB.push_back(tmpG);
    tmpG.lower = .25;                     /// Lower value
    tmpG.middle = .375;                  /// Middle value
    tmpG.upper = .5;                    /// Upper value
    tmpG.lowerColor = FXRGB(0, 255, 255); /// Lower color
    tmpG.upperColor = FXRGB(0, 255, 0); /// Upper color
    tmpG.blend = GRADIENT_BLEND_LINEAR; /// Blend method
    tmpGB.push_back(tmpG);
    tmpG.lower = .5;                     /// Lower value
    tmpG.middle = .625;                  /// Middle value
    tmpG.upper = .75;                    /// Upper value
    tmpG.lowerColor = FXRGB(0, 255, 0); /// Lower color
    tmpG.upperColor = FXRGB(255, 255, 0); /// Upper color
    tmpG.blend = GRADIENT_BLEND_LINEAR; /// Blend method
    tmpGB.push_back(tmpG);
    tmpG.lower = .75;                     /// Lower value
    tmpG.middle = .875;                  /// Middle value
    tmpG.upper = 1;                    /// Upper value
    tmpG.lowerColor = FXRGB(255, 255, 0); /// Lower color
    tmpG.upperColor = FXRGB(255, 0, 0); /// Upper color
    tmpG.blend = GRADIENT_BLEND_LINEAR; /// Blend method
    tmpGB.push_back(tmpG);
    myGradients.push_back(tmpGB);
}


GUIGradientStorage::~GUIGradientStorage()
{}


std::vector<RGBColor>
GUIGradientStorage::getRGBColors(GradientName idx, FXint noCells)
{
    size_t i;
    FXGradientBar *bar =
        new FXGradientBar(myVirtualParent);
    FXGradient *gradients;
    FXMALLOC(&gradients, FXGradient, myGradients[idx].size());
    for (i=0; i<myGradients[idx].size(); i++) {
        gradients[i] = myGradients[idx][i];
    }
    bar->setGradients(gradients, (FXint) myGradients[idx].size());

    FXColor *ramp;
    FXMALLOC(&ramp, FXColor, noCells);
    bar->gradient(ramp, noCells);
    std::vector<RGBColor> ret;
    for (i=0; i<noCells; i++) {
        ret.push_back(
            RGBColor(
                (SUMOReal)(FXREDVAL(ramp[i])/255.0),
                (SUMOReal)(FXGREENVAL(ramp[i])/255.0),
                (SUMOReal)(FXBLUEVAL(ramp[i])/255.0)));
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



/****************************************************************************/

