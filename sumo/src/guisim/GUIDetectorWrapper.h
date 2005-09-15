#ifndef GUIDetectorWrapper_h
#define GUIDetectorWrapper_h
//---------------------------------------------------------------------------//
//                        GUIDetectorWrapper.h -
//  The base class for detector wrapper
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.14  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.13  2004/11/24 08:46:42  dkrajzew
// recent changes applied
//
// Revision 1.12  2004/07/02 08:41:00  dkrajzew
// using global selection storage
//
// Revision 1.11  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.10  2004/01/26 06:59:37  dkrajzew
// work on detectors: e3-detectors loading and visualisation;
//  variable offsets and lengths for lsa-detectors;
//  coupling of detectors to tl-logics;
//  different detector visualistaion in dependence to his controller
//
// Revision 1.9  2003/11/18 14:27:39  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.8  2003/11/12 14:00:19  dkrajzew
// commets added; added parameter windows to all detectors
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/geom/HaveBoundary.h>
#include <utils/geom/Position2D.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIBaseDetectorDrawer;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIDetectorWrapper
 * This is the base class for detector wrapper; As detectors may have a
 * position only or additionally a length, different display mechanisms are
 * necessary. These must be implemented in class erived from this one,
 * according to the wrapped detectors' properties.
 *
 * The following methods must be implemented by derived classes:
 * drawGL_SG: This method shall draw the detector when simple geometry is used
 * drawGL_FG: This method shall draw the detector when full geometry is used
 * getPosition2D: shall return the position of the detector in order to make
 *  a zoom at the detector possible
 */
class GUIDetectorWrapper
    : public GUIGlObject_AbstractAdd {
public:
    /// Constructor
    GUIDetectorWrapper(GUIGlObjectStorage &idStorage,
        std::string id);

    /// Constructor for collectors over somthing
    GUIDetectorWrapper(GUIGlObjectStorage &idStorage,
        std::string id, size_t glID);

    /// Destructor
    ~GUIDetectorWrapper();

    /** @brief Returns the popup-menu of this detector */
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns the type of the object (always GLO_DETECTOR)
    GUIGlObjectType getType() const;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

