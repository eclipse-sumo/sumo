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
/// @file    GNEDemandElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
///
// A abstract class for demand elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/GNEHierarchicalElement.h>
#include <netedit/elements/GNEPathElement.h>
#include <utils/gui/div/GUIGeometry.h>
#include <utils/gui/globjects/GUIGlObject.h>

// ===========================================================================
// class declarations
// ===========================================================================

class SUMOVehicleParameter;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDemandElement : public GNEAttributeCarrier, public GNEHierarchicalElement, public GUIGlObject, public GNEPathElement {

public:
    /// @brief friend declaration (needed for vTypes)
    friend class GNERouteHandler;
    friend class GNEDemandElementFlow;
    friend class GNEDemandElementPlan;

    /// @brief enum class for demandElement problems
    enum class Problem {
        OK,                         // There is no problem
        INVALID_ELEMENT,            // Element is invalid (for example, a route without edges)
        INVALID_PATH,               // Path (route, trip... ) is not valid (i.e is empty)
        DISCONNECTED_PLAN,          // Plan element (person, containers) is not connected with the previous or next plan
        INVALID_STOPPOSITION,       // StopPosition is invalid (only used in stops over edges or lanes
        STOP_DOWNSTREAM,            // Stops don't follow their route parent
        REPEATEDROUTE_DISCONNECTED, // Repeated route is disconnected
        NO_PLANS                    // Person or container doesn't have a plan
    };

    /**@brief Constructor
     * @param[in] id Gl-id of the demand element element (Must be unique)
     * @param[in] net pointer to GNEViewNet of this demand element element belongs
     * @param[in] filename file in which this AttributeCarrier is stored
     * @param[in] tag Type of xml tag that define the demand element element (SUMO_TAG_ROUTE, SUMO_TAG_VEHICLE, etc...)
     * @param[in] pathOptions path options
     */
    GNEDemandElement(const std::string& id, GNENet* net, const std::string& filename,
                     SumoXMLTag tag, const GNEPathElement::Options pathOptions);

    /**@brief Constructor
     * @param[in] demandElementParent pointer to parent demand element pointer (used to generate an ID)
     * @param[in] net pointer to GNEViewNet of this demand element element belongs
     * @param[in] tag Type of xml tag that define the demand element element (SUMO_TAG_ROUTE, SUMO_TAG_VEHICLE, etc...)
     * @param[in] pathOptions path options
     */
    GNEDemandElement(GNEDemandElement* demandElementParent, SumoXMLTag tag, const GNEPathElement::Options pathOptions);

    /// @brief Destructor
    virtual ~GNEDemandElement();

    /// @brief methods to retrieve the elements linked to this GNEDemandElement
    /// @{

    /// @brief get GNEHierarchicalElement associated with this GNEDemandElement
    GNEHierarchicalElement* getHierarchicalElement() override;

    /// @brief get GUIGlObject associated with this GNEDemandElement
    GUIGlObject* getGUIGlObject() override;

    /// @brief get GUIGlObject associated with this GNEDemandElement (constant)
    const GUIGlObject* getGUIGlObject() const override;

    /// @}

    /// @brief get demand element geometry (stacked)
    const GUIGeometry& getDemandElementGeometry();

    /// @brief get previous child demand element to the given demand element
    GNEDemandElement* getPreviousChildDemandElement(const GNEDemandElement* demandElement) const;

    /// @brief get next child demand element to the given demand element
    GNEDemandElement* getNextChildDemandElement(const GNEDemandElement* demandElement) const;

    /// @brief update element stacked geometry (stacked)
    void updateDemandElementGeometry(const GNELane* lane, const double posOverLane);

    /// @brief update stack label
    void updateDemandElementStackLabel(const int stack);

    /// @brief update element spread geometry
    void updateDemandElementSpreadGeometry(const GNELane* lane, const double posOverLane);

    /// @name members and functions relative to elements common to all demand elements
    /// @{

    /// @brief obtain VClass related with this demand element
    virtual SUMOVehicleClass getVClass() const = 0;

    /// @brief get color
    virtual const RGBColor& getColor() const = 0;

    /// @}

    /// @name members and functions relative to write demand elements into XML
    /// @{

    /**@brief write demand element element into a xml file
     * @param[in] device device in which write parameters of demand element element
     */
    virtual void writeDemandElement(OutputDevice& device) const = 0;

    /// @brief check if current demand element is valid to be written into XML (by default true, can be reimplemented in children)
    virtual Problem isDemandElementValid() const = 0;

    /// @brief return a string with the current demand element problem (by default empty, can be reimplemented in children)
    virtual std::string getDemandElementProblem() const = 0;

    /// @brief fix demand element problem (by default throw an exception, has to be reimplemented in children)
    virtual void fixDemandElementProblem() = 0;

    /// @}

    /**@brief open DemandElement Dialog
     * @note: if demand element needs an demand element dialog, this function has to be implemented in childrens (see GNERerouter and GNEVariableSpeedSign)
     * @throw invalid argument if demand element doesn't have an demand element Dialog
     */
    virtual void openDemandElementDialog();

    /// @name Functions related with geometry of element
    /// @{

    /// @brief Returns position of demand element in view
    virtual Position getPositionInView() const = 0;

    /// @brief split geometry
    virtual void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) = 0;

    /// @brief get demand element geometry
    const GUIGeometry& getDemandElementGeometry() const;

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

    /// @name inherited from GUIGlObject
    /// @{

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    virtual GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @brief check if element is locked
    bool isGLObjectLocked() const;

    /// @brief mark element as front element
    void markAsFrontElement();

    /// @brief delete element
    void deleteGLObject();

    /// @brief select element
    void selectGLObject();

    /// @brief update GLObject (geometry, ID, etc.)
    void updateGLObject();

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{

    /* @brief method for getting the Attribute of an XML key in position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    Position getAttributePosition(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in positionVector format
     * @param[in] key The attribute key
     * @return positionVector with the value associated to key
     */
    PositionVector getAttributePositionVector(SumoXMLAttr key) const override;

    /// @}

    /// @name inherited from GNEPathElement
    /// @{

    /// @brief check if path element is selected
    bool isPathElementSelected() const;

    /// @}

    /// @brief get invalid stops
    std::vector<GNEDemandElement*> getInvalidStops() const;

protected:
    /// @brief demand element geometry (also called "stacked geometry")
    GUIGeometry myDemandElementGeometry;

    /// @brief demand element spread geometry (Only used by vehicles and pedestrians)
    GUIGeometry mySpreadGeometry;

    /// @brief stacked label number
    int myStackedLabelNumber;

    /// @brief check if a new demand element ID is valid
    bool isValidDemandElementID(const std::string& value) const;

    /// @brief check if a new demand element ID is valid
    bool isValidDemandElementID(const std::vector<SumoXMLTag>& tags, const std::string& value) const;

    /// @brief set demand element id
    void setDemandElementID(const std::string& newID);

    /// @brief get type parent (needed because first parent can be either type or typeDistribution)
    GNEDemandElement* getTypeParent() const;

    /// @brief get route parent (always the second parent demand element)
    GNEDemandElement* getRouteParent() const;

    /// @brief draw line between junctions
    void drawJunctionLine(const GNEDemandElement* element) const;

    /// @brief draw stack label
    void drawStackLabel(const int number, const std::string& element, const Position& position, const double rotation,
                        const double width, const double length, const double exaggeration) const;

    /// @name replace parent elements
    /// @{

    /// @brief all edges
    void replaceParentEdges(const std::string& value);

    /// @brief replace the first parent lane
    void replaceFirstParentLane(const std::string& value);

    /// @brief replace the first parent junction
    void replaceFirstParentJunction(const std::string& value);

    /// @brief replace the last parent junction
    void replaceLastParentJunction(const std::string& value);

    /// @brief replace the first parent edge
    void replaceFirstParentEdge(const std::string& value);

    /// @brief replace the last parent edge
    void replaceLastParentEdge(const std::string& value);

    /// @brief replace the first parent additional
    void replaceFirstParentAdditional(SumoXMLTag tag, const std::string& value);

    /// @brief replace the last parent additional
    void replaceLastParentAdditional(SumoXMLTag tag, const std::string& value);

    /// @brief replace demand element parent
    void replaceDemandElementParent(SumoXMLTag tag, const std::string& value, const int parentIndex);

    /// @}

    /// @brief auxiliar struct used for calculate pathStopIndex
    struct EdgeStopIndex {

        /// @brief constructor
        EdgeStopIndex(GNEEdge* edge_, GNEDemandElement* stop) :
            edge(edge_),
            stops({stop}) {}

        /// @brief edge (obtained from segment)
        const GNEEdge* edge = nullptr;

        /// @brief list of stops placed in the edge
        std::vector<GNEDemandElement*> stops;

        /// @brief stopIndex (-1 menans out of route)
        int stopIndex = -1;

    private:
        /// @brief default constructor (disabled)
        EdgeStopIndex() {}
    };

    /// @brief get edgeStopIndex
    std::vector<EdgeStopIndex> getEdgeStopIndex() const;

    /// @brief get color by scheme (used by vehicles, persons and containers)
    RGBColor getColorByScheme(const GUIColorer& c, const SUMOVehicleParameter* parameters) const;

    /// @brief build menu command route length
    void buildMenuCommandRouteLength(GUIGLObjectPopupMenu* ret) const;

    /// @brief build menu command route length
    void buildMenuAddReverse(GUIGLObjectPopupMenu* ret) const;

private:
    /**@brief check restriction with the number of children
     * @throw ProcessError if itis called without be reimplemented in child class
     */
    virtual bool checkChildDemandElementRestriction() const;

    /// @brief Invalidated copy constructor.
    GNEDemandElement(const GNEDemandElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElement& operator=(const GNEDemandElement&) = delete;
};
