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
#include <config.h>

#include "GNEFrame.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAttributeCarrier;
class GNENetElement;
class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEAdditionalFrame
 * The Widget for setting internal attributes of additional elements
 */
class GNEAdditionalFrame : public GNEFrame {

public:

    /// @brief enum with all possible values after try to create an additional using frame
    enum AddAdditionalResult {
        ADDADDITIONAL_INVALID_ARGUMENTS,    // Parameters of additionals are invalid
        ADDADDITIONAL_INVALID_PARENT,       // NetElement parent is invalid
        ADDADDITIONAL_SUCCESS               // additional was successfully created
    };

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
        void showAdditionalParameters();

        /// @brief hide group box
        void hideAdditionalParameters();

        /// @brief get attributes and their values
        std::map<SumoXMLAttr, std::string> getAttributesAndValues() const;

        /// @brief check if parameters of attributes are valid
        bool areValuesValid() const;

        /// @brief show warning message with information about non-valid attributes
        void showWarningMessage(std::string extra = "") const;

        /// @brief get additional frame parent
        GNEAdditionalFrame* getAdditionalFrameParent() const;

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

        /// @brief vector with the additional parameters
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

        /// @brief show length field and reference point
        void showNeteditAttributes(bool includeLengthAndReferencePoint);

        /// @brief show length field and reference point
        void hideNeteditAttributes();

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
    // class SelectorParentAdditional
    // ===========================================================================

    class SelectorParentAdditional : protected FXGroupBox {
    public:
        /// @brief constructor
        SelectorParentAdditional(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~SelectorParentAdditional();

        /// @brief get currently additional parent selected
        std::string getIdSelected() const;

        /// @brief select manually a element of the list
        void setIDSelected(const std::string& id);

        /// @brief Show list of SelectorParentAdditional
        void showListOfAdditionalParents(SumoXMLTag additionalTypeParent);

        /// @brief hide SelectorParentAdditional
        void hideListOfAdditionalParents();

        /// @brief Refresh list of Additional Parents
        void refreshListOfAdditionalParents();

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
    // class SelectorParentEdges
    // ===========================================================================

    class SelectorParentEdges : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::SelectorParentEdges)

    public:
        /// @brief constructor
        SelectorParentEdges(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~SelectorParentEdges();

        /// @brief get list of selecte id's in string format
        std::string getIdsSelected() const;

        /// @brief Show list of SelectorParentEdges
        void showList(std::string search = "");

        /// @brief hide SelectorParentEdges
        void hideList();

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
        SelectorParentEdges() {}

    private:
        /// @brief pointer to additional frame parent
        GNEAdditionalFrame* myAdditionalFrameParent;

        /// @brief CheckBox for selected edges
        FXCheckButton* myUseSelectedEdgesCheckButton;

        /// @brief List of SelectorParentEdges
        FXList* myList;

        /// @brief text field for search edge IDs
        FXTextField* myEdgesSearch;

        /// @brief button for clear selection
        FXButton* myClearEdgesSelection;

        /// @brief button for invert selection
        FXButton* myInvertEdgesSelection;
    };

    // ===========================================================================
    // class SelectorParentLanes
    // ===========================================================================

    class SelectorParentLanes : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::SelectorParentLanes)

    public:
        /// @brief constructor
        SelectorParentLanes(GNEAdditionalFrame* additionalFrameParent);

        /// @brief destructor
        ~SelectorParentLanes();

        /// @brief get list of selecte id's in string format
        std::string getIdsSelected() const;

        /// @brief Show list of SelectorParentLanes
        void showList(std::string search = "");

        /// @brief hide SelectorParentLanes
        void hideList();

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
        SelectorParentLanes() {}

    private:
        /// @brief pointer to additional frame parent
        GNEAdditionalFrame* myAdditionalFrameParent;

        /// @brief CheckBox for selected lanes
        FXCheckButton* myUseSelectedLanesCheckButton;

        /// @brief List of SelectorParentLanes
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
     * @param[in] netElement clicked GNENetElement. If user doesn't clicked over a GNENetElement in view, netElement will be nullptr
     * @param[in] additional clicked GNEAdditional. If user dind't clicked over a GNENetElement in view, additional will be nullptr
     * @return AddAdditionalStatus with the result of operation
     */
    AddAdditionalResult addAdditional(GNENetElement* netElement, GNEAdditional* additional);

    /**@brief remove an additional element previously added
     * @param[in] additional element to erase
     */
    void removeAdditional(GNEAdditional* additional);

    /// @brief show additional frame and update use selected edges/lanes
    void show();

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

protected:
    /// @brief get additional selector
    AdditionalSelector* getAdditionalSelector() const;

    /// @brief get additional attributes
    AdditionalAttributes* getAdditionalParameters() const;

    /// @brief get netedit attributes
    NeteditAttributes* getNeteditAttributes() const;

    /// @brief get additional parent selector
    SelectorParentAdditional* getAdditionalParentSelector() const;

    /// @brief get edge parents selector
    SelectorParentEdges* getEdgeParentsSelector() const;

    /// @brief get lane parents selector
    SelectorParentLanes* getLaneParentsSelector() const;

private:
    /// @brief generate a ID for an additiona element
    std::string generateID(GNENetElement* netElement) const;

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

    /// @brief list of additional Set
    GNEAdditionalFrame::SelectorParentAdditional* myFirstAdditionalParentSelector;

    /// @brief list of SelectorParentEdges
    GNEAdditionalFrame::SelectorParentEdges* myEdgeParentsSelector;

    /// @brief list of SelectorParentLanes
    GNEAdditionalFrame::SelectorParentLanes* myLaneParentsSelector;
};


#endif

/****************************************************************************/
