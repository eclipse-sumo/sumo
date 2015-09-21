/****************************************************************************/
/// @file    GNECrossing.h
/// @author  Jakob Erdmann
/// @date    June 2011
/// @version $Id: GNECrossing.h 18830 2015-09-11 08:57:34Z behrisch $
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNECrossing_h
#define GNECrossing_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <netbuild/NBNode.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GUIGLObjectPopupMenu;
class PositionVector;
class GNEJunction;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECrossing
 * @brief This object is responsible for drawing a shape and for supplying a
 * a popup menu. Messages are routeted to an internal dataTarget and to the
 * editor (hence inheritance from FXDelegator)
 */
class GNECrossing : public GUIGlObject {

public:

    /** @brief Constructor
     * @param[in] editor The editor to notify about changes
     * @param[in] id The id of this internal lane
     * @param[in] shape The shape of the lane
     * @param[in] tlIndex The tl-index of the lane
     */
    GNECrossing(GNEJunction& parentJunction, const std::string& id);

    /// @brief Destructor
    virtual ~GNECrossing() ;

    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app,
                                       GUISUMOAbstractView& parent) ;

    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app,
            GUISUMOAbstractView& parent) ;

    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const ;

    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const ;
    //@}



private:
    /// @brief the parent junction of this crossing
    const GNEJunction& myParentJunction;

    /// @brief the data for this crossing
    const NBNode::Crossing& myCrossing;

    /// @brief the shape of the edge
    const PositionVector myShape;

    /// @name computed only once (for performance) in updateGeometry()
    //@{
    /// The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;
    //@}

    /// @brief the created popup
    GUIGLObjectPopupMenu* myPopup;

private:

    /// @brief Invalidated copy constructor.
    GNECrossing(const GNECrossing&);

    /// @brief Invalidated assignment operator.
    GNECrossing& operator=(const GNECrossing&);

};


#endif

/****************************************************************************/

