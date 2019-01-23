/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDemandElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
/// @version $Id$
///
// A abstract class for demand elements
/****************************************************************************/
#ifndef GNEDemandElement_h
#define GNEDemandElement_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNEAttributeCarrier.h>
#include <utils/common/Parameterised.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;
class GNELane;
class GNEViewNet;
class GUIGLObjectPopupMenu;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEDemandElement
 * @brief An Element which don't belongs to GNENet but has influency in the simulation
 */
class GNEDemandElement : public GUIGlObject, public GNEAttributeCarrier, public Parameterised {

public:
    /**@brief Constructor
     * @param[in] id Gl-id of the demand element element (Must be unique)
     * @param[in] viewNet pointer to GNEViewNet of this demand element element belongs
     * @param[in] type GUIGlObjectType of demand element
     * @param[in] tag Type of xml tag that define the demand element element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
     */
    GNEDemandElement(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag);

    /**@brief Constructor used by DemandElements that have another demand element as parent
    * @param[in] singleDemandElementParent pointer to single demand element parent
    * @param[in] viewNet pointer to GNEViewNet of this demand element element belongs
    * @param[in] type GUIGlObjectType of demand element
    * @param[in] tag Type of xml tag that define the demand element element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
    */
    GNEDemandElement(GNEDemandElement* singleDemandElementParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag);

    /**@brief Constructor used by DemandElements that have two demand elements as parent
    * @param[in] demand elementParent pointer to first demand element parent
    * @param[in] demand elementParent pointer to second demand element parent
    * @param[in] viewNet pointer to GNEViewNet of this demand element element belongs
    * @param[in] type GUIGlObjectType of demand element
    * @param[in] tag Type of xml tag that define the demand element element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
    */
    GNEDemandElement(GNEDemandElement* firstDemandElementParent, GNEDemandElement* secondDemandElementParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag);

    /**@brief Constructor used by DemandElements that have Edge childs
    * @param[in] id Gl-id of the demand element element (Must be unique)
    * @param[in] viewNet pointer to GNEViewNet of this demand element element belongs
    * @param[in] type GUIGlObjectType of demand element
    * @param[in] tag Type of xml tag that define the demand element element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
    * @param[in] block movement enable or disable demand element movement
    * @param[in] edgeChilds vector of edge childs
    */
    GNEDemandElement(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::vector<GNEEdge*> edgeChilds);

    /**@brief Constructor used by DemandElements that have lane childs
    * @param[in] id Gl-id of the demand element element (Must be unique)
    * @param[in] viewNet pointer to GNEViewNet of this demand element element belongs
    * @param[in] type GUIGlObjectType of demand element
    * @param[in] tag Type of xml tag that define the demand element element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
    * @param[in] laneChilds vector of lane childs
    */
    GNEDemandElement(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::vector<GNELane*> laneChilds);

    /// @brief Destructor
    ~GNEDemandElement();

    /// @name members and functions relative to write demand elements into XML
    /// @{
    /**@brief writte demand element element into a xml file
     * @param[in] device device in which write parameters of demand element element
     */
    void writeDemandElement(OutputDevice& device) const;

    /// @brief check if current demand element is valid to be writed into XML (by default true, can be reimplemented in childs)
    virtual bool isDemandElementValid() const;

    /// @brief return a string with the current demand element problem (by default empty, can be reimplemented in childs)
    virtual std::string getDemandElementProblem() const;

    /// @brief fix demand element problem (by default throw an exception, has to be reimplemented in childs)
    virtual void fixDemandElementProblem();
    /// @}

    /**@brief open DemandElement Dialog
     * @note: if demand element needs an demand element dialog, this function has to be implemented in childrens (see GNERerouter and GNEVariableSpeedSign)
     * @throw invalid argument if demand element doesn't have an demand element Dialog
     */
    virtual void openDemandElementDialog();

    /// @name Functions related with geometry of element
    /// @{
    /// @brief begin movement (used when user click over demand element to start a movement, to avoid problems with problems with GL Tree)
    void startGeometryMoving();

    /// @brief begin movement (used when user click over demand element to start a movement, to avoid problems with problems with GL Tree)
    void endGeometryMoving();

    /**@brief change the position of the element geometry without saving in undoList
     * @param[in] offset Position used for calculate new position of geometry without updating RTree
     */
    virtual void moveGeometry(const Position& offset) = 0;

    /**@brief commit geometry changes in the attributes of an element after use of moveGeometry(...)
    * @param[in] undoList The undoList on which to register changes
    */
    virtual void commitGeometryMoving(GNEUndoList* undoList) = 0;

    /// @brief update pre-computed geometry information
    virtual void updateGeometry(bool updateGrid) = 0;

    /// @brief Returns position of demand element in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @brief Returns a pointer to GNEViewNet in which demand element element is located
    GNEViewNet* getViewNet() const;

    /// @brief Returns demand element element's shape
    PositionVector getShape() const;

    // @brief get first demand element parent
    GNEDemandElement* getFirstDemandElementParent() const;

    // @brief get second demand element parent
    GNEDemandElement* getSecondDemandElementParent() const;

    /// @brief gererate a new ID for an demand element child
    std::string generateDemandElementChildID(SumoXMLTag childTag);

    /// @name members and functions relative to demand element's childs
    /// @{

    /// @brief add demand element child to this demand element
    void addDemandElementChild(GNEDemandElement* demandElement);

    /// @brief remove demand element child from this demand element
    void removeDemandElementChild(GNEDemandElement* demandElement);

    /// @brief return vector of demand elements that have as Parent this edge (For example, Calibrators)
    const std::vector<GNEDemandElement*>& getDemandElementChilds() const;

    /// @brief sort childs (used by Rerouters and VSS)
    void sortDemandElementChilds();

    /// @brief check if childs are overlapped (Used by Rerouters)
    bool checkDemandElementChildsOverlapping() const;

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
    /// @brief select attribute carrier using GUIGlobalSelection
    virtual void selectAttributeCarrier(bool changeFlag = true) = 0;

    /// @brief unselect attribute carrier using GUIGlobalSelection
    virtual void unselectAttributeCarrier(bool changeFlag = true) = 0;

    /// @brief check if attribute carrier is selected
    bool isAttributeCarrierSelected() const;

    /// @brief check if attribute carrier must be drawn using selecting color.
    bool drawUsingSelectColor() const;

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /**@brief method for setting the attribute and letting the object perform demand element changes
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

    /// @brief get PopPup ID (Used in AC Hierarchy)
    virtual std::string getPopUpID() const = 0;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    virtual std::string getHierarchyName() const = 0;
    /// @}

    /// @name Functions related with Generic Paramters
    /// @{

    /// @brief return generic parameters in string format
    std::string getGenericParametersStr() const;

    /// @brief return generic parameters as vector of pairs format
    std::vector<std::pair<std::string, std::string> > getGenericParameters() const;

    /// @brief set generic parameters in string format
    void setGenericParametersStr(const std::string& value);

    /// @}

    /** @brief check if a route is valid
     * @param[in] edges vector with the route's edges
     * @param[in] report enable or disable writting warnings if route isn't valid
     */
    static bool isRouteValid(const std::vector<GNEEdge*>& edges, bool report);

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for stadistics)
    virtual void updateDemandElementParent();

protected:
    /// @brief struct for pack all variables related with geometry of elemement
    struct DemandElementGeometry {
        /// @brief constructor
        DemandElementGeometry();

        /// @brief reset geometry
        void clearGeometry();

        /// @brief calculate multi shape unified
        void calculateMultiShapeUnified();

        /// @brief calculate shape rotations and lenghts
        void calculateShapeRotationsAndLengths();

        /// @brief calculate multi shape rotations and lenghts
        void calculateMultiShapeRotationsAndLengths();

        /// @brief The shape of the demand element element
        PositionVector shape;

        /// @brief The multi-shape of the demand element element (used by certain demand elements)
        std::vector<PositionVector> multiShape;

        /// @brief The rotations of the single shape parts
        std::vector<double> shapeRotations;

        /// @brief The lengths of the single shape parts
        std::vector<double> shapeLengths;

        /// @brief The rotations of the multi-shape parts
        std::vector<std::vector<double> > multiShapeRotations;

        /// @brief The lengths of the multi-shape shape parts
        std::vector<std::vector<double> > multiShapeLengths;

        /// @brief multi shape unified
        PositionVector multiShapeUnified;
    };

    /// @brief struct for pack all variables related with demand element move
    struct DemandElementMove {
        /// @brief boundary used during moving of elements (to avoid insertion in RTREE
        Boundary movingGeometryBoundary;

        /// @brief value for saving first original position over lane before moving
        Position originalViewPosition;

        /// @brief value for saving first original position over lane before moving
        std::string firstOriginalLanePosition;

        /// @brief value for saving second original position over lane before moving
        std::string secondOriginalPosition;
    };

    /// @brief struct for pack all variables and functions relative to connections between DemandElements and their childs
    struct ChildConnections {
        /// @brief constructor
        ChildConnections(GNEDemandElement* demandElement);

        /// @brief update Connection's geometry
        void update();

        /// @brief draw connections between Parent and childrens
        void draw() const;

        /// @brief position and rotation of every symbol over lane
        std::vector<std::pair<Position, double> > symbolsPositionAndRotation;

        /// @brief Matrix with the Vertex's positions of connections between parents an their childs
        std::vector<PositionVector> connectionPositions;

    private:
        /// @brief pointer to demand element parent
        GNEDemandElement* myDemandElement;
    };

    /// @brief The GNEViewNet this demand element element belongs
    GNEViewNet* myViewNet;

    /// @brief geometry to be precomputed in updateGeometry(...)
    DemandElementGeometry myGeometry;

    /// @brief variable DemandElementMove
    DemandElementMove myMove;

    /// @brief pointer to first DemandElement parent
    GNEDemandElement* myFirstDemandElementParent;

    /// @brief pointer to second DemandElement parent
    GNEDemandElement* mySecondDemandElementParent;

    /// @brief vector with the DemandElement childs
    std::vector<GNEDemandElement*> myDemandElementChilds;

    /// @brief vector with the edge childs of this demand element
    std::vector<GNEEdge*> myEdgeChilds;

    /// @brief vector with the lane childs of this demand element
    std::vector<GNELane*> myLaneChilds;

    /// @brief variable ChildConnections
    ChildConnections myChildConnections;

    /// @brief change all attributes of demand element with their default values (note: this cannot be undo)
    void setDefaultValues();

    /// @name Functions relative to change values in setAttribute(...)
    /// @{

    /// @brief returns DemandElement ID
    const std::string& getDemandElementID() const;

    /// @brief check if a new demand element ID is valid
    bool isValidDemandElementID(const std::string& newID) const;

    /// @brief check if a new detector ID is valid
    bool isValidDetectorID(const std::string& newID) const;

    /**@brief change ID of demand element
    * @throw exception if exist already an demand element whith the same ID
    * @throw exception if ID isn't valid
    */
    void changeDemandElementID(const std::string& newID);

    /**@brief change edge of demand element
    * @throw exception if oldEdge doesn't belong to an edge
    * @throw exception if edge with ID newEdgeID doesn't exist
    */
    GNEEdge* changeEdge(GNEEdge* oldEdge, const std::string& newEdgeID);

    /**@brief change lane of demand element
    * @throw exception if oldLane doesn't belong to an edge
    * @throw exception if lane with ID newLaneID doesn't exist
    */
    GNELane* changeLane(GNELane* oldLane, const std::string& newLaneID);

    /**@brief change first demand element parent of demand element
    * @throw exception if this demand element doesn't have previously a defined DemandElement parent
    * @throw exception if demand element with ID newDemandElementParentID doesn't exist
    */
    void changeFirstDemandElementParent(const std::string& newDemandElementParentID);

    /**@brief change second demand element parent of demand element
    * @throw exception if this demand element doesn't have previously a defined DemandElement parent
    * @throw exception if demand element with ID newDemandElementParentID doesn't exist
    */
    void changeSecondDemandElementParent(const std::string& newDemandElementParentID);
    /// @}

private:
    /**@brief check restriction with the number of childs
     * @throw ProcessError if itis called without be reimplemented in child class
     */
    virtual bool checkDemandElementChildRestriction() const;

    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEDemandElement(const GNEDemandElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElement& operator=(const GNEDemandElement&) = delete;
};


#endif

/****************************************************************************/

