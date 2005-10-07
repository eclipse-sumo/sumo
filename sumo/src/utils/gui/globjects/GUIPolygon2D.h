#ifndef GUIPolygon2D_h
#define GUIPolygon2D_h
//---------------------------------------------------------------------------//
//                        GUIPolygon2D.h -
//  A 2d-polygon
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
//  copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
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
class GUIPolygon2D : public Polygon2D, public GUIGlObject
{
public:
    /// Constructor
    GUIPolygon2D(GUIGlObjectStorage &idStorage,
        const std::string name, const std::string type,
        const RGBColor &color, const Position2DVector &Pos);

    /// Destructor
    ~GUIPolygon2D();



    /// Returns an own popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent);

    /// Returns an own parameter window
    GUIParameterTableWindow *getParameterWindow(
        GUIMainWindow &app, GUISUMOAbstractView &parent);

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /// Returns the information whether this object is still active
    bool active() const;

	/// Returns the boundary to which the object shall be centered
	Boundary getCenteringBoundary() const;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif // !defined(AFX_GUIPolygon2D_H__4491EABC_872C_4BA2_8A8F_FA8045E3D4DD__INCLUDED_)

// Local Variables:
// mode:C++
// End:


