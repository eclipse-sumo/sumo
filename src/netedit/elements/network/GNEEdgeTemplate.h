/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEEdgeTemplate.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
///
// Template for edges
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEEdge;
class GNELaneTemplate;

// ===========================================================================
// class definitions
// ===========================================================================
class GNEEdgeTemplate : public GNEAttributeCarrier {

public:
    /// @brief Constructor
    GNEEdgeTemplate(const GNEEdge* edge);

    /// @brief Destructor.
    ~GNEEdgeTemplate();

    /// @brief get GNEHierarchicalElement associated with this AttributeCarrier
    GNEHierarchicalElement* getHierarchicalElement();

    /// @brief get vector with the lane templates of this edge
    const std::vector<GNELaneTemplate*>& getLaneTemplates() const;

    /// @brief update lane templates
    void updateLaneTemplates();

    /// @name Function related with graphics
    /// @{

    /// @brief get GUIGlObject associated with this AttributeCarrier
    GUIGlObject* getGUIGlObject();

    /// @brief get GUIGlObject associated with this AttributeCarrier (constant)
    const GUIGlObject* getGUIGlObject() const;

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @}

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw from contour (green)
    bool checkDrawFromContour() const;

    /// @brief check if draw from contour (magenta)
    bool checkDrawToContour() const;

    /// @brief check if draw related contour (cyan)
    bool checkDrawRelatedContour() const;

    /// @brief check if draw over contour (orange)
    bool checkDrawOverContour() const;

    /// @brief check if draw delete contour (pink/white)
    bool checkDrawDeleteContour() const;

    /// @brief check if draw select contour (blue)
    bool checkDrawSelectContour() const;

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const;

    /// @}

    /// @name Functions related with attributes
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in Position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    PositionVector getAttributePositionVector(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for check if new value for certain attribute is valid
     * @param[in] key The attribute key
     * @param[in] value The new value
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;

    /// @}

    /// @brief get parameters map
    const Parameterised::Map& getACParametersMap() const;

protected:
    /// @brief pointer to original edge
    const GNEEdge* myEdge;

    /// @brief vector with the lane templates of this edge
    std::vector<GNELaneTemplate*> myLaneTemplates;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief invalidated copy constructor
    GNEEdgeTemplate(const GNEEdgeTemplate& s) = delete;

    /// @brief invalidated assignment operator
    GNEEdgeTemplate& operator=(const GNEEdgeTemplate& s) = delete;
};
