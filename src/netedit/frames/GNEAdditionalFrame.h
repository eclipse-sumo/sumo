/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAdditionalFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
// The Widget for add additional elements
/****************************************************************************/
#ifndef GNEAdditionalFrame_h
#define GNEAdditionalFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEAdditionalFrame
 * The Widget for setting internal attributes of additional elements
 */
class GNEAdditionalFrame : public GNEFrame {

public:

    // ===========================================================================
    // class SelectorLaneParents
    // ===========================================================================

    class SelectorLaneParents : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::SelectorLaneParents)
    public:
        /// @brief constructor
        SelectorLaneParents(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~SelectorLaneParents();

        /// @brief show SelectorLaneParents modul
        void showSelectorLaneParentsModul();

        /// @brief hide SelectorLaneParents
        void hideSelectorLaneParentsModul();

        /// @brief start selection of consecutive lanes
        void startConsecutiveLaneSelector(GNELane* lane, const Position& clickedPosition);

        /// @brief stop selection of consecutive lanes
        bool stopConsecutiveLaneSelector();

        /// @brief abort selection of consecutive lanes
        void abortConsecutiveLaneSelector();

        /// @brief return true if lane can be selected as consecutive lane
        bool addSelectedLane(GNELane* lane, const Position& clickedPosition);

        /// @brief remove last added point
        void removeLastSelectedLane();

        /// @brief return true if modul is selecting lane
        bool isSelectingLanes() const;

        /// @brief return true if modul is shown
        bool isShown() const;

        /// @brief get selected lane color
        const RGBColor& getSelectedLaneColor() const;

        /// @brief get current selected lanes
        const std::vector<std::pair<GNELane*, double> >& getSelectedLanes() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press stop selection button
        long onCmdStopSelection(FXObject*, FXSelector, void*);

        /// @brief Called when the user press abort selection button
        long onCmdAbortSelection(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        SelectorLaneParents() {}

    private:
        /// @brief pointer to additionalFrameParent
        GNEAdditionalFrame* myAdditionalFrameParent;

        /// @brief button for stop selecting
        FXButton* myStopSelectingButton;

        /// @brief button for abort selecting
        FXButton* myAbortSelectingButton;

        /// @brief Vector with the selected lanes and the clicked position
        std::vector<std::pair<GNELane*, double> > mySelectedLanes;

        /// @brief Vector with the colored lanes
        std::vector<GNELane*> myCandidateLanes;

        /// @brief color for candidate lanes
        RGBColor myCandidateLaneColor;

        /// @brief color for selected lanes
        RGBColor mySelectedLaneColor;

        /// @brief check if certain lane is selected
        bool isLaneSelected(GNELane* lane) const;
    };

    // ===========================================================================
    // class SelectorEdgeChildren
    // ===========================================================================

    class SelectorEdgeChildren : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::SelectorEdgeChildren)

    public:
        /// @brief constructor
        SelectorEdgeChildren(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~SelectorEdgeChildren();

        /// @brief get list of selecte id's in string format
        std::string getEdgeIdsSelected() const;

        /// @brief Show SelectorEdgeChildren Modul
        void showSelectorEdgeChildrenModul(std::string search = "");

        /// @brief hide SelectorEdgeChildren Modul
        void hideSelectorEdgeChildrenModul();

        /// @brief Update use selectedEdges
        void updateUseSelectedEdges();

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user trigger checkBox of useSelectedEdges
        long onCmdUseSelectedEdges(FXObject*, FXSelector, void*);

        /// @brief called when user type in search box
        long onCmdTypeInSearchBox(FXObject*, FXSelector, void*);

        /// @brief called when user select a edge of the list
        long onCmdSelectEdge(FXObject*, FXSelector, void*);

        /// @brief called when clear selection button is pressed
        long onCmdClearSelection(FXObject*, FXSelector, void*);

        /// @brief called when invert selection button is pressed
        long onCmdInvertSelection(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        SelectorEdgeChildren() {}

    private:
        /// @brief pointer to additional frame parent
        GNEAdditionalFrame* myAdditionalFrameParent;

        /// @brief CheckBox for selected edges
        FXCheckButton* myUseSelectedEdgesCheckButton;

        /// @brief List of SelectorEdgeChildren
        FXList* myList;

        /// @brief text field for search edge IDs
        FXTextField* myEdgesSearch;

        /// @brief button for clear selection
        FXButton* myClearEdgesSelection;

        /// @brief button for invert selection
        FXButton* myInvertEdgesSelection;
    };

    // ===========================================================================
    // class SelectorLaneChildren
    // ===========================================================================

    class SelectorLaneChildren : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::SelectorLaneChildren)

    public:
        /// @brief constructor
        SelectorLaneChildren(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~SelectorLaneChildren();

        /// @brief get list of selecte lane ids in string format
        std::string getLaneIdsSelected() const;

        /// @brief Show list of SelectorLaneChildren Modul
        void showSelectorLaneChildrenModul(std::string search = "");

        /// @brief hide SelectorLaneChildren Modul
        void hideSelectorLaneChildrenModul();

        // @brief Update use selectedLanes
        void updateUseSelectedLanes();

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user trigger checkBox of useSelectedLanes
        long onCmdUseSelectedLanes(FXObject*, FXSelector, void*);

        /// @brief called when user type in search box
        long onCmdTypeInSearchBox(FXObject*, FXSelector, void*);

        /// @brief called when user select a lane of the list
        long onCmdSelectLane(FXObject*, FXSelector, void*);

        /// @brief called when clear selection button is pressed
        long onCmdClearSelection(FXObject*, FXSelector, void*);

        /// @brief called when invert selection button is pressed
        long onCmdInvertSelection(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        SelectorLaneChildren() {}

    private:
        /// @brief pointer to additional frame parent
        GNEAdditionalFrame* myAdditionalFrameParent;

        /// @brief CheckBox for selected lanes
        FXCheckButton* myUseSelectedLanesCheckButton;

        /// @brief List of SelectorLaneChildren
        FXList* myList;

        /// @brief text field for search lane IDs
        FXTextField* myLanesSearch;

        /// @brief button for clear selection
        FXButton* clearLanesSelection;

        /// @brief button for invert selection
        FXButton* invertLanesSelection;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEAdditionalFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEAdditionalFrame();

    /// @brief show Frame
    void show();

    /**@brief add additional element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if additional was sucesfully added
     */
    bool addAdditional(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief show selector lane child and update use selected edges/lanes
    void showSelectorLaneChildrenModul();

    /// @brief getConsecutive Lane Selector
    GNEAdditionalFrame::SelectorLaneParents* getConsecutiveLaneSelector() const;

protected:
    /// @brief Tag selected in TagSelector
    void tagSelected();

private:
    /// @brief generate a ID for an additiona element
    std::string generateID(GNENetElement* netElement) const;

    /// @brief build common additional attributes
    bool buildAdditionalCommonAttributes(std::map<SumoXMLAttr, std::string>& valuesMap, const GNEAttributeCarrier::TagProperties& tagValues);

    /// @brief build additional with Parent
    bool buildAdditionalWithParent(std::map<SumoXMLAttr, std::string>& valuesMap, GNEAdditional* parent, const GNEAttributeCarrier::TagProperties& tagValues);

    /// @brief build additional over an edge (parent of lane)
    bool buildAdditionalOverEdge(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane, const GNEAttributeCarrier::TagProperties& tagValues);

    /// @brief build additional over a single lane
    bool buildAdditionalOverLane(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane, const GNEAttributeCarrier::TagProperties& tagValues);

    /// @brief build additional over lanes
    bool buildAdditionalOverLanes(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane, const GNEAttributeCarrier::TagProperties& tagValues);

    /// @brief build additional over view
    bool buildAdditionalOverView(std::map<SumoXMLAttr, std::string>& valuesMap, const GNEAttributeCarrier::TagProperties& tagValues);

    /// @brief item selector
    GNEFrameModuls::TagSelector* myAdditionalTagSelector;

    /// @brief internal additional attributes
    GNEFrameAttributesModuls::AttributesCreator* myAdditionalAttributes;

    /// @brief Netedit parameter
    GNEFrameAttributesModuls::NeteditAttributes* myNeteditAttributes;

    /// @brief Modul for select lane parents (currently only consecutives)
    SelectorLaneParents* mySelectorLaneParents;

    /// @brief Modul for select a single additional parent
    GNEFrameModuls::SelectorParent* myAdditionalParent;

    /// @brief Modul for select edge children
    SelectorEdgeChildren* mySelectorEdgeChildren;

    /// @brief Modul for select lane children
    SelectorLaneChildren* mySelectorLaneChildren;
};


#endif

/****************************************************************************/
