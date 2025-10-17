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
/// @file    GNEPerson.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// Representation of persons in netedit
/****************************************************************************/
#pragma once
#include <config.h>
#include <netedit/elements/GNEContour.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>

#include "GNEDemandElement.h"
#include "GNEDemandElementFlow.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEMoveElementPlanParent;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEPerson : public GNEDemandElement, public GNEDemandElementFlow {

public:
    /// @brief class used in GUIGLObjectPopupMenu for person transformations
    class GNEPersonPopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(GNEPersonPopupMenu)

    public:
        /** @brief Constructor
         * @param[in] person GNEPerson to be transformed
         * @param[in] app The main window for instantiation of other windows
         * @param[in] parent The parent view for changing it
         */
        GNEPersonPopupMenu(GNEPerson* person, GUIMainWindow& app, GUISUMOAbstractView& parent);

        /// @brief Destructor
        ~GNEPersonPopupMenu();

        /// @brief Called to transform the current person to another person type
        long onCmdTransform(FXObject* obj, FXSelector, void*);

    protected:
        FOX_CONSTRUCTOR(GNEPersonPopupMenu)

    private:
        /// @brief current person
        GNEPerson* myPerson;

        /// @brief menu command for transform to person
        FXMenuCommand* myTransformToPerson;

        /// @brief menu command for transform to personFlow
        FXMenuCommand* myTransformToPersonFlow;
    };

    /// @brief class used in GUIGLObjectPopupMenu for single person transformations
    class GNESelectedPersonsPopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(GNESelectedPersonsPopupMenu)

    public:
        /** @brief Constructor
         * @param[in] person clicked GNEPerson
         * @param[in] selectedPerson vector with selected GNEPerson
         * @param[in] app The main window for instantiation of other windows
         * @param[in] parent The parent view for changing it
         */
        GNESelectedPersonsPopupMenu(GNEPerson* person, const std::vector<GNEPerson*>& selectedPerson, GUIMainWindow& app, GUISUMOAbstractView& parent);

        /// @brief Destructor
        ~GNESelectedPersonsPopupMenu();

        /// @brief Called to transform the current person to another person type
        long onCmdTransform(FXObject* obj, FXSelector, void*);

    protected:
        FOX_CONSTRUCTOR(GNESelectedPersonsPopupMenu)

    private:
        /// @brief tag of clicked person
        SumoXMLTag myPersonTag;

        /// @brief current selected persons
        std::vector<GNEPerson*> mySelectedPersons;

        /// @brief menu command for transform to person
        FXMenuCommand* myTransformToPerson;

        /// @brief menu command for transform to personFlow
        FXMenuCommand* myTransformToPersonFlow;
    };

    /// @brief default constructor
    GNEPerson(SumoXMLTag tag, GNENet* net);

    /// @brief constructor for persons
    GNEPerson(SumoXMLTag tag, GNENet* net, const std::string& filename, GNEDemandElement* pType, const SUMOVehicleParameter& personparameters);

    /// @brief destructor
    ~GNEPerson();

    /// @brief methods to retrieve the elements linked to this person
    /// @{

    /// @brief get GNEMoveElement associated with this person
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this person
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this person (constant)
    const Parameterised* getParameters() const override;

    /// @}

    /**@brief write demand element element into a xml file
     * @param[in] device device in which write parameters of demand element element
     */
    void writeDemandElement(OutputDevice& device) const;

    /// @brief check if current demand element is valid to be written into XML (by default true, can be reimplemented in children)
    Problem isDemandElementValid() const;

    /// @brief return a string with the current demand element problem (by default empty, can be reimplemented in children)
    std::string getDemandElementProblem() const;

    /// @brief fix demand element problem (by default throw an exception, has to be reimplemented in children)
    void fixDemandElementProblem();

    /// @name members and functions relative to elements common to all demand elements
    /// @{
    /// @brief obtain VClass related with this demand element
    SUMOVehicleClass getVClass() const;

    /// @brief get color
    const RGBColor& getColor() const;

    /// @}

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    void updateGeometry() override;

    /// @brief Returns position of demand element in view
    Position getPositionInView() const;
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

    /**@brief Returns the name of the parent object
     * @return This object's parent id
     */
    std::string getParentName() const;

    /// @brief return exaggeration associated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const;

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     * @return The boundary the object is within
     */
    Boundary getCenteringBoundary() const;

    /// @brief split geometry
    void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList);

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    /// @}

    /// @name inherited from GNEPathElement
    /// @{

    /// @brief compute pathElement
    void computePathElement();

    /**@brief Draws partial object over lane
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment lane segment
     * @param[in] offsetFront front offset
     */
    void drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const;

    /**@brief Draws partial object over junction
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment junction segment
     * @param[in] offsetFront front offset
     */
    void drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const;

    /// @brief get first path lane
    GNELane* getFirstPathLane() const;

    /// @brief get last path lane
    GNELane* getLastPathLane() const;
    /// @}

    /// @brief inherited from GNEAttributeCarrier
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

    /* @brief method for setting the attribute and letting the object perform demand element changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     * @param[in] net optionally the GNENet to inform about gui updates
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) override;

    /* @brief method for setting the attribute and letting the object perform demand element changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    bool isValid(SumoXMLAttr key, const std::string& value) override;

    /* @brief method for enable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    void enableAttribute(SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for disable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    void disableAttribute(SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const override;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const override;
    /// @}

protected:
    /// @brief move element plan parent
    GNEMoveElementPlanParent* myMoveElementPlanParent = nullptr;

    /// @brief variable used for contours
    GNEContour myPersonContour;

    /// @brief get drawing color
    RGBColor getDrawingColor(const GUIVisualizationSettings& s) const;

private:
    // @brief struct used for calculating person plan geometry segments
    struct personPlanSegment {
        /// @brief parameter constructor
        personPlanSegment(GNEDemandElement* _personPlan);

        /// @brief person plan
        const GNEDemandElement* personPlan;

        /// @brief edge
        GNEEdge* edge;

        /// @brief busStops placed in this segment
        std::vector<GNEAdditional*> busStops;

        /// @brief stops placed in this segment
        std::vector<GNEDemandElement*> stops;

        /// @brief arrival position
        double arrivalPos;

    private:
        /// @brief constructor
        personPlanSegment();
    };

    /// @brief method for setting the attribute and nothing else
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief method for enable or disable the attribute and nothing else (used in GNEChange_ToggleAttribute)
    void toggleAttribute(SumoXMLAttr key, const bool value);

    /// @brief Invalidated copy constructor.
    GNEPerson(const GNEPerson&) = delete;

    /// @brief Invalidated assignment operator
    GNEPerson& operator=(const GNEPerson&) = delete;
};
