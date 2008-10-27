/****************************************************************************/
/// @file    GUINetWrapper.h
/// @author  Daniel Krajzewicz
/// @date
/// @version $Id$
///
// No geometrical information is hold, here. Still, some methods for
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
#ifndef GUINetWrapper_h
#define GUINetWrapper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utility>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/HaveBoundary.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIGlObject.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUINetWrapper
 */
class GUINetWrapper :
            public GUIGlObject,
            public HaveBoundary
{
public:
    /// constructor
    GUINetWrapper(GUIGlObjectStorage &idStorage,
                  GUINet &net) throw();

    /// destructor
    virtual ~GUINetWrapper() throw();

    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent) throw();


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow *getParameterWindow(
        GUIMainWindow &app, GUISUMOAbstractView &parent) throw();


    /** @brief Returns the id of the object as known to microsim
     *
     * @return An empty string (net has no id)
     * @see GUIGlObject::microsimID
     */
    const std::string &microsimID() const throw();

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const throw();

    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const throw();
    //@}

    Boundary getBoundary() const;

    GUINet &getNet() const;
    void drawGL(const GUIVisualizationSettings &s) const throw();

protected:
    GUINet &myNet;

};


#endif

/****************************************************************************/

