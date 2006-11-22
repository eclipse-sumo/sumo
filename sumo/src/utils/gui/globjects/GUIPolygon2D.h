#ifndef GUIPolygon2D_h
#define GUIPolygon2D_h
/***************************************************************************
                          GUIPolygon2D.h
    The GUI-version of a polygon
                             -------------------
    project              : SUMO - Simulation of Urban MObility
    begin                : June 2006
    copyright            : (C) 2006 by Daniel Krajzewicz
    organisation         : IVF/DLR http://ivf.dlr.de
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2006/11/22 13:06:46  dkrajzew
// patching problems on choosing an object when using shapes within different layers
//
// Revision 1.6  2006/10/12 10:14:32  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
// Revision 1.5  2006/08/01 07:36:47  dkrajzew
// API for drawing not filled circles
//
// Revision 1.4  2006/04/18 08:12:05  dkrajzew
// consolidation of interaction with gl-objects
//
// Revision 1.3  2006/04/11 11:05:44  dkrajzew
// code beautifying: embedding string in strings removed
//
// Revision 1.2  2005/10/07 11:45:32  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.1  2005/09/15 12:19:44  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2005/09/09 12:55:25  dksumo
// shape handling added
//
// Revision 1.3  2005/08/01 05:57:15  dksumo
// style adapted
//
// Revision 1.2  2005/06/14 11:29:50  dksumo
// documentation added
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

#include <string>
#include <utils/shapes/Polygon2D.h>
#include <utils/gui/globjects/GUIGlObject.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/*
 * @class GUIPolygon2D
 * @brief The GUI-version of a polygon
 */
class GUIPolygon2D : public Polygon2D, public GUIGlObject
{
public:
    /// Constructor
    GUIPolygon2D(GUIGlObjectStorage &idStorage, int layer,
        const std::string name, const std::string type,
        const RGBColor &color, const Position2DVector &Pos,
        bool fill);

    /// Destructor
    ~GUIPolygon2D();


    //@{ From GUIGlObject
    /// Returns the popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns the parameter window
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the id of the object as known to microsim
    const std::string &microsimID() const;

    /// Returns the information whether this object is still active
    bool active() const;

	/// Returns the boundary to which the object shall be centered
	Boundary getCenteringBoundary() const;
    //@}


    /// Returns the layer the object is located in
    int getLayer() const;

protected:
    /** @brief The layer this object is located in
     *
     * This value is used for determining which object to choose as being on top under the cursor
     */
    int myLayer;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


