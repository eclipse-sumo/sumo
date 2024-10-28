/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNETAZFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2018
///
// The Widget for add TAZ elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/GNEDrawingShape.h>


// ===========================================================================
// class definitions
// ===========================================================================
class GNETAZ;
class GNETAZSourceSink;


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
    // class CurrentTAZ
    // ===========================================================================

    class CurrentTAZ : public MFXGroupBoxModule {

    public:
        /// @brief struct for edges and the source/sink colors
        class TAZEdgeColor {

        public:
            /// @brief constructor
            TAZEdgeColor(CurrentTAZ* CurrentTAZParent, GNEEdge* _edge, GNETAZSourceSink* _source, GNETAZSourceSink* _sink);

            /// @brief destructor (needed because RGBColors has to be deleted)
            ~TAZEdgeColor();

            /// @brief update colors
            void updateColors();

            /// @brief TAZ edge
            GNEEdge* edge;

            /// @brief source TAZ
            GNETAZSourceSink* source;

            /// @brif sink TAZ
            GNETAZSourceSink* sink;

            /// @brief color by source [0-9]
            int sourceColor;

            /// @brief color by sink [0-9]
            int sinkColor;

            /// @brief color by source + sink [0-9]
            int sourcePlusSinkColor;

            /// @brief color by source - sink [0-9]
            int sourceMinusSinkColor;

        private:
            /// @brief pointer to CurrentTAZParent
            CurrentTAZ* myCurrentTAZParent;

            /// @brief default color
            TAZEdgeColor();
        };

        /// @brief constructor
        CurrentTAZ(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~CurrentTAZ();

        /// @brief set current TAZ
        void setTAZ(GNETAZ* editedTAZ);

        /// @brief get current TAZ
        GNETAZ* getTAZ() const;

        /// @brief check if given edge belongs to current TAZ
        bool isTAZEdge(GNEEdge* edge) const;

        /// @brief get current selected edges
        const std::vector<GNEEdge*>& getSelectedEdges() const;

        /// @brief get TAZEdges
        const std::vector<CurrentTAZ::TAZEdgeColor>& getTAZEdges() const;

        /// @brief refresh TAZEdges
        void refreshTAZEdges();

    protected:
        /// @brief add TAZChild
        void addTAZChild(GNETAZSourceSink* additional);

    private:
        /// @brief pointer to TAZ Frame
        GNETAZFrame* myTAZFrameParent;

        /// @brief current edited TAZ
        GNETAZ* myEditedTAZ;

        /// @brief vector with pointers to selected edges
        std::vector<GNEEdge*> mySelectedEdges;

        /// @brief vector with TAZ's edges
        std::vector<TAZEdgeColor> myTAZEdgeColors;

        /// @brief Label for current TAZ
        FXLabel* myCurrentTAZLabel;

        /// @brief maximum source plus sink value of current TAZ Edges
        double myMaxSourcePlusSinkWeight;

        /// @brief minimum source plus sink value of current TAZ Edges
        double myMinSourcePlusSinkWeight;

        /// @brief maximum source minus sink value of current TAZ Edges
        double myMaxSourceMinusSinkWeight;

        /// @brief minimum source minus sink value of current TAZ Edges
        double myMinSourceMinusSinkWeight;
    };

    // ===========================================================================
    // class TAZCommonStatistics
    // ===========================================================================

    class TAZCommonStatistics : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        TAZCommonStatistics(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZCommonStatistics();

        /// @brief show TAZ Common Statistics Module
        void showTAZCommonStatisticsModule();

        /// @brief hide TAZ Common Statistics Module
        void hideTAZCommonStatisticsModule();

        /// @brief update Statistics label
        void updateStatistics();

    private:
        /// @brief pointer to TAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @brief Statistics labels
        FXLabel* myStatisticsLabel;
    };

    // ===========================================================================
    // class TAZSaveChanges
    // ===========================================================================

    class TAZSaveChanges : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZSaveChanges)

    public:
        /// @brief constructor
        TAZSaveChanges(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZSaveChanges();

        /// @brief show TAZ Save Changes Module
        void showTAZSaveChangesModule();

        /// @brief hide TAZ Save Changes Module
        void hideTAZSaveChangesModule();

        /// @brief enable buttons save and cancel changes (And begin Undo List)
        void enableButtonsAndBeginUndoList();

        /// @brief return true if there is changes to save
        bool isChangesPending() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press the button save changes
        long onCmdSaveChanges(FXObject*, FXSelector, void*);

        /// @brief Called when the user press the button cancel changes
        long onCmdCancelChanges(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(TAZSaveChanges)

    private:
        /// @brief pointer to TAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @field FXButton for save changes in TAZEdges
        FXButton* mySaveChangesButton;

        /// @field FXButton for cancel changes in TAZEdges
        FXButton* myCancelChangesButton;
    };

    // ===========================================================================
    // class TAZChildDefaultParameters
    // ===========================================================================

    class TAZChildDefaultParameters : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZChildDefaultParameters)

    public:
        /// @brief constructor
        TAZChildDefaultParameters(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZChildDefaultParameters();

        /// @brief extend TAZ child default parameters Module (if we have selected a TAZ)
        void extendTAZChildDefaultParameters();

        /// @brief collapse TAZ child default parameters Module (if we have selected a TAZ)
        void collapseTAZChildDefaultParameters();

        /// @brief update "select edges button"
        void updateSelectEdgesButton();

        /// @brief get default source weight
        double getDefaultTAZSourceWeight() const;

        /// @brief default sink weight
        double getDefaultTAZSinkWeight() const;

        /// @brief check if toggle membership is enabled
        bool getToggleMembership() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user changes default values
        long onCmdSetDefaultValues(FXObject* obj, FXSelector, void*);

        /// @brief Called when the user press "use selected edges" button
        long onCmdUseSelectedEdges(FXObject* obj, FXSelector, void*);

        /// @brief Called when the user press "zero fringe probabilities" button
        long onCmdSetZeroFringeProbabilities(FXObject* obj, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(TAZChildDefaultParameters)

    private:
        /// @brief pointer to TAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @brief CheckButton to enable or disable Toggle edge Membership
        FXCheckButton* myToggleMembership;

        /// @brief Horizontal Frame toggle membership
        FXHorizontalFrame* myToggleMembershipFrame;

        /// @brief Horizontal Frame for default TAZ Source Weight
        FXHorizontalFrame* myDefaultTAZSourceFrame;

        /// @brief textField to set a default value for TAZ Sources
        FXTextField* myTextFieldDefaultValueTAZSources;

        /// @brief Horizontal Frame for default TAZ Sink Weight
        FXHorizontalFrame* myDefaultTAZSinkFrame;

        /// @brief textField to set a default value for TAZ Sinks
        FXTextField* myTextFieldDefaultValueTAZSinks;

        /// @brief button for use selected edges
        FXButton* myUseSelectedEdges;

        /// @brief button for setting zero fringe probabilities
        FXButton* myZeroFringeProbabilities;

        /// @brief information label
        FXLabel* myInformationLabel;

        /// @brief default source weight
        double myDefaultTAZSourceWeight;

        /// @brief default sink weight
        double myDefaultTAZSinkWeight;
    };

    // ===========================================================================
    // class TAZSelectionStatistics
    // ===========================================================================

    class TAZSelectionStatistics : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZSelectionStatistics)

    public:
        /// @brief constructor
        TAZSelectionStatistics(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZSelectionStatistics();

        /// @brief show TAZ Selection Statistics Module
        void showTAZSelectionStatisticsModule();

        /// @brief hide TAZ Selection Statistics Module
        void hideTAZSelectionStatisticsModule();

        /// @brief add an edge and their TAZ Children in the list of selected items
        bool selectEdge(const CurrentTAZ::TAZEdgeColor& edge);

        /// @brief un select an edge (and their TAZ Children)
        bool unselectEdge(GNEEdge* edge);

        /// @brief check if an edge is selected
        bool isEdgeSelected(GNEEdge* edge);

        /// @brief clear current TAZ children
        void clearSelectedEdges();

        /// @brief get map with edge and TAZChildren
        const std::vector<CurrentTAZ::TAZEdgeColor>& getEdgeAndTAZChildrenSelected() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user set a new value
        long onCmdSetNewValues(FXObject* obj, FXSelector, void*);

        /// @brief Called when the user press select edges
        long onCmdSelectEdges(FXObject* obj, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(TAZSelectionStatistics)

        /// @brief update TAZSelectionStatistics
        void updateStatistics();

    private:
        /// @brief pointer to TAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @brief Horizontal Frame for default TAZ Source Weight
        FXHorizontalFrame* myTAZSourceFrame;

        /// @brief textField for TAZ Source weight
        FXTextField* myTextFieldTAZSourceWeight;

        /// @brief Horizontal Frame for default TAZ Sink Weight
        FXHorizontalFrame* myTAZSinkFrame;

        /// @brief textField for TAZ Sink weight
        FXTextField* myTextFieldTAZSinkWeight;

        /// @brief Statistics labels
        FXLabel* myStatisticsLabel;

        /// @brief vector with the current selected edges and their associated children
        std::vector<CurrentTAZ::TAZEdgeColor> myEdgeAndTAZChildrenSelected;
    };

    // ===========================================================================
    // class TAZParameters
    // ===========================================================================

    class TAZParameters : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZParameters)

    public:
        /// @brief constructor
        TAZParameters(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZParameters();

        /// @brief show TAZ parameters and set the default value of parameters
        void showTAZParametersModule();

        /// @brief hide TAZ parameters
        void hideTAZParametersModule();

        /// @brief check if current parameters are valid
        bool isCurrentParametersValid() const;

        /// @brief check if edges within has to be used after TAZ Creation
        bool isAddEdgesWithinEnabled() const;

        /// @brief get a map with attributes and their values
        void getAttributesAndValues() const;

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
        FOX_CONSTRUCTOR(TAZParameters)

    private:
        /// @brief pointer to GNETAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @brief TAZ
        GNETAZ* myTAZTemplate;

        /// @brief Button for open color editor
        FXButton* myColorEditor;

        /// @brief text field center
        FXTextField* myTextFieldCenter;

        /// @brief CheckButton to enable or disable fill
        FXCheckButton* myCheckButtonFill;

        /// @brief textField to modify the default value of color parameter
        FXTextField* myTextFieldColor;

        /// @brief textField to modify the default value of name parameter
        FXTextField* myTextFieldName;

        /// @brief CheckButton to enable or disable use edges within TAZ after creation
        FXCheckButton* myAddEdgesWithinCheckButton;

        /// @brief button for help
        FXButton* myHelpTAZAttribute;
    };

    // ===========================================================================
    // class TAZEdgesGraphic
    // ===========================================================================

    class TAZEdgesGraphic : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZEdgesGraphic)

    public:
        /// @brief constructor
        TAZEdgesGraphic(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZEdgesGraphic();

        /// @brief show TAZ Edges Graphic Module
        void showTAZEdgesGraphicModule();

        /// @brief hide TAZ Edges Graphic Module
        void hideTAZEdgesGraphicModule();

        /// @brief update edge colors;
        void updateEdgeColors();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select one kind of representation
        long onCmdChoosenBy(FXObject* obj, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(TAZEdgesGraphic)

    private:
        /// @brief pointer to TAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @brief add radio button "color by source"
        FXRadioButton* myColorBySourceWeight;

        /// @brief add radio button "color by sink"
        FXRadioButton* myColorBySinkWeight;

        /// @brief add radio button "color source + sink"
        FXRadioButton* myColorBySourcePlusSinkWeight;

        /// @brief add radio button "color source - Sink"
        FXRadioButton* myColorBySourceMinusSinkWeight;

        /// @brief default RGBColor for all edges
        RGBColor myEdgeDefaultColor;

        /// @brief RGBColor color for selected egdes
        RGBColor myEdgeSelectedColor;
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNETAZFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNETAZFrame();

    /// @brief hide TAZ frame
    void hide();

    /**@brief process click over Viewnet
     * @param[in] clickedPosition clicked position over ViewNet
     * @param[in] viewObjects objects under cursors
     * @return true if something (select TAZ or add edge) was sucefully done
     */
    bool processClick(const Position& clickedPosition, const GNEViewNetHelper::ViewObjectsSelector& viewObjects);

    /// @brief process selection of edges in view net
    void processEdgeSelection(const std::vector<GNEEdge*>& edges);

    /// @brief get drawing mode modul
    GNEDrawingShape* getDrawingShapeModule() const;

    /// @brief get Current TAZ modul
    CurrentTAZ* getCurrentTAZModule() const;

    /// @brief get TAZ Selection Statistics modul
    TAZSelectionStatistics* getTAZSelectionStatisticsModule() const;

    /// @brief get TAZ Save Changes modul
    TAZSaveChanges* getTAZSaveChangesModule() const;

protected:
    /// @brief SumoBaseObject used for creating TAZ
    CommonXMLStructure::SumoBaseObject* myBaseTAZ;

    /**@brief build a shaped element using the drawed shape
     * return true if was successfully created
     * @note called when user stop drawing shape
     */
    bool shapeDrawed();

    /// @brief add or remove a source and a sink, or remove it if edge is in the list of TAZ Children
    bool addOrRemoveTAZMember(GNEEdge* edge);

    /// @brief drop all TAZSources and TAZ Sinks of current TAZ
    void dropTAZMembers();

private:
    /// @brief current TAZ
    CurrentTAZ* myCurrentTAZ;

    /// @brief TAZ Edges common parameters
    TAZCommonStatistics* myTAZCommonStatistics;

    /// @brief TAZ parameters
    TAZParameters* myTAZParameters;

    /// @brief Drawing shape
    GNEDrawingShape* myDrawingShape;

    /// @brief save TAZ Edges
    TAZSaveChanges* myTAZSaveChanges;

    /// @brief TAZ child defaults parameters
    TAZChildDefaultParameters* myTAZChildDefaultParameters;

    /// @brief TAZ Edges selection parameters
    TAZSelectionStatistics* myTAZSelectionStatistics;

    /// @brief TAZ Edges Graphic
    TAZEdgesGraphic* myTAZEdgesGraphic;
};
