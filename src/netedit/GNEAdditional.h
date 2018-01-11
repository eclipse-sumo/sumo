/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAdditional.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2016
/// @version $Id$
///
// A abstract class for representation of additional elements
/****************************************************************************/
#ifndef GNEAdditional_h
#define GNEAdditional_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIPropertySchemeStorage.h>

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditionalDialog;
class GNEEdge;
class GNELane;
class GNENet;
class GNEViewNet;
class GUIGLObjectPopupMenu;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAdditional
 * @brief An Element which don't belongs to GNENet but has influency in the simulation
 */
class GNEAdditional : public GUIGlObject, public GNEAttributeCarrier {

public:
    /**@brief Constructor
     * @param[in] id Gl-id of the additional element (Must be unique)
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
     * @param[in] icon GUIIcon associated to the additional
     * @param[in] movable Flag to indicate if this additional is movable
     */
    GNEAdditional(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, bool movable);

    /**@brief Constructor used by Additionals that have another additional sparent
    * @param[in] id Gl-id of the additional element (Must be unique)
    * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
    * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
    * @param[in] icon GUIIcon associated to the additional
    * @param[in] movable Flag to indicate if this additional is movable
    * @param[in] additionalParent pointer to additional parent
    */
    GNEAdditional(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, bool movable, GNEAdditional* additionalParent);

    /**@brief Constructor used by Additionals that have Edge childs
    * @param[in] id Gl-id of the additional element (Must be unique)
    * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
    * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
    * @param[in] icon GUIIcon associated to the additional
    * @param[in] movable Flag to indicate if this additional is movable
    * @param[in] edgeChilds vector of edge childs
    */
    GNEAdditional(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, bool movable, std::vector<GNEEdge*> edgeChilds);

    /**@brief Constructor used by Additionals that have lane childs
    * @param[in] id Gl-id of the additional element (Must be unique)
    * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
    * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
    * @param[in] icon GUIIcon associated to the additional
    * @param[in] movable Flag to indicate if this additional is movable
    * @param[in] laneChilds vector of lane childs
    */
    GNEAdditional(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, bool movable, std::vector<GNELane*> laneChilds);

    /// @brief Destructor
    ~GNEAdditional();

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    virtual void writeAdditional(OutputDevice& device) const = 0;

    /**@brief open Additional Dialog
     * @note: if additional needs an additional dialog, this function has to be implemented in childrens (see GNERerouter and GNEVariableSpeedSign)
     * @throw invalid argument if additional doesn't have an additional Dialog
     */
    virtual void openAdditionalDialog();

    /// @name Functions related with geometry of element
    /// @{
    /**@brief change the position of the element geometry without saving in undoList
    * @param[in] oldPos position before start movement
    * @param[in] offset movement offset regardings to oldPos
    */
    virtual void moveGeometry(const Position& oldPos, const Position& offset) = 0;

    /**@brief commit geometry changes in the attributes of an element after use of moveGeometry(...)
    * @param[in] oldPos the old position of additional
    * @param[in] undoList The undoList on which to register changes
    */
    virtual void commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) = 0;

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of additional in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @brief Returns a pointer to GNEViewNet in which additional element is located
    GNEViewNet* getViewNet() const;

    /// @brief Returns additional element's shape
    PositionVector getShape() const;

    /// @brief Check if additional item is currently blocked (i.e. cannot be moved with mouse)
    bool isAdditionalBlocked() const;

    // @brief Check if additional item is selected
    bool isAdditionalSelected() const;

    // @brief get additional parent
    GNEAdditional* getAdditionalParent() const;

    /// @name members and functions relative to  childs of this additional
    /// @{

    /// @brief add additional child to this additional
    void addAdditionalChild(GNEAdditional* additional);

    /// @brief remove additional child from this additional
    void removeAdditionalChild(GNEAdditional* additional);

    /// @brief return vector of additionals that have as Parent this edge (For example, Calibrators)
    const std::vector<GNEAdditional*>& getAdditionalChilds() const;

    /// @brief add edge child
    void addEdgeChild(GNEEdge* edge);

    /// @brief remove edge child
    void removeEdgeChild(GNEEdge* edge);

    /// @brief get edge chidls
    const std::vector<GNEEdge*>& getEdgeChilds() const;

    /// @brief add lane child
    void addLaneChild(GNELane* lane);

    /// @brief remove lane child
    void removeLaneChild(GNELane* lane);

    /// @brief get lanes of VSS
    const std::vector<GNELane*>& getLaneChilds() const;

    /// @}

    /// @name inherited from GUIGlObject
    /// @{
    /// @brief Returns the name (ID) of the parent object
    virtual const std::string& getParentName() const = 0;

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     */
    Boundary getCenteringBoundary() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /**@brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /**@brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;
    /// @}

    /** @brief check if a route is valid
     * @param[in] edges vector with the route's edges
     * @param[in] report enable or disable writting warnings if route isn't valid
     */
    static bool isRouteValid(const std::vector<GNEEdge*>& edges, bool report);

protected:
    /// @brief The GNEViewNet this additional element belongs
    GNEViewNet* myViewNet;

    /**@brief The shape of the additional element
     * @note must be configured in updateGeometry()
     */
    PositionVector myShape;

    /// @name computed only once (for performance) in updateGeometry()
    /// @{
    /// The rotations of the shape parts
    std::vector<double> myShapeRotations;

    /// The lengths of the shape parts
    std::vector<double> myShapeLengths;
    /// @}

    /// @brief boolean to check if additional element is blocked (i.e. cannot be moved with mouse)
    bool myBlocked;

    /// @brief pointer to Addititional parent
    GNEAdditional* myAdditionalParent;

    /// @brief vector with the Additional childs
    std::vector<GNEAdditional*> myAdditionalChilds;

    /// @brief vector with the edge childs of this additional
    std::vector<GNEEdge*> myEdgeChilds;

    /// @brief vector with the lane childs of this additional
    std::vector<GNELane*> myLaneChilds;

    /// @name members and functions relative to block icon
    /// @{
    /// @brief set Rotation of block Icon (must be called in updateGeometry() function)
    void setBlockIconRotation(GNELane* additionalLane = NULL);

    /// @brief draw lock icon
    void drawLockIcon(double size = 0.5) const;

    /// @brief position of the block icon
    Position myBlockIconPosition;

    /// @brief The offSet of the block icon
    Position myBlockIconOffset;

    /// @brief The rotation of the block icon
    double myBlockIconRotation;
    /// @}

    /// @name Functions relative to change values in setAttribute(...)
    /// @{

    /// @brief returns Additional ID
    const std::string& getAdditionalID() const;

    /// @brief check if a new additional ID is valid
    bool isValidAdditionalID(const std::string& newID) const;

    /**@brief change ID of additional
    * @throw exception if exist already an additional whith the same ID
    * @throw exception if ID isn't valid
    */
    void changeAdditionalID(const std::string& newID);

    /**@brief change edge of additional
    * @throw exception if oldEdge doesn't belong to an edge
    * @throw exception if edge with ID newEdgeID doesn't exist
    */
    GNEEdge* changeEdge(GNEEdge* oldEdge, const std::string& newEdgeID);

    /**@brief change lane of additional
    * @throw exception if oldLane doesn't belong to an edge
    * @throw exception if lane with ID newLaneID doesn't exist
    */
    GNELane* changeLane(GNELane* oldLane, const std::string& newLaneID);

    /**@brief change additional parent of additional
    * @throw exception if this additional doesn't have previously a defined Additional parent
    * @throw exception if additional with ID newAdditionalParentID doesn't exist
    */
    void changeAdditionalParent(const std::string& newAdditionalParentID);

    /// @}

    /// @name members and functions relative to connections between Additionals and their childs
    /// @{

    /// @brief update Connection's geometry
    void updateChildConnections();

    /// @brief draw connections between Parent and childrens
    void drawChildConnections() const;

    /// @brief position and rotation of every simbol over lane
    std::vector<std::pair<Position, double> > mySymbolsPositionAndRotation;
    /// @}

private:
    /// @brief flag to check if this additional is movable
    bool myMovable;

    /// @brief Matrix with the Vertex's positions of connections between parents an their childs
    std::vector<std::vector<Position> > myChildConnectionPositions;

    /// @brief Invalidated copy constructor.
    GNEAdditional(const GNEAdditional&) = delete;

    /// @brief Invalidated assignment operator.
    GNEAdditional& operator=(const GNEAdditional&) = delete;
};

#endif
