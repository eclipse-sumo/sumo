/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETAZFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2018
/// @version $Id$
///
// The Widget for add TAZ elements
/****************************************************************************/
#ifndef GNETAZFrame_h
#define GNETAZFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETAZFrame
 * The Widget for setting internal attributes of TAZ elements
 */
class GNETAZFrame : public GNEFrame {

public:
    // ===========================================================================
    // class TAZCurrent
    // ===========================================================================

    class TAZCurrent : protected FXGroupBox {

    public:
        /// @brief constructor
        TAZCurrent(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZCurrent();

        /// @brief set current TAZ
        void setTAZ(GNETAZ* TAZCurrent);

        /// @brief get current TAZ
        GNETAZ* getTAZ() const;

    private:
        /// @brief pointer to TAZ Frame
        GNETAZFrame* myTAZFrameParent;

        /// @brief current TAZ
        GNETAZ* myTAZCurrent;

        /// @brief Label for current TAZ
        FXLabel* myTAZCurrentLabel;
    };

    // ===========================================================================
    // class TAZEdgesSelector
    // ===========================================================================

    class TAZEdgesSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZEdgesSelector)

    public:
        /// @brief constructor
        TAZEdgesSelector(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZEdgesSelector();

        /// @brief show TAZEdgesSelector
        void showEdgeTAZSelectorModul();

        /// @brief hide TAZEdgesSelector
        void hideEdgeTAZSelectorModul();

        /// @brief select edge
        bool selectEdge(GNEEdge *edge);

        /// @brief update list of EdgeTAZRow
        void updateList();

        /// @name FOX-callbacks
        /// @{

        /// @brief called when user change a depart or arrival weight
        long onCmdSetAttribute(FXObject* obj, FXSelector, void*);

        /// @brieef called when user press a remove edgeTAZ button
        long onCmdRemoveEdgeTAZ(FXObject* obj, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        TAZEdgesSelector() {}

    private:
        /// @brief struct for show Edge TAZs in frame based on a vertical frame
        class EdgeTAZRow : private FXVerticalFrame {

        public:
            /// @brief constructor
            EdgeTAZRow(TAZEdgesSelector *TAZEdgesSelector, GNEEdge *edge, GNEAdditional *TAZSource, GNEAdditional *TAZSink);

            /// @brief destructor
            ~EdgeTAZRow();

            /// @brief pointer to associated TAZSource
            GNEAdditional *getEditedTAZSource() const;

            /// @brief pointer to associated TAZSink
            GNEAdditional *getEditedTAZSink() const;

            /// @brief get remove button
            FXButton *getRemoveButton() const;

            /// @brief get textField for depart weight
            FXTextField *getDepartWeightTextField() const;

            /// @brief get textField for arrival weight
            FXTextField *getArrivalWeightTextField() const;

            /// @brief pointer to TAZEdge's edge
            GNEEdge *getEdge() const;

        private:
            /// @brief pointer to Edges TAZSelector Parent
            TAZEdgesSelector *myTAZEdgesSelectorParent;

            /// @brief pointer to edited TAZ Source
            GNEAdditional *myEditedTAZSource;

            /// @brief pointer to edited TAZ Sink
            GNEAdditional *myEditedTAZSink;

            /// @brief pointer to Parent's edge
            GNEEdge *myEdge;

            /// @brief edge label
            FXLabel *myEdgeLabel;

            /// @brief remove button
            FXButton *myRemoveButton;

            /// @brief textField for depart weight
            FXTextField *myDepartWeightTextField;

            /// @brief textField for arrival weight
            FXTextField *myArrivalWeightTextField;
        };

        /// @brief pointer to GNETAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @brief Label for current edges
        FXLabel *myCurrentEdgesLabel;

        /// @brief vector with the EdgeTAZ Rows
        std::vector<EdgeTAZRow*> myEdgeTAZRows;
    };

    // ===========================================================================
    // class TAZSaveEdges
    // ===========================================================================

    class TAZSaveEdges : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZSaveEdges)

    public:
        /// @brief constructor
        TAZSaveEdges(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZSaveEdges();

        /// @brief show save TAZ Edges Modul
        void showTAZSaveEdgesModul();

        /// @brief hide save TAZ Edges Modul
        void hideTAZSaveEdgesModul();

        /// @brief enable buttons save and cancel changes
        void enableButtons();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press the button save changes
        long onCmdSaveChanges(FXObject*, FXSelector, void*);

        /// @brief Called when the user press the button cancel changes
        long onCmdCancelChanges(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        TAZSaveEdges() {}

    private:
        /// @brief pointer to TAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @field FXButton for save changes in TAZEdges
        FXButton* mySaveChangesButton;

        /// @field FXButton for cancel changes in TAZEdges
        FXButton* myCancelChangesButton;
    };

    // ===========================================================================
    // class TAZEdgesCommonParameters
    // ===========================================================================

    class TAZEdgesCommonParameters : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZEdgesCommonParameters)

    public:
        /// @brief constructor
        TAZEdgesCommonParameters(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZEdgesCommonParameters();

        /// @brief show save TAZ Edges Modul
        void showTAZEdgesCommonParametersModul();

        /// @brief hide save TAZ Edges Modul
        void hideTAZEdgesCommonParametersModul();

        /// @brief get default TAZSource weight
        double getDefaultTAZSourceWeight() const;

        /// @brief default TAZSink weight
        double getDefaultTAZSinkWeight() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press the button save changes
        long onCmdSetDefaultValues(FXObject* obj, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        TAZEdgesCommonParameters() {}

    private:
        /// @brief pointer to TAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @brief CheckButton to enable or disable Toggle edge Membership
        FXCheckButton* myToggleMembership;

        /// @brief CheckButton to enable or disables a default value for new TAZ Sources
        FXCheckButton* myCheckBoxSetDefaultValueTAZSources;

        /// @brief textField to set a default value for TAZ Sources
        FXTextField* myTextFieldDefaultValueTAZSources;

        /// @brief CheckButton to enable or disables a default value for new TAZ Sinks
        FXCheckButton* myCheckBoxSetDefaultValueTAZSinks;

        /// @brief textField to set a default value for TAZ Sinks
        FXTextField* myTextFieldDefaultValueTAZSinks;

        /// @brief default TAZSource weight
        double myDefaultTAZSourceWeight;

        /// @brief default TAZSink weight
        double myDefaultTAZSinkWeight;
    };

    // ===========================================================================
    // class TAZParameters
    // ===========================================================================

    class TAZParameters : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZParameters)

    public:
        /// @brief constructor
        TAZParameters(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZParameters();

        /// @brief show TAZ parameters and set the default value of parameters
        void showTAZParametersModul();

        /// @brief hide TAZ parameters
        void hideTAZParametersModul();

        /// @brief check if current parameters are valid
        bool isCurrentParametersValid() const;

        /// @brief check if edges within has to be used after TAZ Creation
        bool isAddEdgesWithinEnabled() const;

        /// @brief get a map with attributes and their values
        std::map<SumoXMLAttr, std::string> getAttributesAndValues() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user press the "Color" button
        long onCmdSetColorAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when user set a value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        TAZParameters() {}

    private:
        /// @brief pointer to GNETAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @brief Button for open color editor
        FXButton* myColorEditor;

        /// @brief textField to modify the default value of color parameter
        FXTextField* myTextFieldColor;

        /// @brief CheckButton to enable or disable use edges within TAZ after creation
        FXCheckButton* myAddEdgesWithinCheckButton;

        /// @brief button for help
        FXButton* myHelpTAZAttribute;
    };
    
    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNETAZFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNETAZFrame();

    /// @brief hide TAZ frame
    void hide();

    /**@brief process click over Viewnet
    * @param[in] clickedPosition clicked position over ViewNet
    * @param[in] TAZ clicked TAZ (can be null)
    * @param[in] edge clicked edge (can be null)
    * @return true if something (select TAZ or add edge) was sucefully done
    */
    bool processClick(const Position& clickedPosition, GNETAZ* taz, GNEEdge* edge);

    /// @brief get drawing mode editor
    DrawingShape* getDrawingShape() const;

    /// @brief get Current TAZ
    TAZCurrent* getTAZCurrent() const;

protected:
    /**@brief build a shaped element using the drawed shape
     * return true if was sucesfully created
     * @note called when user stop drawing shape
     */
    bool buildShape();

     /// @brief enable moduls depending of item selected in ItemSelector
    void enableModuls(const GNEAttributeCarrier::TagValues &tagValue);

    /// @brief disable moduls if element selected in itemSelector isn't valid
    void disableModuls();

private:
    /// @brief current TAZ
    TAZCurrent* myTAZCurrent;

    /// @brief TAZ parameters
    TAZParameters* myTAZParameters;

    /// @brief Netedit parameter
    NeteditAttributes* myNeteditAttributes;

    /// @brief Drawing shape
    DrawingShape* myDrawingShape;

    /// @brief save TAZ Edges
    TAZSaveEdges* myTAZSaveEdges;

    /// @brief TAZ Edges common parameters
    TAZEdgesCommonParameters* myTAZEdgesCommonParameters;

    /// @brief edge TAZ selector
    TAZEdgesSelector* myTAZEdgesSelector;
};


#endif

/****************************************************************************/
