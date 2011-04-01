/****************************************************************************/
/// @file    GUIPointOfInterest.h
/// @author  Daniel Krajzewicz
/// @date    June 2006
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIPointOfInterest_h
#define GUIPointOfInterest_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/shapes/PointOfInterest.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObjectStorage;


// ===========================================================================
// class definitions
// ===========================================================================
/*
 * @class GUIPointOfInterest
 * @brief The GUI-version of a point of interest
 */
class GUIPointOfInterest : public PointOfInterest, public GUIGlObject_AbstractAdd {
public:
    /** @brief Constructor
     * @param[in] idStorage The gl-id storage for giving this object an gl-id
     * @param[in] layer The layer the PoI will be located in
     * @param[in] id The name of the PoI
     * @param[in] type The type of the PoI
     * @param[in] p The position of the PoI
     * @param[in] c The color of the PoI
     */
    GUIPointOfInterest(GUIGlObjectStorage &idStorage, int layer,
                       const std::string &id, const std::string &type,
                       const Position2D &p, const RGBColor &c) throw();

    /// @brief Destructor
    virtual ~GUIPointOfInterest() throw();



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
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
            GUISUMOAbstractView &parent) throw();


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const throw();


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings &s) const throw();
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

