/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
        void startConsecutiveLaneSelector(GNELane *lane, const Position &clickedPosition);

        /// @brief stop selection of consecutive lanes
        bool stopConsecutiveLaneSelector();

        /// @brief abort selection of consecutive lanes
        void abortConsecutiveLaneSelector();

        /// @brief return true if lane can be selected as consecutive lane
        bool addSelectedLane(GNELane *lane, const Position &clickedPosition);

        /// @brief remove last added point
        void removeLastSelectedLane();

        /// @brief return true if modul is selecting lane
        bool isSelectingLanes() const;

        /// @brief return true if modul is shown
        bool isShown() const;

        /// @brief get selected lane color
        const RGBColor &getSelectedLaneColor() const;

        /// @brief get current selected lanes
        const std::vector<std::pair<GNELane*, double> > &getSelectedLanes() const;

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
        bool isLaneSelected(GNELane *lane) const;
    };

    // ===========================================================================
    // class SelectorAdditionalParent
    // ===========================================================================

    class SelectorAdditionalParent : protected FXGroupBox {
    public:
        /// @brief constructor
        SelectorAdditionalParent(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~SelectorAdditionalParent();

        /// @brief get currently additional parent selected
        std::string getIdSelected() const;

        /// @brief select manually a element of the list
        void setIDSelected(const std::string& id);

        /// @brief Show list of SelectorAdditionalParent Modul
        bool showSelectorAdditionalParentModul(SumoXMLTag additionalTypeParent);

        /// @brief hide SelectorAdditionalParent Modul
        void hideSelectorAdditionalParentModul();

        /// @brief Refresh list of Additional Parents Modul
        void refreshSelectorAdditionalParentModul();

    private:
        /// @brief pointer to Additional Frame Parent
        GNEAdditionalFrame* myAdditionalFrameParent;

        /// @brief current additional type parent
        SumoXMLTag myAdditionalTypeParent;

        /// @brief Label with the name of additional
        FXLabel* myFirstAdditionalParentsLabel;

        /// @brief List of additional sets
        FXList* myFirstAdditionalParentsList;
    };

    // ===========================================================================
    // class SelectorEdgeChilds
    // ===========================================================================

    class SelectorEdgeChilds : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::SelectorEdgeChilds)

    public:
        /// @brief constructor
        SelectorEdgeChilds(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~SelectorEdgeChilds();

        /// @brief get list of selecte id's in string format
        std::string getEdgeIdsSelected() const;

        /// @brief Show SelectorEdgeChilds Modul
        void showSelectorEdgeChildsModul(std::string search = "");

        /// @brief hide SelectorEdgeChilds Modul
        void hideSelectorEdgeChildsModul();

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
        SelectorEdgeChilds() {}

    private:
        /// @brief pointer to additional frame parent
        GNEAdditionalFrame* myAdditionalFrameParent;

        /// @brief CheckBox for selected edges
        FXCheckButton* myUseSelectedEdgesCheckButton;

        /// @brief List of SelectorEdgeChilds
        FXList* myList;

        /// @brief text field for search edge IDs
        FXTextField* myEdgesSearch;

        /// @brief button for clear selection
        FXButton* myClearEdgesSelection;

        /// @brief button for invert selection
        FXButton* myInvertEdgesSelection;
    };

    // ===========================================================================
    // class SelectorLaneChilds
    // ===========================================================================

    class SelectorLaneChilds : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::SelectorLaneChilds)

    public:
        /// @brief constructor
        SelectorLaneChilds(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~SelectorLaneChilds();

        /// @brief get list of selecte lane ids in string format
        std::string getLaneIdsSelected() const;

        /// @brief Show list of SelectorLaneChilds Modul
        void showSelectorLaneChildsModul(std::string search = "");

        /// @brief hide SelectorLaneChilds Modul
        void hideSelectorLaneChildsModul();

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
        SelectorLaneChilds() {}

    private:
        /// @brief pointer to additional frame parent
        GNEAdditionalFrame* myAdditionalFrameParent;

        /// @brief CheckBox for selected lanes
        FXCheckButton* myUseSelectedLanesCheckButton;

        /// @brief List of SelectorLaneChilds
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
    bool addAdditional(const GNEViewNet::ObjectsUnderCursor &objectsUnderCursor);

    /// @brief show selector lane child and update use selected edges/lanes
    void showSelectorLaneChildsModul();

    /// @brief getConsecutive Lane Selector
    GNEAdditionalFrame::SelectorLaneParents* getConsecutiveLaneSelector() const;

protected:
     /// @brief enable moduls depending of item selected in ItemSelector
    void enableModuls(const GNEAttributeCarrier::TagProperties &tagProperties);

    /// @brief disable moduls if element selected in itemSelector isn't valid
    void disableModuls();

private:
    /// @brief generate a ID for an additiona element
    std::string generateID(GNENetElement* netElement) const;

    /// @brief build common additional attributes
    bool buildAdditionalCommonAttributes(std::map<SumoXMLAttr, std::string> &valuesMap, const GNEAttributeCarrier::TagProperties &tagValues);

    /// @brief build additional with Parent
    bool buildAdditionalWithParent(std::map<SumoXMLAttr, std::string> &valuesMap, GNEAdditional* parent, const GNEAttributeCarrier::TagProperties &tagValues);

    /// @brief build additional over an edge (parent of lane)
    bool buildAdditionalOverEdge(std::map<SumoXMLAttr, std::string> &valuesMap, GNELane* lane, const GNEAttributeCarrier::TagProperties &tagValues);

    /// @brief build additional over a single lane
    bool buildAdditionalOverLane(std::map<SumoXMLAttr, std::string> &valuesMap, GNELane* lane, const GNEAttributeCarrier::TagProperties &tagValues);

    /// @brief build additional over lanes
    bool buildAdditionalOverLanes(std::map<SumoXMLAttr, std::string> &valuesMap, GNELane* lane, const GNEAttributeCarrier::TagProperties &tagValues);

    /// @brief build additional over view
    bool buildAdditionalOverView(std::map<SumoXMLAttr, std::string> &valuesMap, const GNEAttributeCarrier::TagProperties &tagValues);

    /// @brief item selector
    ItemSelector* myItemSelector;

    /// @brief internal additional attributes
    ACAttributes* myAdditionalAttributes;

    /// @brief Netedit parameter
    NeteditAttributes* myNeteditAttributes;
    
    /// @brief Modul for select lane parents (currently only consecutives)
    SelectorLaneParents* mySelectorLaneParents;

    /// @brief Modul for select a single additional parent (Used only for first Additional parent)
    SelectorAdditionalParent* mySelectorAdditionalParent;

    /// @brief Modul for select edge childs
    SelectorEdgeChilds* mySelectorEdgeChilds;

    /// @brief Modul for select lane childs
    SelectorLaneChilds* mySelectorLaneChilds;
};


#endif

/****************************************************************************/
