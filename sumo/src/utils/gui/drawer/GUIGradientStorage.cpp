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
// Revision 1.4  2005/09/23 06:07:54  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/09/15 12:19:10  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 09:16:39  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/11/23 10:38:30  dkrajzew
// debugging
//
// Revision 1.2  2004/11/22 12:59:50  dksumo
// 'scientific' gradient added
//
// Revision 1.1  2004/10/22 12:50:47  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.1  2004/08/02 11:53:57  dkrajzew
// gradients added
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

#include "GUIGradientStorage.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
                (SUMOReal) (FXREDVAL(ramp[i])/255.0),
                (SUMOReal) (FXGREENVAL(ramp[i])/255.0),
                (SUMOReal) (FXBLUEVAL(ramp[i])/255.0)));
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
