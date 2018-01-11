/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECrossing.h
/// @author  Jakob Erdmann
/// @date    June 2011
/// @version $Id$
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
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

#include "GNENetElement.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GUIGLObjectPopupMenu;
class PositionVector;
class GNEJunction;
class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECrossing
 * @brief This object is responsible for drawing a shape and for supplying a
 * a popup menu. Messages are routeted to an internal dataTarget and to the
 * editor (hence inheritance from FXDelegator)
 */
class GNECrossing : public GNENetElement {
public:

    /**@brief Constructor
     * @param[in] parentJunction GNEJunction in which this crossing is placed
     * @param[in] crossing Node::Crossing
     */
    GNECrossing(GNEJunction* parentJunction, NBNode::Crossing* crossing);

    /// @brief Destructor
    virtual ~GNECrossing();

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief get parent Junction
    GNEJunction* getParentJunction() const;

    ///@brief get referente to NBode::Crossing
    NBNode::Crossing* getNBCrossing() const;

    /// @name inherited from GUIGlObject
    /// @{
    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

    /// @brief return true if a edge belongs to crossing's edges
    bool checkEdgeBelong(GNEEdge* edges) const;

    /// @brief return true if a edge of a vector of edges belongs to crossing's edges
    bool checkEdgeBelong(const std::vector<GNEEdge*>& edges) const;

protected:
    /// @brief the parent junction of this crossing
    GNEJunction* myParentJunction;

    /// @brief the data for this crossing
    NBNode::Crossing* myCrossing;

    /// @name computed only once (for performance) in updateGeometry()
    /// @{
    /// The rotations of the shape parts
    std::vector<double> myShapeRotations;

    /// The lengths of the shape parts
    std::vector<double> myShapeLengths;
    /// @}

private:
    /// @brief flag used for force draw custom shape
    bool myForceDrawCustomShape;

    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNECrossing(const GNECrossing&) = delete;

    /// @brief Invalidated assignment operator.
    GNECrossing& operator=(const GNECrossing&) = delete;
};


#endif

/****************************************************************************/

