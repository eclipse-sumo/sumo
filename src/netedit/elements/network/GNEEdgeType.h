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
/// @file    GNEEdgeType.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2020
///
/// A SUMO edge type file assigns default values for certain attributes to types of roads.
/****************************************************************************/
#pragma once
#include <config.h>
#include "GNENetworkElement.h"

#include <netbuild/NBTypeCont.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNELaneType;
class GNEEdgeTemplate;
class GNECreateEdgeFrame;


// ===========================================================================
// class definitions
// ===========================================================================
class GNEEdgeType : public GNENetworkElement, public Parameterised, public NBTypeCont::EdgeTypeDefinition {
public:
    /// @brief GNECreateEdgeFrame need access to setAttribute
    friend class GNECreateEdgeFrame;

    /**@brief Constructor for default edge (empty ID)
     * @param[in] createEdgeFrame Frame in which edgeType is created
     */
    GNEEdgeType(GNECreateEdgeFrame* createEdgeFrame);

    /// @brief copy constructor (this doesn't create new lanes)
    GNEEdgeType(const GNEEdgeType* edgeType);

    /**@brief Constructor
     * @param[in] net The net to inform about gui updates
     */
    GNEEdgeType(GNENet* net);

    /**@brief Constructor (parameters)
     * @param[in] net The net to inform about gui updates
     * @param[in] ID edge type ID
     * @param[in] edgeType original NBTypeCont::EdgeTypeDefinition
     * @param[in] spreadType original spreadType
     */
    GNEEdgeType(GNENet* net, const std::string& ID, const NBTypeCont::EdgeTypeDefinition* edgeType);

    /// @brief Destructor.
    ~GNEEdgeType();

    /// @brief methods to retrieve the elements linked to this edgeType
    /// @{

    /// @brief get GNEMoveElement associated with this edgeType
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this edgeType
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this edgeType (constant)
    const Parameterised* getParameters() const override;

    /// @}

    /// @brief copy edge template
    void copyTemplate(const GNEEdgeTemplate* edgeTemplate);

    /// @brief get laneTypes
    const std::vector<GNELaneType*>& getLaneTypes() const;

    /// @brief get laneType index
    int getLaneTypeIndex(const GNELaneType* laneType) const;

    /// @brief add laneType
    void addLaneType(GNELaneType* laneType);

    /// @brief remove laneType
    void removeLaneType(const int index);

    /// @name Functions related with geometry of element
    /// @{

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

    /// @name Functions related with move elements
    /// @{
    /// @brief get move operation for the given shapeOffset (can be nullptr)
    GNEMoveOperation* getMoveOperation();

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);
    /// @}

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

    /* @brief method for checking if the key and their conrrespond attribute are valids
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

protected:
    /// @brief vector with laneTypes
    std::vector<GNELaneType*> myLaneTypes;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief invalidated copy constructor
    GNEEdgeType(const GNEEdgeType& s) = delete;

    /// @brief invalidated assignment operator
    GNEEdgeType& operator=(const GNEEdgeType& s) = delete;
};
