/****************************************************************************/
/// @file    GUIDetectorWrapper.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for detector wrapper
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
#ifndef GUIDetectorWrapper_h
#define GUIDetectorWrapper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/geom/HaveBoundary.h>
#include <utils/geom/Position2D.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIBaseDetectorDrawer;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDetectorWrapper
 * This is the base class for detector wrapper; As detectors may have a
 * position only or additionally a length, different display mechanisms are
 * necessary. These must be implemented in class erived from this one,
 * according to the wrapped detectors' properties.
 */
class GUIDetectorWrapper
            : public GUIGlObject_AbstractAdd
{
public:
    /// Constructor
    GUIDetectorWrapper(GUIGlObjectStorage &idStorage,
                       std::string id) throw();

    /// Constructor for collectors over somthing
    GUIDetectorWrapper(GUIGlObjectStorage &idStorage,
                       std::string id, size_t glID) throw();

    /// Destructor
    ~GUIDetectorWrapper() throw();

    /** @brief Returns the popup-menu of this detector */
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent) throw();

    /// Returns the type of the object (always GLO_DETECTOR)
    GUIGlObjectType getType() const throw();

};


#endif

/****************************************************************************/

