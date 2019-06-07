/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPerson.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
/// @version $Id$
///
// Representation of persons in NETEDIT
/****************************************************************************/
#ifndef GNEPerson_h
#define GNEPerson_h


// ===========================================================================
// included modules
// ===========================================================================

#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>


#include "GNEDemandElement.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEPerson
 */
class GNEPerson : public GNEDemandElement, public SUMOVehicleParameter {

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
        GNEPersonPopupMenu(GNEPerson *person, GUIMainWindow& app, GUISUMOAbstractView& parent);

        /// @brief Destructor
        ~GNEPersonPopupMenu();

        /// @brief Called to transform the current person to another person type
        long onCmdTransform(FXObject* obj, FXSelector, void*);

    protected:
        /// @brief default constructor needed by FOX
        GNEPersonPopupMenu() { }

    private:
        /// @brief current person
        GNEPerson *myPerson;

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
        GNESelectedPersonsPopupMenu(GNEPerson *person, const std::vector<GNEPerson*> &selectedPerson, GUIMainWindow& app, GUISUMOAbstractView& parent);

        /// @brief Destructor
        ~GNESelectedPersonsPopupMenu();

        /// @brief Called to transform the current person to another person type
        long onCmdTransform(FXObject* obj, FXSelector, void*);

    protected:
        /// @brief default constructor needed by FOX
        GNESelectedPersonsPopupMenu() { }

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

    /// @brief default constructor for persons and routeFlows without embedded routes
    GNEPerson(SumoXMLTag tag, GNEViewNet* viewNet, const std::string& personID, GNEDemandElement* personType, GNEDemandElement* route);

    /// @brief parameter constructor for persons and routeFlows without embedded routes
    GNEPerson(GNEViewNet* viewNet, GNEDemandElement* personType, GNEDemandElement* route, const SUMOVehicleParameter& personParameters);

    /// @brief parameter constructor for persons and routeFlows with embedded routes (note: After creation create immediately a embedded route referencing this person)
    GNEPerson(GNEViewNet* viewNet, GNEDemandElement* personType, const SUMOVehicleParameter& personParameters);

    /// @brief default constructor for trips and Flows (note: Edges : from + via + to)
    GNEPerson(SumoXMLTag tag, GNEViewNet* viewNet, const std::string& personID, GNEDemandElement* personType, const std::vector<GNEEdge*>& edges);

    /// @brief parameter constructor for trips and Flows (note: Edges : from + via + to)
    GNEPerson(GNEViewNet* viewNet, GNEDemandElement* personType, const std::vector<GNEEdge*>& edges, const SUMOVehicleParameter& personParameters);

    /// @brief destructor
    ~GNEPerson();

    /// @brief obtain VClass related with this demand element
    SUMOVehicleClass getVClass() const;

    /**@brief get begin time of demand element
     * @note: used by demand elements of type "Person", and it has to be implemented as childs
     * @throw invalid argument if demand element doesn't has a begin time
     */
    std::string getBegin() const;

    /// @brief get color
    const RGBColor& getColor() const;

    /**@brief writte demand element element into a xml file
     * @param[in] device device in which write parameters of demand element element
     */
    void writeDemandElement(OutputDevice& device) const;

    /// @brief check if current demand element is valid to be writed into XML (by default true, can be reimplemented in childs)
    bool isDemandElementValid() const;

    /// @brief return a string with the current demand element problem (by default empty, can be reimplemented in childs)
    std::string getDemandElementProblem() const;

    /// @brief fix demand element problem (by default throw an exception, has to be reimplemented in childs)
    void fixDemandElementProblem();

    /// @name Functions related with geometry of element
    /// @{
    /// @brief start geometry movement
    void startGeometryMoving();

    /// @brief end geometry movement
    void endGeometryMoving();

    /**@brief change the position of the element geometry without saving in undoList
     * @param[in] offset Position used for calculate new position of geometry without updating RTree
     */
    void moveGeometry(const Position& offset);

    /**@brief commit geometry changes in the attributes of an element after use of moveGeometry(...)
     * @param[in] undoList The undoList on which to register changes
     */
    void commitGeometryMoving(GNEUndoList* undoList);

    /// @brief update pre-computed geometry information
    void updateGeometry();

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

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     * @return The boundary the object is within
     */
    Boundary getCenteringBoundary() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @brief inherited from GNEAttributeCarrier
    /// @{
    /// @brief select attribute carrier using GUIGlobalSelection
    void selectAttributeCarrier(bool changeFlag = true);

    /// @brief unselect attribute carrier using GUIGlobalSelection
    void unselectAttributeCarrier(bool changeFlag = true);

    /* @brief method for getting the Attribute of an XML key
    * @param[in] key The attribute key
    * @return string with the value associated to key
    */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform demand element changes
    * @param[in] key The attribute key
    * @param[in] value The new value
    * @param[in] undoList The undoList on which to register changes
    * @param[in] net optionally the GNENet to inform about gui updates
    */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for setting the attribute and letting the object perform demand element changes
    * @param[in] key The attribute key
    * @param[in] value The new value
    * @param[in] undoList The undoList on which to register changes
    */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeSet(SumoXMLAttr key) const;

    /* @brief method for check if certain attribute is set (used by ACs with disjoint attributes)
     * @param[in] key The attribute key
     * @return true if it's set, false in other case
     */
    bool isDisjointAttributeSet(const SumoXMLAttr attr) const;

    /* @brief method for set certain attribute is set (used by ACs with disjoint attributes)
     * @param[in] attr The attribute key
     * @param[in] undoList The undoList on which to register changes
     */
    void setDisjointAttribute(const SumoXMLAttr attr, GNEUndoList* undoList);

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

protected:
    /// @brief sets the color according to the currente settings
    void setColor(const GUIVisualizationSettings& s) const;

private:
    /// @brief method for setting the attribute and nothing else
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief method for setting the disjoint attribute and nothing else (used in GNEChange_Attribute)
    void setDisjointAttribute(const int newParameterSet);

    /// @brief Invalidated copy constructor.
    GNEPerson(const GNEPerson&) = delete;

    /// @brief Invalidated assignment operator
    GNEPerson& operator=(const GNEPerson&) = delete;
};

#endif
/****************************************************************************/
