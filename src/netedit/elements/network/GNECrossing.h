/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNECrossing.h
/// @author  Jakob Erdmann
/// @date    June 2011
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
/****************************************************************************/
#pragma once
#include <config.h>
#include "GNENetworkElement.h"
#include <netbuild/NBNode.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;
class GNEJunction;
class GNEMoveElementCrossing;
class GUIGLObjectPopupMenu;
class PositionVector;

// ===========================================================================
// class definitions
// ===========================================================================


class GNECrossing : public GNENetworkElement {

public:
    /// @brief default constructor
    GNECrossing(GNENet* net);

    /**@brief Constructor
     * @param[in] junction GNEJunction in which this crossing is placed
     * @param[in] crossing Node::Crossing
     */
    GNECrossing(GNEJunction* junction, std::vector<NBEdge*> edges);

    /// @brief Destructor
    ~GNECrossing();

    /// @brief methods to retrieve the elements linked to this crossing
    /// @{

    /// @brief get GNEMoveElement associated with this crossing
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this crossing
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this crossing (constant)
    const Parameterised* getParameters() const override;

    /// @}

    /// @brief check if current network element is valid to be written into XML
    bool isNetworkElementValid() const;

    /// @brief return a string with the current network element problem
    std::string getNetworkElementProblem() const;

    /// @name Functions related with geometry of element
    /// @{

    /// @brief get Crossing shape
    const PositionVector& getCrossingShape() const;

    /// @brief update pre-computed geometry information
    void updateGeometry() override;

    /// @brief Returns position of hierarchical element in view
    Position getPositionInView() const;

    /// @}

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw from contour (green)
    bool checkDrawFromContour() const override;

    /// @brief check if draw from contour (magenta)
    bool checkDrawToContour() const override;

    /// @brief check if draw related contour (cyan)
    bool checkDrawRelatedContour() const override;

    /// @brief check if draw over contour (orange)
    bool checkDrawOverContour() const override;

    /// @brief check if draw delete contour (pink/white)
    bool checkDrawDeleteContour() const override;

    /// @brief check if draw delete contour small (pink/white)
    bool checkDrawDeleteContourSmall() const override;

    /// @brief check if draw select contour (blue)
    bool checkDrawSelectContour() const override;

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const override;

    /// @}

    /// @brief get crossingEdges
    const std::vector<NBEdge*>& getCrossingEdges() const;

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

    /// @brief Returns the boundary to which the view shall be centered in order to show the object
    Boundary getCenteringBoundary() const;

    /// @brief update centering boundary (implies change in RTREE)
    void updateCenteringBoundary(const bool updateGrid);

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    /// @brief delete element
    void deleteGLObject();

    /// @brief update GLObject (geometry, ID, etc.)
    void updateGLObject();

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in double format
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getAttributeDouble(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    Position getAttributePosition(SumoXMLAttr key) const override;

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
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) override;

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value) override;

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /// @}

    /// @brief return true if a edge belongs to crossing's edges
    bool checkEdgeBelong(GNEEdge* edges) const;

    /// @brief return true if a edge of a vector of edges belongs to crossing's edges
    bool checkEdgeBelong(const std::vector<GNEEdge*>& edges) const;

protected:
    /// @brief move element crossing
    GNEMoveElementCrossing* myMoveElementCrossing = nullptr;

    /// @brief Crossing Edges (It works as ID because a junction can only ONE Crossing with the same edges)
    std::vector<NBEdge*> myCrossingEdges;

    /// @brief crossing geometry
    GUIGeometry myCrossingGeometry;

    /// @brief template NBCrossing
    NBNode::Crossing* myTemplateNBCrossing;

private:
    /// @brief check if draw crossing
    bool checkDrawCrossing(const GUIVisualizationSettings& s) const;

    /// @brief draw crossing
    void drawCrossing(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                      const NBNode::Crossing* crossing, const double width, const double exaggeration) const;

    /// @brief get crossing color
    RGBColor getCrossingColor(const GUIVisualizationSettings& s, const NBNode::Crossing* crossing) const;

    /// @brief draw crossing with hight detail
    void drawCrossingDetailed(const double width, const double exaggeration) const;

    /// @brief calculate contour
    void calculateCrossingContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                  const double width, const double exaggeration) const;

    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief draw TLS Link Number
    void drawTLSLinkNo(const GUIVisualizationSettings& s, const NBNode::Crossing* crossing) const;

    /// @brief Invalidated copy constructor.
    GNECrossing(const GNECrossing&) = delete;

    /// @brief Invalidated assignment operator.
    GNECrossing& operator=(const GNECrossing&) = delete;
};
