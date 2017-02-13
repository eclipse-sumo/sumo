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
 * The Widget for setting default parameters of additional elements
 */
class GNEAdditionalFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNEAdditionalFrame)

public:

    // ===========================================================================
    // class singleAdditionalParameter
    // ===========================================================================

    class singleAdditionalParameter : public FXMatrix {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::singleAdditionalParameter)

    public:
        /// @brief constructor
        singleAdditionalParameter(FXComposite* parent);

        /// @brief destructor
        ~singleAdditionalParameter();

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
        singleAdditionalParameter() {}

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
    // class singleAdditionalParameterList
    // ===========================================================================

    class singleAdditionalParameterList : public FXMatrix {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::singleAdditionalParameterList)

    public:
        /// @brief constructor
        singleAdditionalParameterList(FXComposite* parent);

        /// @brief destructor
        ~singleAdditionalParameterList();

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
        singleAdditionalParameterList() {}

    private:
        /// @brief current XML tag
        SumoXMLTag myAdditionalTag;

        /// @brief current XML attribute
        SumoXMLAttr myAdditionalAttr;

        /// @brief vector with with the name of every parameter
        std::vector<FXLabel*> myLabels;

        /// @brief vector textField to modify the value of parameter
        std::vector<FXTextField*> myTextFields;

        /// @brief Button to increase the number of textFields
        FXButton* add;

        /// @brief Button to decrease the number of textFields
        FXButton* remove;

        /// @brief number of visible text fields
        int numberOfVisibleTextfields;

        /// @brief Number max of values in a parameter of type list
        int myMaxNumberOfValuesInParameterList;
    };

    // ===========================================================================
    // class additionalParameters
    // ===========================================================================

    class additionalParameters : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::additionalParameters)

    public:
        /// @brief constructor
        additionalParameters(FXComposite* parent);

        /// @brief destructor
        ~additionalParameters();

        /// @brief clear attributes
        void clearAttributes();

        /// @brief add attribute
        void addAttribute(SumoXMLTag additionalTag, SumoXMLAttr additionalAttribute);

        /// @brief show group box
        void showAdditionalParameters();

        /// @brief hide group box
        void hideAdditionalParameters();

        /// @brief get attributes and their values
        std::map<SumoXMLAttr, std::string> getAttributesAndValues() const;

        /// @brief check if all values defined by user are valid
        bool isValuesValid() const;

        /// @brief get number of added attributes
        int getNumberOfAddedAttributes() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        additionalParameters() {}

    private:
        /// @brief current additional tag
        SumoXMLTag myAdditionalTag;

        /// @brief vector with the additional parameters
        std::vector<singleAdditionalParameter*> myVectorOfsingleAdditionalParameter;

        /// @brief Index for myVectorOfsingleAdditionalParameter
        int myIndexParameter;

        /// @brief vector with the additional parameters of type list
        std::vector<singleAdditionalParameterList*> myVectorOfsingleAdditionalParameterList;

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
    // class editorParameters
    // ===========================================================================

    class editorParameters : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::editorParameters)

    public:
        /// @brief list of the reference points
        enum additionalReferencePoint {
            GNE_ADDITIONALREFERENCEPOINT_LEFT,
            GNE_ADDITIONALREFERENCEPOINT_RIGHT,
            GNE_ADDITIONALREFERENCEPOINT_CENTER
        };

        /// @brief constructor
        editorParameters(FXComposite* parent);

        /// @brief destructor
        ~editorParameters();

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
        /// @brief Called when the user enters a new lenght
        long onCmdSetLength(FXObject*, FXSelector, void*);

        /// @brief Called when the user enters another reference point
        long onCmdSelectReferencePoint(FXObject*, FXSelector, void*);

        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        editorParameters() {}

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

        /// @brief checkBox for the option "force position"
        FXMenuCheck* myCheckForcePosition;

        /// @brief checkBox for blocking movement
        FXMenuCheck* myCheckBlock;

        /// @brief Flag to check if current lenght is valid
        bool myCurrentLengthValid;
    };

    // ===========================================================================
    // class additionalParentSelector
    // ===========================================================================

    class additionalParentSelector : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::additionalParentSelector)

    public:
        /// @brief constructor
        additionalParentSelector(FXComposite* parent, GNEViewNet* viewNet);

        /// @brief destructor
        ~additionalParentSelector();

        /// @brief get if currently additional Set
        std::string getIdSelected() const;

        /// @brief Show list of additionalParentSelector
        void showListOfAdditionals(SumoXMLTag type, bool uniqueSelection);

        /// @brief hide additionalParentSelector
        void hideListOfAdditionals();

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user select an additionalParentSelector of the list
        long onCmdSelectAdditionalParent(FXObject*, FXSelector, void*);

        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        additionalParentSelector() {}

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
    // class edgeParentsSelector
    // ===========================================================================

    class edgeParentsSelector : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::edgeParentsSelector)

    public:
        /// @brief constructor
        edgeParentsSelector(FXComposite* parent, GNEViewNet* viewNet);

        /// @brief destructor
        ~edgeParentsSelector();

        /// @brief get list of selecte id's in string format
        std::string getIdsSelected() const;

        /// @brief Show list of edgeParentsSelector
        void showList(std::string search = "");

        /// @brief hide edgeParentsSelector
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
        edgeParentsSelector() {}

    private:
        /// @brief CheckBox for selected edges
        FXMenuCheck* myUseSelectedEdges;

        /// @brief List of edgeParentsSelector
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
    // class laneParentsSelector
    // ===========================================================================

    class laneParentsSelector : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEAdditionalFrame::laneParentsSelector)

    public:
        /// @brief constructor
        laneParentsSelector(FXComposite* parent, GNEViewNet* viewNet);

        /// @brief destructor
        ~laneParentsSelector();

        /// @brief get list of selecte id's in string format
        std::string getIdsSelected() const;

        /// @brief Show list of laneParentsSelector
        void showList(std::string search = "");

        /// @brief hide laneParentsSelector
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
        laneParentsSelector() {}

    private:
        /// @brief CheckBox for selected lanes
        FXMenuCheck* myUseSelectedLanes;

        /// @brief List of laneParentsSelector
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

    /// @brief additional default parameters
    GNEAdditionalFrame::additionalParameters* myadditionalParameters;

    /// @brief editor parameter
    GNEAdditionalFrame::editorParameters* myEditorParameters;

    /// @brief list of additional Set
    GNEAdditionalFrame::additionalParentSelector* myAdditionalParentSelector;

    /// @brief list of edgeParentsSelector
    GNEAdditionalFrame::edgeParentsSelector* myedgeParentsSelector;

    /// @brief list of laneParentsSelector
    GNEAdditionalFrame::laneParentsSelector* mylaneParentsSelector;

    /// @brief actual additional type selected in the match Box
    SumoXMLTag myActualAdditionalType;
};


#endif

/****************************************************************************/
