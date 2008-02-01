/****************************************************************************/
/// @file    GUIPolygon2D.h
/// @author  Daniel Krajzewicz
/// @date    June 2006
/// @version $Id$
///
// The GUI-version of a polygon
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
#ifndef GUIPolygon2D_h
#define GUIPolygon2D_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/shapes/Polygon2D.h>
#include <utils/gui/globjects/GUIGlObject.h>


// ===========================================================================
// class definitions
// ===========================================================================
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


    /// @name inherited from GUIGlObject
    //@{

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


#endif

/****************************************************************************/

