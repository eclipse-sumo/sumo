#ifndef GUIDetectorDrawer_h
#define GUIDetectorDrawer_h
//---------------------------------------------------------------------------//
//                        GUIDetectorDrawer.h -
//  Base class for detector drawing
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 14.Jan 2004
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
// Revision 1.2  2007/01/08 12:11:18  dkrajzew
// visualization of poi and detector names added
//
// Revision 1.1  2006/12/12 12:10:39  dkrajzew
// removed simple/full geometry options; everything is now drawn using full geometry
//
// Revision 1.8  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.7  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.3  2004/07/02 08:12:12  dkrajzew
// detector drawers now also draw other additional items
//
// Revision 1.2  2004/03/19 12:34:30  dkrajzew
// porting to FOX
//
// Revision 1.1  2004/01/26 06:39:41  dkrajzew
// visualisation of e3-detectors added; documentation added
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

#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/glutils/GLHelper.h>



class GUIGlObject_AbstractAdd;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIDetectorDrawer
{
public:
    /// Constructor
    GUIDetectorDrawer(const std::vector<GUIGlObject_AbstractAdd*> &additionals);

    /// Destructor
    virtual ~GUIDetectorDrawer();

    /// Draws all detectors
    void drawGLDetectors(size_t *which, size_t maxDetectors,
        SUMOReal scale, GUISUMOAbstractView::VisualizationSettings &settings);

    void setGLID(bool val);

protected:
    /** @brief Inititialises the drawing process
        The textures have to be initialised */
    void initStep();

protected:
    /// The list of detectors to consider at drawing
    const std::vector<GUIGlObject_AbstractAdd*> &myAdditionals;

    /// Information whether the gl-id shall be set
    bool myShowToolTips;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
