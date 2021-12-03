/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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

    /// @brief copy edge template
    void copyTemplate(const GNEEdgeTemplate *edgeTemplate);

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
    void updateGeometry();

    /// @brief Returns position of hierarchical element in view
    Position getPositionInView() const;
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

    /// @brief return exaggeration asociated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const;

    /// @brief update centering boundary (implies change in RTREE)
    void updateCenteringBoundary(const bool updateGrid);

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

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /* @brief method for check if the value for certain attribute is computed (for example, due a network recomputing)
     * @param[in] key The attribute key
     */
    bool isAttributeComputed(SumoXMLAttr key) const;

    /// @}

    /// @brief get parameters map
    const std::map<std::string, std::string>& getACParametersMap() const;

protected:
    /// @brief vector with laneTypes
    std::vector<GNELaneType*> myLaneTypes;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief invalidated copy constructor
    GNEEdgeType(const GNEEdgeType& s) = delete;

    /// @brief invalidated assignment operator
    GNEEdgeType& operator=(const GNEEdgeType& s) = delete;
};
