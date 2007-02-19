/****************************************************************************/
/// @file    GUIDetectorDrawer.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 14.Jan 2004
/// @version $Id$
///
// Base class for detector drawing
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
#ifndef GUIDetectorDrawer_h
#define GUIDetectorDrawer_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/glutils/GLHelper.h>



class GUIGlObject_AbstractAdd;


// ===========================================================================
// class definitions
// ===========================================================================
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


#endif

/****************************************************************************/

