/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEFrameModuls.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliar class for GNEFrame Moduls
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/GNEViewNetHelper.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFrameModuls {

public:
    // ===========================================================================
    // class TagSelector
    // ===========================================================================

    class TagSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameModuls::TagSelector)

    public:
        /// @brief constructor
        TagSelector(GNEFrame* frameParent, GNETagProperties::TagType type, bool onlyDrawables = true);

        /// @brief destructor
        ~TagSelector();

        /// @brief show item selector
        void showTagSelector();

        /// @brief hide item selector
        void hideTagSelector();

        /// @brief get current type tag
        const GNETagProperties& getCurrentTagProperties() const;

        /// @brief set current type manually
        void setCurrentTagType(GNETagProperties::TagType tagType);

        /// @brief set current type manually
        void setCurrentTag(SumoXMLTag newTag);

        /// @brief due myCurrentTagProperties is a Reference, we need to refresh it when frameParent is show
        void refreshTagProperties();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select an group in ComboBox
        long onCmdSelectTagType(FXObject*, FXSelector, void*);

        /// @brief Called when the user select an elementin ComboBox
        long onCmdSelectTag(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(TagSelector)

    private:
        /// @brief pointer to Frame Parent
        GNEFrame* myFrameParent;

        /// @brief comboBox with tag type
        FXComboBox* myTagTypesMatchBox;

        /// @brief comboBox with the list of tags
        FXComboBox* myTagsMatchBox;

        /// @brief current tag properties
        GNETagProperties myCurrentTagProperties;

        /// @brief list of tags types that will be shown in Match Box
        std::vector<std::pair<std::string, GNETagProperties::TagType> > myListOfTagTypes;

        /// @brief list of tags that will be shown in Match Box
        std::vector<SumoXMLTag> myListOfTags;

        /// @brief dummy tag properties used if user select an invalid tag
        GNETagProperties myInvalidTagProperty;
    };

    // ===========================================================================
    // class DemandElementSelector
    // ===========================================================================

    class DemandElementSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameModuls::DemandElementSelector)

    public:
        /// @brief constructor with a single tag
        DemandElementSelector(GNEFrame* frameParent, SumoXMLTag demandElementTag);

        /// @brief constructor with tag type
        DemandElementSelector(GNEFrame* frameParent, const std::vector<GNETagProperties::TagType>& tagTypes);

        /// @brief destructor
        ~DemandElementSelector();

        /// @brief get current demand element
        GNEDemandElement* getCurrentDemandElement() const;

        // @brief obtain allowed tags (derived from tagTypes)
        const std::vector<SumoXMLTag>& getAllowedTags() const;

        /// @brief set current demand element
        void setDemandElement(GNEDemandElement* demandElement);

        /// @brief show demand element selector
        void showDemandElementSelector();

        /// @brief hide demand element selector
        void hideDemandElementSelector();

        /// @brief check if demand element selector is shown
        bool isDemandElementSelectorShown() const;

        /// @brief refresh demand element selector
        void refreshDemandElementSelector();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another demand elementelement in ComboBox
        long onCmdSelectDemandElement(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(DemandElementSelector)

    private:
        /// @brief pointer to frame Parent
        GNEFrame* myFrameParent;

        /// @brief comboBox with the list of elements type
        FXComboBox* myDemandElementsMatchBox;

        /// @brief current demand element
        GNEDemandElement* myCurrentDemandElement;

        /// @brief demand element tags
        std::vector<SumoXMLTag> myDemandElementTags;
    };

    // ===========================================================================
    // class EdgePathCreator
    // ===========================================================================

    class EdgePathCreator : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameModuls::EdgePathCreator)

    public:

        /// @brief list of the edge path creator modes
        enum Modes {
            CONSECUTIVE,    // Path must be consecutive
            FROM_TO,        // Path requires only two from-to edges
            FROM_TO_VIA,    // Path requires a from-via-to edges
            FROM_BUSSTOP,   // Path start in a BusStop
            TO_BUSSTOP      // Path ends in a BusStop
        };

        /// @brief default constructor
        EdgePathCreator(GNEFrame* frameParent, int edgePathCreatorModes);

        /// @brief destructor
        ~EdgePathCreator();

        /// @brief update EdgePathCreator name
        void edgePathCreatorName(const std::string& name);

        /// @brief show EdgePathCreator
        void showEdgePathCreator();

        /// @brief show EdgePathCreator
        void hideEdgePathCreator();

        /// @brief set SUMOVehicleClass
        void setVClass(SUMOVehicleClass vClass);

        /// @brief set Modes
        void setModes(int edgePathCreatorModes);

        /// @brief get current clicked edges
        std::vector<GNEEdge*> getClickedEdges() const;

        /// @brief get current clicked edges
        GNEAdditional* getClickedBusStop() const;

        /// @brief add edge in path
        bool addPathEdge(GNEEdge* edge);

        /// @brief add busStop to path
        bool addBusStop(GNEAdditional* busStop);

        /// @brief clear path edges (and restore colors)
        void clearEdges();

        /// @brief draw temporal route
        void drawTemporalPath() const;

        /// @brief abort edge path creation
        void abortEdgePathCreation();

        /// @brief finish edge path creation
        void finishEdgePathCreation();

        /// @brief remove last added element (either a BusStop or an edge)
        void removeLastInsertedElement();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user click over button "Abort route creation"
        long onCmdAbortRouteCreation(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Finish route creation"
        long onCmdFinishRouteCreation(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Remove las inserted edge/busStop"
        long onCmdRemoveLastInsertedElement(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(EdgePathCreator)

    private:
        /// @brief pointer to GNEFrame Parent
        GNEFrame* myFrameParent;

        /// @brief button for finish route creation
        FXButton* myFinishCreationButton;

        /// @brief button for abort route creation
        FXButton* myAbortCreationButton;

        /// @brief button for removing last inserted edge
        FXButton* myRemoveLastInsertedEdge;

        /// @brief VClass used for this trip
        SUMOVehicleClass myVClass;

        /// @brief current clicked edges
        std::vector<GNEEdge*> myClickedEdges;

        /// @brief pointer to selected busStop
        GNEAdditional* mySelectedBusStop;

        /// @brief vector with temporal route edges
        std::vector<GNEEdge*> myTemporalRoute;

        /// @brief current edge path creator modes
        int myModes;

        /// @brief restore colors of given edge
        void restoreEdgeColor(const GNEEdge* edge);
    };

    // ===========================================================================
    // class AttributeCarrierHierarchy
    // ===========================================================================

    class AttributeCarrierHierarchy : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameModuls::AttributeCarrierHierarchy)

    public:
        /// @brief constructor
        AttributeCarrierHierarchy(GNEFrame* frameParent);

        /// @brief destructor
        ~AttributeCarrierHierarchy();

        /// @brief show AttributeCarrierHierarchy
        void showAttributeCarrierHierarchy(GNEAttributeCarrier* AC);

        /// @brief hide AttributeCarrierHierarchy
        void hideAttributeCarrierHierarchy();

        /// @brief refresh AttributeCarrierHierarchy
        void refreshAttributeCarrierHierarchy();

        /// @brief if given AttributeCarrier is the same of myAC, disable it
        void removeCurrentEditedAttribute(const GNEAttributeCarrier* AC);

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user press right click over an item of list of children
        long onCmdShowChildMenu(FXObject*, FXSelector, void* data);

        /// @brief called when user click over option "center" of child Menu
        long onCmdCenterItem(FXObject*, FXSelector, void*);

        /// @brief called when user click over option "inspect" of child menu
        long onCmdInspectItem(FXObject*, FXSelector, void*);

        /// @brief called when user click over option "delete" of child menu
        long onCmdDeleteItem(FXObject*, FXSelector, void*);

        /// @brief called when user click over option "Move up" of child menu
        long onCmdMoveItemUp(FXObject*, FXSelector, void*);

        /// @brief called when user click over option "Move down" of child menu
        long onCmdMoveItemDown(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(AttributeCarrierHierarchy)

        // @brief create pop-up menu in the positions X-Y for the clicked attribute carrier
        void createPopUpMenu(int X, int Y, GNEAttributeCarrier* clickedAC);

        /// @brief show child of current attributeCarrier
        FXTreeItem* showAttributeCarrierParents();

        /// @brief show child of current attributeCarrier
        void showAttributeCarrierChildren(GNEAttributeCarrier* AC, FXTreeItem* itemParent);

        /// @brief add item into list
        FXTreeItem* addListItem(GNEAttributeCarrier* AC, FXTreeItem* itemParent = nullptr, std::string prefix = "", std::string sufix = "");

        /// @brief add item into list
        FXTreeItem* addListItem(FXTreeItem* itemParent, const std::string& text, FXIcon* icon, bool expanded);
    private:
        /// @brief Frame Parent
        GNEFrame* myFrameParent;

        /// @brief Attribute carrier
        GNEAttributeCarrier* myAC;

        /// @brief pointer to current clicked Attribute Carrier
        GNEAttributeCarrier* myClickedAC;

        /// @brief junction (casted from myClickedAC)
        GNEJunction* myClickedJunction;

        /// @brief edge (casted from myClickedAC)
        GNEEdge* myClickedEdge;

        /// @brief lane (casted from myClickedAC)
        GNELane* myClickedLane;

        /// @brief crossing (casted from myClickedAC)
        GNECrossing* myClickedCrossing;

        /// @brief junction (casted from myClickedAC)
        GNEConnection* myClickedConnection;

        /// @brief shape (casted from myClickedAC)
        GNEShape* myClickedShape;

        /// @brief additional (casted from myClickedAC)
        GNEAdditional* myClickedAdditional;

        /// @brief demand element (casted from myClickedAC)
        GNEDemandElement* myClickedDemandElement;

        /// @brief data set element (casted from myClickedAC)
        GNEDataSet* myClickedDataSet;

        /// @brief data interval element (casted from myClickedAC)
        GNEDataInterval* myClickedDataInterval;

        /// @brief generic data element (casted from myClickedAC)
        GNEGenericData* myClickedGenericData;

        /// @brief tree list to show the children of the element to erase
        FXTreeList* myTreelist;

        /// @brief map used to save the Tree items with their AC
        std::map<FXTreeItem*, GNEAttributeCarrier*> myTreeItemToACMap;

        /// @brief set used to save tree items without AC assigned, the Incoming/Outcoming connections
        std::set<FXTreeItem*> myTreeItemsConnections;
    };

    // ===========================================================================
    // class DrawingShape
    // ===========================================================================

    class DrawingShape : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameModuls::DrawingShape)

    public:
        /// @brief constructor
        DrawingShape(GNEFrame* frameParent);

        /// @brief destructor
        ~DrawingShape();

        /// @brief show Drawing mode
        void showDrawingShape();

        /// @brief hide Drawing mode
        void hideDrawingShape();

        /// @brief start drawing
        void startDrawing();

        /// @brief stop drawing and check if shape can be created
        void stopDrawing();

        /// @brief abort drawing
        void abortDrawing();

        /// @brief add new point to temporal shape
        void addNewPoint(const Position& P);

        /// @brief remove last added point
        void removeLastPoint();

        /// @brief get Temporal shape
        const PositionVector& getTemporalShape() const;

        /// @brief return true if currently a shape is drawed
        bool isDrawing() const;

        /// @brief enable or disable delete last created point
        void setDeleteLastCreatedPoint(bool value);

        /// @brief get flag delete last created point
        bool getDeleteLastCreatedPoint();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press start drawing button
        long onCmdStartDrawing(FXObject*, FXSelector, void*);

        /// @brief Called when the user press stop drawing button
        long onCmdStopDrawing(FXObject*, FXSelector, void*);

        /// @brief Called when the user press abort drawing button
        long onCmdAbortDrawing(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(DrawingShape)

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @brief flag to enable/disable delete point mode
        bool myDeleteLastCreatedPoint;

        /// @brief current drawed shape
        PositionVector myTemporalShapeShape;

        /// @brief button for start drawing
        FXButton* myStartDrawingButton;

        /// @brief button for stop drawing
        FXButton* myStopDrawingButton;

        /// @brief button for abort drawing
        FXButton* myAbortDrawingButton;

        /// @brief Label with information
        FXLabel* myInformationLabel;
    };

    // ===========================================================================
    // class SelectorParent
    // ===========================================================================

    class SelectorParent : protected FXGroupBox {
    public:
        /// @brief constructor
        SelectorParent(GNEFrame* frameParent);

        /// @brief destructor
        ~SelectorParent();

        /// @brief get currently parent additional selected
        std::string getIdSelected() const;

        /// @brief select manually a element of the list
        void setIDSelected(const std::string& id);

        /// @brief Show list of SelectorParent Modul
        bool showSelectorParentModul(SumoXMLTag additionalTypeParent);

        /// @brief hide SelectorParent Modul
        void hideSelectorParentModul();

        /// @brief Refresh list of Additional Parents Modul
        void refreshSelectorParentModul();

    private:
        /// @brief pointer to Frame Parent
        GNEFrame* myFrameParent;

        /// @brief current parent additional tag
        SumoXMLTag myParentTag;

        /// @brief Label with parent name
        FXLabel* myParentsLabel;

        /// @brief List of parents
        FXList* myParentsList;
    };

    // ===========================================================================
    // class OverlappedInspection
    // ===========================================================================

    class OverlappedInspection : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameModuls::OverlappedInspection)

    public:
        /// @brief constructor
        OverlappedInspection(GNEFrame* frameParent);

        /// @brief constructor (used for filter objects under cusor
        OverlappedInspection(GNEFrame* frameParent, const SumoXMLTag filteredTag);

        /// @brief destructor
        ~OverlappedInspection();

        /// @brief show template editor
        void showOverlappedInspection(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const Position& clickedPosition);

        /// @brief hide template editor
        void hideOverlappedInspection();

        /// @brief check if overlappedInspection modul is shown
        bool overlappedInspectionShown() const;

        /// @brief get number of overlapped ACSs
        int getNumberOfOverlappedACs() const;

        /// @brief check if given position is near to saved position
        bool checkSavedPosition(const Position& clickedPosition) const;

        /// @brief try to go to next element if clicked position is near to saved position
        bool nextElement(const Position& clickedPosition);

        /// @brief try to go to previous element if clicked position is near to saved position
        bool previousElement(const Position& clickedPosition);

        /// @name FOX-callbacks
        /// @{

        /// @brief Inspect next Element (from top to bot)
        long onCmdNextElement(FXObject*, FXSelector, void*);

        /// @brief Inspect previous element (from top to bot)
        long onCmdPreviousElement(FXObject*, FXSelector, void*);

        /// @brief show list of overlapped elements
        long onCmdShowList(FXObject*, FXSelector, void*);

        /// @brief called when a list item is selected
        long onCmdListItemSelected(FXObject*, FXSelector, void*);

        /// @brief Called when user press the help button
        long onCmdOverlappingHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        OverlappedInspection();

        /// @brief build Fox Toolkit elemements
        void buildFXElements();

    private:
        /// @brief current frame parent
        GNEFrame* myFrameParent;

        /// @brief Previous element button
        FXButton* myPreviousElement;

        /// @brief Button for current index
        FXButton* myCurrentIndexButton;

        /// @brief Next element button
        FXButton* myNextElement;

        /// @brief list of overlapped elements
        FXList* myOverlappedElementList;

        /// @brief button for help
        FXButton* myHelpButton;

        /// @brief filtered tag
        const SumoXMLTag myFilteredTag;

        /// @brief objects under cursor
        std::vector<GNEAttributeCarrier*> myOverlappedACs;

        /// @brief current index item
        size_t myItemIndex;

        /// @brief saved clicked position
        Position mySavedClickedPosition;
    };

    /// @brief build rainbow in frame modul
    static std::vector<RGBColor> buildRainbow(FXComposite* parent);
};
