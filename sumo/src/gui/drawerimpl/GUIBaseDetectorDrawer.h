#ifndef GUIBaseDetectorDrawer_h
#define GUIBaseDetectorDrawer_h
//---------------------------------------------------------------------------//
//                        GUIBaseDetectorDrawer.h -
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



class GUIGlObject_AbstractAdd;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIBaseDetectorDrawer
{
public:
    /// Constructor
    GUIBaseDetectorDrawer(const std::vector<GUIGlObject_AbstractAdd*> &additionals);

    /// Destructor
    virtual ~GUIBaseDetectorDrawer();

    /// Draws all detectors
    void drawGLDetectors(size_t *which, size_t maxDetectors,
        SUMOReal scale, SUMOReal upscale);

protected:
    /** @brief Virtual function to be implemented taht actually performs the drawing
        The drawing process depends on whether the full or the simply geometry shall
        be used and whether tool tips shall be displayed or not */
    virtual void myDrawGLDetectors(size_t *which, size_t maxDetectors,
        SUMOReal scale, SUMOReal upscale) = 0;

protected:
    /** @brief Inititialises the drawing process
        The textures have to be initialised */
    void initStep();

protected:
    /// The list of detectors to consider at drawing
    const std::vector<GUIGlObject_AbstractAdd*> &myAdditionals;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
