#ifndef GUIDetectorDrawer_FGwT_h
#define GUIDetectorDrawer_FGwT_h
//---------------------------------------------------------------------------//
//                        GUIDetectorDrawer_FGwT.h -
//  Class for drawing detectors with tooltips
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 02.09.2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.3  2004/01/26 06:39:18  dkrajzew
// visualisation of e3-detectors added; documentation added
//
// Revision 1.2  2003/09/25 08:59:28  dkrajzew
// documentation patched
//
// Revision 1.1  2003/09/23 14:28:16  dkrajzew
// possibility to visualise detectors using different geometry complexities added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIBaseDetectorDrawer.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIDetectorDrawer_FGwT
 * Derived from GUIBaseDetectorDrawer, this class implements the method that
 *  draws the detectors. For each detector two steps are performed:
 *  The tooltip information is set first. Then, the method to draw the detector
 *  in full geometry mode is called.
 */
class GUIDetectorDrawer_FGwT
        : public GUIBaseDetectorDrawer
{
public:
    ///  Constructor
    GUIDetectorDrawer_FGwT(std::vector<GUIDetectorWrapper*> &detectors)
        : GUIBaseDetectorDrawer(detectors) { }

    /// Destructor
    ~GUIDetectorDrawer_FGwT() { }

protected:
    /// Draws the detectors
    void myDrawGLDetectors(size_t *which, size_t maxDetectors,
        double scale);

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
