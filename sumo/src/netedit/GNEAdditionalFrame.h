/****************************************************************************/
/// @file    GNEAdditionalFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
/// The Widget for add additional elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEAdditionalFrame_h
#define GNEAdditionalFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
    /// @brief FOX-declaration
    FXDECLARE(GNEAdditionalFrame)

public:

    // ===========================================================================
    // class AdditionalAttributeSingle
    // ===========================================================================

    class AdditionalAttributeSingle : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::AdditionalAttributeSingle)

    public:
        /// @brief constructor
        AdditionalAttributeSingle(FXComposite* parent);

        /// @brief destructor
        ~AdditionalAttributeSingle();

        /// @brief show name and value of attribute of type string
        void showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::string value);

        /// @brief show name and value of parameters of type int
        void showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, int value);

        /// @brief show name and value of parameters of type float/real/time
        void showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, SUMOReal value, bool isTime = false);

        /// @brief show name and value of parameters of type bool
        void showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, bool value);

        /// @brief hide all parameters
        void hideParameter();

        /// @brief return tag
        SumoXMLTag getTag() const;

        /// @brief return Attr
        SumoXMLAttr getAttr() const;

        /// @brief return value
        std::string getValue() const;

        /// @brief check if the current value is valid
        bool isCurrentValueValid() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user set the value of an attribute of type int/float/string
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief called when user change the value of myMenuCheck
        long onCmdSetBooleanAttribute(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        AdditionalAttributeSingle() {}

    private:
        /// @brief current XML attribute
        SumoXMLTag myAdditionalTag;

        /// @brief current XML attribute
        SumoXMLAttr myAdditionalAttr;

        /// @brief lael with the name of the parameter
        FXLabel* myLabel;

        /// @brief textField to modify the value of int/float/string parameters
        FXTextField* myTextField;

        /// @brief spindial  to modify the value of time parameters
        FXSpinner* myTimeSpinDial;

        /// @brief menuCheck to enable/disable the value of boolean parameters
        FXMenuCheck* myMenuCheck;

        /// @brief Flag which indicates that the current value is correct
        bool myCurrentValueValid;
    };

    // ===========================================================================
    // class AdditionalAttributeList
    // ===========================================================================

    class AdditionalAttributeList : public FXVerticalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::AdditionalAttributeList)

    public:
        /// @brief constructor
        AdditionalAttributeList(FXComposite* parent);

        /// @brief destructor
        ~AdditionalAttributeList();

        /// @brief show name and value of parameters of type int
        void showListParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::vector<int> value);

        /// @brief show name and value of parameters of type float
        void showListParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::vector<SUMOReal> value, bool isTime = false);

        /// @brief show name and value of parameters of type bool
        void showListParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::vector<bool> value);

        /// @brief show name and value of parameters of type string
        void showListParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::vector<std::string> value);

        /// @brief hide all parameters
        void hideParameter();

        /// @brief return tag of list
        SumoXMLTag getTag() const;

        /// @brief return attribute of list
        SumoXMLAttr getAttr() const;

        /// @brief return the value of list
        std::string getListValues();

        /// @brief check that current list is valid
        bool isCurrentListValid() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief add a new row int the list
        long onCmdAddRow(FXObject*, FXSelector, void*);

        /// @brief add a new row int the list
        long onCmdRemoveRow(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        AdditionalAttributeList() {}

    private:
        /// @brief current XML tag
        SumoXMLTag myAdditionalTag;

        /// @brief current XML attribute
        SumoXMLAttr myAdditionalAttr;

        /// @brief Vector with HorizontalFrames
        std::vector<FXHorizontalFrame*>myHorizontalFrames;

        /// @brief vector with with the name of every parameter
        std::vector<FXLabel*> myLabels;

        /// @brief vector textField to modify the value of parameter
        std::vector<FXTextField*> myTextFields;

        /// @brief horizontal frame for buttons
        FXHorizontalFrame* myHorizontalFrameButtons;

        /// @brief Button to increase the number of textFields
        FXButton* myAddButton;

        /// @brief Button to decrease the number of textFields
        FXButton* myRemoveButton;

        /// @brief number of visible text fields
        int myNumberOfVisibleTextfields;

        /// @brief Number max of values in a parameter of type list
        int myMaxNumberOfValuesInParameterList;
    };

    // ===========================================================================
    // class AdditionalAttributes
    // ===========================================================================

    class AdditionalAttributes : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::AdditionalAttributes)

    public:
        /// @brief constructor
        AdditionalAttributes(FXComposite* parent);

        /// @brief destructor
        ~AdditionalAttributes();

        /// @brief clear attributes
        void clearAttributes();

        /// @brief add attribute
        void addAttribute(SumoXMLTag additionalTag, SumoXMLAttr AdditionalAttributeSingle);

        /// @brief show group box
        void showAdditionalParameters();

        /// @brief hide group box
        void hideAdditionalParameters();

        /// @brief get attributes and their values
        std::map<SumoXMLAttr, std::string> getAttributesAndValues() const;

        /// @brief check if all values defined by user are valid
        bool areValuesValid() const;

        /// @brief get number of added attributes
        int getNumberOfAddedAttributes() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        AdditionalAttributes() {}

    private:
        /// @brief current additional tag
        SumoXMLTag myAdditionalTag;

        /// @brief vector with the additional parameters
        std::vector<AdditionalAttributeSingle*> myVectorOfsingleAdditionalParameter;

        /// @brief Index for myVectorOfsingleAdditionalParameter
        int myIndexParameter;

        /// @brief vector with the additional parameters of type list
        std::vector<AdditionalAttributeList*> myVectorOfsingleAdditionalParameterList;

        /// @brief index for myIndexParameterList
        int myIndexParameterList;

        /// @brief max number of parameters (Defined in constructor)
        int maxNumberOfParameters;

        /// @brief max number of parameters (Defined in constructor)
        int maxNumberOfListParameters;

        /// @brief button for help
        FXButton* helpAdditional;
    };

    // ===========================================================================
    // class NeteditAttributes
    // ===========================================================================

    class NeteditAttributes : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::NeteditAttributes)

    public:
        /// @brief list of the reference points
        enum additionalReferencePoint {
            GNE_ADDITIONALREFERENCEPOINT_LEFT,
            GNE_ADDITIONALREFERENCEPOINT_RIGHT,
            GNE_ADDITIONALREFERENCEPOINT_CENTER
        };

        /// @brief constructor
        NeteditAttributes(FXComposite* parent);

        /// @brief destructor
        ~NeteditAttributes();

        /// @brief show length field
        void showLengthField();

        /// @brief hide length field
        void hideLengthField();

        /// @brief show refence point comboBox
        void showReferencePoint();

        /// @brief hide refence point comboBox
        void hideReferencePoint();

        /// @brief get actual reference point
        additionalReferencePoint getActualReferencePoint();

        /// @brief get value of lenght
        SUMOReal getLenght();

        /// @brief check if block is enabled
        bool isBlockEnabled();

        /// @brief check if force position is enabled
        bool isForcePositionEnabled();

        /// @brief check if current lenght is valid
        bool isCurrentLenghtValid() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user enters a new lenght
        long onCmdSetLength(FXObject*, FXSelector, void*);

        /// @brief Called when user enters another reference point
        long onCmdSelectReferencePoint(FXObject*, FXSelector, void*);

        /// @brief Called when user changes the checkbox "set blocking"
        long onCmdSetBlocking(FXObject*, FXSelector, void*);

        /// @brief Called when user changes the checkbox "force position"
        long onCmdSetForcePosition(FXObject*, FXSelector, void*);

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

        /// @brief Label for lenght
        FXLabel* myLengthLabel;

        /// @brief textField for lenght
        FXTextField* myLengthTextField;

        /// @brief Label for force position
        FXLabel* myForcePositionLabel;

        /// @brief checkBox for the option "force position"
        FXMenuCheck* myCheckForcePosition;

        /// @brief Label for block movement
        FXLabel* myBlockLabel;

        /// @brief checkBox for block movement
        FXMenuCheck* myCheckBlock;

        /// @brief Flag to check if current lenght is valid
        bool myCurrentLengthValid;
    };

    // ===========================================================================
    // class SelectorParentAdditional
    // ===========================================================================

    class SelectorParentAdditional : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::SelectorParentAdditional)

    public:
        /// @brief constructor
        SelectorParentAdditional(FXComposite* parent, GNEViewNet* viewNet);

        /// @brief destructor
        ~SelectorParentAdditional();

        /// @brief get if currently additional Set
        std::string getIdSelected() const;

        /// @brief Show list of SelectorParentAdditional
        void showListOfAdditionals(SumoXMLTag type, bool uniqueSelection);

        /// @brief hide SelectorParentAdditional
        void hideListOfAdditionals();

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user select an SelectorParentAdditional of the list
        long onCmdSelectAdditionalParent(FXObject*, FXSelector, void*);

        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        SelectorParentAdditional() {}

    private:

        /// @brief List of additional sets
        FXList* myList;

        /// @brief Label with the name of additional
        FXLabel* mySetLabel;

        /// @brief flag to check if only a single parent is allowed
        bool myUniqueSelection;

        /// @brief button for help
        FXButton* myHelpAdditionalParentSelector;

        /// @brief viewNet associated to GNEAdditionalFrame
        GNEViewNet* myViewNet;
    };

    // ===========================================================================
    // class SelectorParentEdges
    // ===========================================================================

    class SelectorParentEdges : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::SelectorParentEdges)

    public:
        /// @brief constructor
        SelectorParentEdges(FXComposite* parent, GNEViewNet* viewNet);

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

        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        SelectorParentEdges() {}

    private:
        /// @brief CheckBox for selected edges
        FXMenuCheck* myUseSelectedEdges;

        /// @brief List of SelectorParentEdges
        FXList* myList;

        /// @brief text field for search edge IDs
        FXTextField* myEdgesSearch;

        /// @brief button for help
        FXButton* myHelpedgeParentsSelector;

        /// @brief button for clear selection
        FXButton* myClearEdgesSelection;

        /// @brief button for invert selection
        FXButton* myInvertEdgesSelection;

        /// @brief viewNet associated to GNEAdditionalFrame
        GNEViewNet* myViewNet;
    };

    // ===========================================================================
    // class SelectorParentLanes
    // ===========================================================================

    class SelectorParentLanes : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::SelectorParentLanes)

    public:
        /// @brief constructor
        SelectorParentLanes(FXComposite* parent, GNEViewNet* viewNet);

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

        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        SelectorParentLanes() {}

    private:
        /// @brief CheckBox for selected lanes
        FXMenuCheck* myUseSelectedLanes;

        /// @brief List of SelectorParentLanes
        FXList* myList;

        /// @brief text field for search lane IDs
        FXTextField* myLanesSearch;

        /// @brief button for help
        FXButton* helpLanes;

        /// @brief button for clear selection
        FXButton* clearLanesSelection;

        /// @brief button for invert selection
        FXButton* invertLanesSelection;

        /// @brief viewNet associated to GNEAdditionalFrame
        GNEViewNet* myViewNet;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEAdditionalFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEAdditionalFrame();

    /**@brief add additional element
     * @param[in] netElement clicked netElement. if user dind't clicked over a GNENetElement in view, netElement will be NULL
     * @param[in] parent AbstractView to obtain the position of the mouse over the lane.
     * @return true if an additional was added, false in other case
     */
    bool addAdditional(GNENetElement* netElement, GUISUMOAbstractView* parent);

    /**@brief remove an additional element previously added
     * @param[in] additional element to erase
     */
    void removeAdditional(GNEAdditional* additional);

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user select another additional Type
    long onCmdSelectAdditional(FXObject*, FXSelector, void*);
    /// @}

    /// @brief show additional frame and update use selected edges/lanes
    void show();

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

protected:
    /// @brief FOX needs this
    GNEAdditionalFrame() {}

private:
    /// @brief set parameters depending of the new additionalType
    void setParametersOfAdditional(SumoXMLTag actualAdditionalType);

    /// @brief generate a ID for an additiona element
    std::string generateID(GNENetElement* netElement) const;

    /// @brief obtain the Start position values of StoppingPlaces and E2 detector over the lane
    SUMOReal setStartPosition(SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfAdditional);

    /// @brief obtain the End position values of StoppingPlaces and E2 detector over the lane
    SUMOReal setEndPosition(SUMOReal laneLenght, SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfAdditional);

    /// @brief groupBox for Match Box of additionals
    FXGroupBox* myGroupBoxForMyAdditionalMatchBox;

    /// @brief combo box with the list of additional elements
    FXComboBox* myAdditionalMatchBox;

    /// @brief additional internal attributes
    GNEAdditionalFrame::AdditionalAttributes* myadditionalParameters;

    /// @brief Netedit parameter
    GNEAdditionalFrame::NeteditAttributes* myEditorParameters;

    /// @brief list of additional Set
    GNEAdditionalFrame::SelectorParentAdditional* myAdditionalParentSelector;

    /// @brief list of SelectorParentEdges
    GNEAdditionalFrame::SelectorParentEdges* myedgeParentsSelector;

    /// @brief list of SelectorParentLanes
    GNEAdditionalFrame::SelectorParentLanes* mylaneParentsSelector;

    /// @brief actual additional type selected in the match Box
    SumoXMLTag myActualAdditionalType;
};


#endif

/****************************************************************************/
