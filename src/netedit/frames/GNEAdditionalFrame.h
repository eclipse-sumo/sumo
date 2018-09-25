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
    /// @brief class declaration
    class AdditionalAttributes;

    // ===========================================================================
    // class AdditionalSelector
    // ===========================================================================

    class AdditionalSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::AdditionalSelector)

    public:
        /// @brief constructor
        AdditionalSelector(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~AdditionalSelector();

        /// @brief get current additional type
        SumoXMLTag getCurrentAdditionalType() const;

        /// @brief set parameters depending of the given additionalType
        void setCurrentAdditional(SumoXMLTag actualAdditionalType);

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another additional Type
        long onCmdselectAttributeCarrier(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        AdditionalSelector() {}

    private:
        /// @brief pointer to Additional Frame Parent
        GNEAdditionalFrame* myAdditionalFrameParent;

        /// @brief combo box with the list of additional elements
        FXComboBox* myAdditionalMatchBox;

        /// @brief actual additional type selected in the match Box
        SumoXMLTag myCurrentAdditionalType;
    };

    // ===========================================================================
    // class AdditionalAttributeSingle
    // ===========================================================================

    class AdditionalAttributeSingle : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::AdditionalAttributeSingle)

    public:
        /// @brief constructor
        AdditionalAttributeSingle(AdditionalAttributes* additionalAttributesParent);

        /// @brief destructor
        ~AdditionalAttributeSingle();

        /// @brief show name and value of attribute of type string
        void showParameter(SumoXMLAttr additionalAttr, std::string value);

        /// @brief hide all parameters
        void hideParameter();

        /// @brief return Attr
        SumoXMLAttr getAttr() const;

        /// @brief return value
        std::string getValue() const;

        /// @brief returns a empty string if current value is valid, a string with information about invalid value in other case
        const std::string& isAttributeValid() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user set the value of an attribute of type int/float/string
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief called when user change the value of myBoolCheckButton
        long onCmdSetBooleanAttribute(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        AdditionalAttributeSingle() {}

    private:
        /// @brief additional attribute parent
        AdditionalAttributes* myAdditionalAttributesParent;

        /// @brief current XML attribute
        SumoXMLAttr myAdditionalAttr;

        /// @brief lael with the name of the parameter
        FXLabel* myLabel;

        /// @brief textField to modify the default value of int/float/string parameters
        FXTextField* myTextFieldInt;

        /// @brief textField to modify the default value of real/times parameters
        FXTextField* myTextFieldReal;

        /// @brief textField to modify the default value of string parameters
        FXTextField* myTextFieldStrings;

        /// @brief check button to enable/disable the value of boolean parameters
        FXCheckButton* myBoolCheckButton;

        /// @brief string which indicates the reason due current value is invalid
        std::string myInvalidValue;
    };

    // ===========================================================================
    // class AdditionalAttributes
    // ===========================================================================

    class AdditionalAttributes : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::AdditionalAttributes)

        /// @brief friend class declaration
        friend class AdditionalAttributeSingle;

    public:
        /// @brief constructor
        AdditionalAttributes(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~AdditionalAttributes();

        /// @brief clear attributes
        void clearAttributes();

        /// @brief add attribute
        void addAttribute(SumoXMLAttr AdditionalAttributeSingle);

        /// @brief show group box
        void showAdditionalAttributesModul();

        /// @brief hide group box
        void hideAdditionalAttributesModul();

        /// @brief get attributes and their values into valuesMap
        void getAttributesAndValues(std::map<SumoXMLAttr, std::string> &valuesMap) const;

        /// @brief check if parameters of attributes are valid
        bool areCurrentAdditionalAttributesValid() const;

        /// @brief show warning message with information about non-valid attributes
        void showWarningMessage(std::string extra = "") const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        AdditionalAttributes() {}

    private:
        /// @brief pointer to additionalFrameParent
        GNEAdditionalFrame* myAdditionalFrameParent;

        /// @brief vector with the additional attributes
        std::vector<AdditionalAttributeSingle*> myVectorOfsingleAdditionalParameter;
    };

    // ===========================================================================
    // class NeteditAttributes
    // ===========================================================================

    class NeteditAttributes : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::NeteditAttributes)

    public:
        /// @brief list of the reference points
        enum additionalReferencePoint {
            GNE_ADDITIONALREFERENCEPOINT_LEFT,
            GNE_ADDITIONALREFERENCEPOINT_RIGHT,
            GNE_ADDITIONALREFERENCEPOINT_CENTER,
            GNE_ADDITIONALREFERENCEPOINT_INVALID
        };

        /// @brief constructor
        NeteditAttributes(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~NeteditAttributes();

        /// @brief show Netedit attributes modul
        void showNeteditAttributesModul(bool includeLengthAndReferencePoint);

        /// @brief hide Netedit attributes modul
        void hideNeteditAttributesModul();

        /// @brief get actual reference point
        additionalReferencePoint getActualReferencePoint() const;

        /// @brief get value of length
        double getLength() const;

        /// @brief check if block is enabled
        bool isBlockEnabled() const;

        /// @brief check if current length is valid
        bool isCurrentLengthValid() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user enters a new length
        long onCmdSetLength(FXObject*, FXSelector, void*);

        /// @brief Called when user enters another reference point
        long onCmdSelectReferencePoint(FXObject*, FXSelector, void*);

        /// @brief Called when user changes the checkbox "set blocking"
        long onCmdSetBlocking(FXObject*, FXSelector, void*);

        /// @brief Called when user press the help button
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        NeteditAttributes() {}

    private:
        /// @brief match box with the list of reference points
        FXComboBox* myReferencePointMatchBox;

        /// @brief Button for help about the reference point
        FXButton* helpReferencePoint;

        /// @brief actual additional reference point selected in the match Box
        additionalReferencePoint myActualAdditionalReferencePoint;

        /// @brief Label for length
        FXLabel* myLengthLabel;

        /// @brief textField for length
        FXTextField* myLengthTextField;

        /// @brief Label for block movement
        FXLabel* myBlockLabel;

        /// @brief checkBox for block movement
        FXCheckButton* myBlockMovementCheckButton;

        /// @brief Flag to check if current length is valid
        bool myCurrentLengthValid;
    };

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
        std::string getIdsSelected() const;

        /// @brief Show SelectorEdgeChilds Modul
        void showSelectorEdgeChildsModul(std::string search = "");

        /// @brief hide SelectorEdgeChilds Modul
        void hideSelectorEdgeChildsModul();

        /// @brief Update use selectedEdges
        void updateUseSelectedEdges();

        /// @brief get status of checkBox UseSelectedEdges
        bool isUseSelectedEdgesEnable() const;

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

        /// @brief get list of selecte id's in string format
        std::string getIdsSelected() const;

        /// @brief Show list of SelectorLaneChilds Modul
        void showSelectorLaneChildsModul(std::string search = "");

        /// @brief hide SelectorLaneChilds Modul
        void hideSelectorLaneChildsModul();

        // @brief Update use selectedLanes
        void updateUseSelectedLanes();

        /// @brief get status of checkBox UseSelectedLanes
        bool isUseSelectedLanesEnable() const;

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

    /**@brief add additional element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if additional was sucesfully added
     */
    bool addAdditional(const GNEViewNet::ObjectsUnderCursor &objectsUnderCursor);

    /**@brief remove an additional element previously added
     * @param[in] additional element to erase
     */
    void removeAdditional(GNEAdditional* additional);

    /// @brief show additional frame and update use selected edges/lanes
    void show();

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

    /// @brief getConsecutive Lane Selector
    GNEAdditionalFrame::SelectorLaneParents* getConsecutiveLaneSelector() const;

private:
    /// @brief generate a ID for an additiona element
    std::string generateID(GNENetElement* netElement) const;

    /// @brief build common additional attributes
    bool buildAdditionalCommonAttributes(std::map<SumoXMLAttr, std::string> &valuesMap, const GNEAttributeCarrier::TagValues &tagValues);

    /// @brief build additional with Parent
    bool buildAdditionalWithParent(std::map<SumoXMLAttr, std::string> &valuesMap, GNEAdditional* parent, const GNEAttributeCarrier::TagValues &tagValues);

    /// @brief build additional over an edge
    bool buildAdditionalOverEdge(std::map<SumoXMLAttr, std::string> &valuesMap, GNEEdge* edge);

    /// @brief build additional over a single lane
    bool buildAdditionalOverLane(std::map<SumoXMLAttr, std::string> &valuesMap, GNELane* lane, const GNEAttributeCarrier::TagValues &tagValues);

    /// @brief build additional over lanes
    bool buildAdditionalOverLanes(std::map<SumoXMLAttr, std::string> &valuesMap, GNELane* lane, const GNEAttributeCarrier::TagValues &tagValues);

    /// @brief build additional over view
    bool buildAdditionalOverView(std::map<SumoXMLAttr, std::string> &valuesMap, const GNEAttributeCarrier::TagValues &tagValues);

    /// @brief obtain the Start position values of StoppingPlaces and E2 detector over the lane
    double setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional);

    /// @brief obtain the End position values of StoppingPlaces and E2 detector over the lane
    double setEndPosition(double laneLength, double positionOfTheMouseOverLane, double lengthOfAdditional);

    /// @brief additional selector
    GNEAdditionalFrame::AdditionalSelector* myAdditionalSelector;

    /// @brief additional internal attributes
    GNEAdditionalFrame::AdditionalAttributes* myAdditionalParameters;

    /// @brief Netedit parameter
    GNEAdditionalFrame::NeteditAttributes* myNeteditParameters;
    
    /// @brief Modul for select lane parents (currently only consecutives)
    GNEAdditionalFrame::SelectorLaneParents* mySelectorLaneParents;

    /// @brief Modul for select a single additional parent (Used only for first Additional parent)
    GNEAdditionalFrame::SelectorAdditionalParent* mySelectorAdditionalParent;

    /// @brief Modul for select edge childs
    GNEAdditionalFrame::SelectorEdgeChilds* mySelectorEdgeChilds;

    /// @brief Modul for select lane childs
    GNEAdditionalFrame::SelectorLaneChilds* mySelectorLaneChilds;
};


#endif

/****************************************************************************/
