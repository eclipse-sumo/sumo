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
 * @class GNEAdditional
 * @brief An Element which don't belongs to GNENet but has influency in the simulation
 */
class GNEAdditional : public GUIGlObject, public GNEAttributeCarrier, public Parameterised {

public:
    /**@brief Constructor
     * @param[in] id Gl-id of the additional element (Must be unique)
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] type GUIGlObjectType of additional
     * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
     * @param[in] name Additional name
     * @param[in] block movement enable or disable additional movement
     */
    GNEAdditional(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement);

    /**@brief Constructor used by Additionals that have another additional as parent
    * @param[in] singleAdditionalParent pointer to single additional parent
    * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
    * @param[in] type GUIGlObjectType of additional
    * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
    * @param[in] name Additional name
    * @param[in] block movement enable or disable additional movement
    */
    GNEAdditional(GNEAdditional* singleAdditionalParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement);

    /**@brief Constructor used by Additionals that have two additionals as parent
    * @param[in] additionalParent pointer to first additional parent
    * @param[in] additionalParent pointer to second additional parent
    * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
    * @param[in] type GUIGlObjectType of additional
    * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
    * @param[in] name Additional name
    * @param[in] block movement enable or disable additional movement
    */
    GNEAdditional(GNEAdditional* firstAdditionalParent, GNEAdditional* secondAdditionalParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement);

    /**@brief Constructor used by Additionals that have Edge childs
    * @param[in] id Gl-id of the additional element (Must be unique)
    * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
    * @param[in] type GUIGlObjectType of additional
    * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
    * @param[in] name Additional name
    * @param[in] block movement enable or disable additional movement
    * @param[in] edgeChilds vector of edge childs
    */
    GNEAdditional(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement, std::vector<GNEEdge*> edgeChilds);

    /**@brief Constructor used by Additionals that have lane childs
    * @param[in] id Gl-id of the additional element (Must be unique)
    * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
    * @param[in] type GUIGlObjectType of additional
    * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
    * @param[in] name Additional name
    * @param[in] block movement enable or disable additional movement
    * @param[in] laneChilds vector of lane childs
    */
    GNEAdditional(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement, std::vector<GNELane*> laneChilds);

    /// @brief Destructor
    ~GNEAdditional();

    /// @name members and functions relative to write additionals into XML
    /// @{
    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief check if current additional is valid to be writed into XML (by default true, can be reimplemented in childs)
    virtual bool isAdditionalValid() const;

    /// @brief return a string with the current additional problem (by default empty, can be reimplemented in childs) 
    virtual std::string getAdditionalProblem() const;

    /// @brief fix additional problem (by default throw an exception, has to be reimplemented in childs) 
    virtual void fixAdditionalProblem();
    /// @}

    /**@brief open Additional Dialog
     * @note: if additional needs an additional dialog, this function has to be implemented in childrens (see GNERerouter and GNEVariableSpeedSign)
     * @throw invalid argument if additional doesn't have an additional Dialog
     */
    virtual void openAdditionalDialog();

    /// @name Functions related with geometry of element
    /// @{
    /// @brief begin movement (used when user click over additional to start a movement, to avoid problems with problems with GL Tree)
    void startGeometryMoving();

    /// @brief begin movement (used when user click over additional to start a movement, to avoid problems with problems with GL Tree)
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

    /// @brief Returns position of additional in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @brief Returns a pointer to GNEViewNet in which additional element is located
    GNEViewNet* getViewNet() const;

    /// @brief Returns additional element's shape
    PositionVector getShape() const;

    /// @brief Check if additional item is currently blocked (i.e. cannot be moved with mouse)
    bool isAdditionalBlocked() const;

    // @brief get first additional parent
    GNEAdditional* getFirstAdditionalParent() const;

    // @brief get second additional parent
    GNEAdditional* getSecondAdditionalParent() const;

    /// @brief gererate a new ID for an additional child
    std::string generateAdditionalChildID(SumoXMLTag childTag);

    /// @name members and functions relative to additional's childs
    /// @{

    /// @brief add additional child to this additional
    void addAdditionalChild(GNEAdditional* additional);

    /// @brief remove additional child from this additional
    void removeAdditionalChild(GNEAdditional* additional);

    /// @brief return vector of additionals that have as Parent this edge (For example, Calibrators)
    const std::vector<GNEAdditional*>& getAdditionalChilds() const;

    /// @brief sort childs (used by Rerouters and VSS)
    void sortAdditionalChilds();

    /// @brief check if childs are overlapped (Used by Rerouters)
    bool checkAdditionalChildsOverlapping() const;

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
    void selectAttributeCarrier(bool changeFlag = true);

    /// @brief unselect attribute carrier using GUIGlobalSelection
    void unselectAttributeCarrier(bool changeFlag = true);

    /// @brief check if attribute carrier is selected
    bool isAttributeCarrierSelected() const;

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

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for stadistics)
    virtual void updateAdditionalParent();

protected:
    /// @brief struct for pack all variables related with geometry of elemement
    struct AdditionalGeometry {
        /// @brief constructor
        AdditionalGeometry();

        /// @brief reset geometry
        void clearGeometry();

        /// @brief calculate multi shape unified
        void calculateMultiShapeUnified();

        /// @brief calculate shape rotations and lenghts
        void calculateShapeRotationsAndLengths();

        /// @brief calculate multi shape rotations and lenghts
        void calculateMultiShapeRotationsAndLengths();

        /// @brief The shape of the additional element
        PositionVector shape;

        /// @brief The multi-shape of the additional element (used by certain additionals)
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

    /// @brief struct for pack all variables related with additional move
    struct AdditionalMove {
        /// @brief boundary used during moving of elements (to avoid insertion in RTREE
        Boundary movingGeometryBoundary;

        /// @brief value for saving first original position over lane before moving
        Position originalViewPosition;

        /// @brief value for saving first original position over lane before moving
        std::string firstOriginalLanePosition;

        /// @brief value for saving second original position over lane before moving
        std::string secondOriginalPosition;
    };

    /// @brief struct for pack all variables and functions related with Block Icon
    struct BlockIcon {
        /// @brief constructor
        BlockIcon(GNEAdditional *additional);

        /// @brief set Rotation of block Icon (must be called in updateGeometry(bool updateGrid) function)
        void setRotation(GNELane* additionalLane = nullptr);

        /// @brief draw lock icon
        void draw(double size = 0.5) const;

    private:
        /// @brief pointer to additional parent
        GNEAdditional *myAdditional;

    public:
        /// @brief position of the block icon
        Position position;

        /// @brief The offSet of the block icon
        Position offset;

        /// @brief The rotation of the block icon
        double rotation;
    };

    /// @brief struct for pack all variables and functions relative to connections between Additionals and their childs
    struct ChildConnections {
        /// @brief constructor
        ChildConnections(GNEAdditional *additional);

        /// @brief update Connection's geometry
        void update();

        /// @brief draw connections between Parent and childrens
        void draw() const;

        /// @brief position and rotation of every symbol over lane
        std::vector<std::pair<Position, double> > symbolsPositionAndRotation;

        /// @brief Matrix with the Vertex's positions of connections between parents an their childs
        std::vector<PositionVector> connectionPositions;

    private:
        /// @brief pointer to additional parent
        GNEAdditional *myAdditional;
    };

    /// @brief The GNEViewNet this additional element belongs
    GNEViewNet* myViewNet;

    /// @brief geometry to be precomputed in updateGeometry(...)
    AdditionalGeometry myGeometry;

    /// @brief variable AdditionalMove
    AdditionalMove myMove;

    /// @brief name of additional
    std::string myAdditionalName;

    /// @brief boolean to check if additional element is blocked (i.e. cannot be moved with mouse)
    bool myBlockMovement;

    /// @brief pointer to first Additional parent
    GNEAdditional* myFirstAdditionalParent;

    /// @brief pointer to second Additional parent
    GNEAdditional* mySecondAdditionalParent;

    /// @brief vector with the Additional childs
    std::vector<GNEAdditional*> myAdditionalChilds;

    /// @brief vector with the edge childs of this additional
    std::vector<GNEEdge*> myEdgeChilds;

    /// @brief vector with the lane childs of this additional
    std::vector<GNELane*> myLaneChilds;

    /// @brief variable BlockIcon
    BlockIcon myBlockIcon;

    /// @brief variable ChildConnections
    ChildConnections myChildConnections;

    /// @brief change all attributes of additional with their default values (note: this cannot be undo)
    void setDefaultValues();

    /// @name Functions relative to change values in setAttribute(...)
    /// @{

    /// @brief returns Additional ID
    const std::string& getAdditionalID() const;

    /// @brief check if a new additional ID is valid
    bool isValidAdditionalID(const std::string& newID) const;

    /// @brief check if a new detector ID is valid
    bool isValidDetectorID(const std::string& newID) const;

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

    /**@brief change first additional parent of additional
    * @throw exception if this additional doesn't have previously a defined Additional parent
    * @throw exception if additional with ID newAdditionalParentID doesn't exist
    */
    void changeFirstAdditionalParent(const std::string& newAdditionalParentID);

    /**@brief change second additional parent of additional
    * @throw exception if this additional doesn't have previously a defined Additional parent
    * @throw exception if additional with ID newAdditionalParentID doesn't exist
    */
    void changeSecondAdditionalParent(const std::string& newAdditionalParentID);
    /// @}

private:
    /**@brief check restriction with the number of childs
     * @throw ProcessError if itis called without be reimplemented in child class
     */
    virtual bool checkAdditionalChildRestriction() const;

    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEAdditional(const GNEAdditional&) = delete;

    /// @brief Invalidated assignment operator.
    GNEAdditional& operator=(const GNEAdditional&) = delete;
};

#endif
