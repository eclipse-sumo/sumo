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
// Revision 1.11  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.10  2004/01/26 06:59:37  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics; different detector visualistaion in dependence to his controller
//
// Revision 1.9  2003/11/18 14:27:39  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.8  2003/11/12 14:00:19  dkrajzew
// commets added; added parameter windows to all detectors
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/geom/HaveBoundery.h>
#include <utils/geom/Position2D.h>
#include <gui/GUIGlObject.h>
#include <gui/GUISUMOAbstractView.h>


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
    : public HaveBoundery, public GUIGlObject {
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
    GUIGLObjectPopupMenu *getPopUpMenu(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns the type of the object (always GLO_DETECTOR)
    GUIGlObjectType getType() const;

    /// Draws the detector in full-geometry mode
    virtual void drawGL_FG(double scale,
        GUIBaseDetectorDrawer &drawer) const = 0;

    /// Draws the detector in simple-geometry mode
    virtual void drawGL_SG(double scale,
        GUIBaseDetectorDrawer &drawer) const = 0;

    /// Returns the detector's coordinates
    virtual Position2D getPosition() const = 0;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

