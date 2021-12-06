/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNECreateEdgeFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
///
// The Widget for create edges (and junctions)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>


// ===========================================================================
// class declaration
// ===========================================================================

class GNEEdgeType;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECreateEdgeFrame
 * The Widget for create edges
 */
class GNECreateEdgeFrame : public GNEFrame {

public:

    // ===========================================================================
    // class EdgeTypeSelector
    // ===========================================================================

    class EdgeTypeSelector : public FXGroupBoxModul {
        /// @brief FOX-declaration
        FXDECLARE(GNECreateEdgeFrame::EdgeTypeSelector)

    public:
        /// @brief constructor
        EdgeTypeSelector(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~EdgeTypeSelector();

        /// @brief refresh edge type selector
        void refreshEdgeTypeSelector();

        /// @brief update id in comboBox
        void updateIDinComboBox(const std::string &oldID, const std::string &newID);

        /// @brief check if we have to use edge template
        bool useEdgeTemplate() const;

        /// @brief check if we're using default edge type
        bool useDefaultEdgeType() const;

        /// @brief get default edgeType
        GNEEdgeType* getDefaultEdgeType() const;

        /// @brief get edgeType selected
        GNEEdgeType* getEdgeTypeSelected() const;

        /// @brief clear edgeType selected
        void clearEdgeTypeSelected();

        /// @brief set current edgeType
        void setCurrentEdgeType(const GNEEdgeType* edgeType);

        /// @brief use template
        void useTemplate();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press a radio button
        long onCmdRadioButton(FXObject*, FXSelector, void*);

        /// @brief Called when the user press button for add a new edge type
        long onCmdAddEdgeType(FXObject*, FXSelector, void*);

        /// @brief Called when the user press button for delete edge type
        long onCmdDeleteEdgeType(FXObject*, FXSelector, void*);

        /// @brief Called when the user press select an edgeType in comboBox
        long onCmdSelectEdgeType(FXObject*, FXSelector, void*);

        /// @brief Called when the user press create edgeType from Template
        long onCmdCreateFromTemplate(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(EdgeTypeSelector);

        /// @brief fill comboBox
        void fillComboBox();

    private:
        /// @brief pointer to createEdgeFrameParent
        GNECreateEdgeFrame* myCreateEdgeFrameParent;
    
        /// @brief default edge type
        GNEEdgeType* myDefaultEdgeType;

        /// @brief selected edgeType
        GNEEdgeType* myEdgeTypeSelected = nullptr;

        /// @brief currentIndex
        int myCurrentIndex;

        /// @brief create default edge
        FXRadioButton* myUseDefaultEdgeType = nullptr;

        /// @brief create custom edge
        FXRadioButton* myUseCustomEdgeType = nullptr;

        /// @brief ComboBox for edge types
        FXComboBox* myEdgeTypesComboBox = nullptr;

        /// @brief button for create new edge type
        FXButton* myAddEdgeTypeButton = nullptr;

        /// @brief button for delete edge type
        FXButton* myDeleteEdgeTypeButton = nullptr;

        /// @brief button for create edgeType from template
        FXButton* myCreateFromTemplate = nullptr;
    };

    // ===========================================================================
    // class LaneTypeSelector
    // ===========================================================================

    class LaneTypeSelector : public FXGroupBoxModul {
        /// @brief FOX-declaration
        FXDECLARE(GNECreateEdgeFrame::LaneTypeSelector)

    public:
        /// @brief constructor
        LaneTypeSelector(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~LaneTypeSelector();

        /// @brief show lane type selector
        void showLaneTypeSelector();

        /// @brief hide lane type selector
        void hideLaneTypeSelector();

        /// @brief refresh LaneTypeSelector
        void refreshLaneTypeSelector();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press button for add a new lane type
        long onCmdAddLaneType(FXObject*, FXSelector, void*);

        /// @brief Called when the user press button for delete lane type
        long onCmdDeleteLaneType(FXObject*, FXSelector, void*);

        /// @brief Called when the user press select an laneType in comboBox
        long onCmdSelectLaneType(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(LaneTypeSelector);

        /// @brief update comboBox
        void updateComboBox();

    private:
        /// @brief pointer to createEdgeFrameParent
        GNECreateEdgeFrame* myCreateEdgeFrameParent;
    
        /// @brief lane index
        int myLaneIndex;
        /// @brief ComboBox for lane types
        FXComboBox* myLaneTypesComboBox = nullptr;

        /// @brief button for create new lane type
        FXButton* myAddLaneTypeButton = nullptr;

        /// @brief button for delete lane type
        FXButton* myDeleteLaneTypeButton = nullptr;
    };

    // ===========================================================================
    // class Legend
    // ===========================================================================

    class Legend : public FXGroupBoxModul {

    public:
        /// @brief constructor
        Legend(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~Legend();
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNECreateEdgeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNECreateEdgeFrame();

    /**@brief handle processClick and set the relative colouring
     * @param[in] clickedPosition clicked position over ViewNet
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @param oppositeEdge automatically create an opposite edge
     * @param chainEdge create edges in chain mode
     */
    void processClick(const Position& clickedPosition, const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const bool oppositeEdge, const bool chainEdge);

    /// @brief abort current edge creation
    void abortEdgeCreation();

    /// @brief get junction source for new edge
    const GNEJunction* getJunctionSource() const;

    /// @brief update objects under snapped cursor
    void updateObjectsUnderSnappedCursor(const std::vector<GUIGlObject*>& GUIGlObjects);

    /// @brief show create edge frame
    void show();

    /// @brief hide create edge frame
    void hide();

    /// @brief get edgeType selector
    EdgeTypeSelector* getEdgeTypeSelector() const;

    /// @brief get edgeType attributes
    GNEFrameAttributesModuls::AttributesCreator* getEdgeTypeAttributes() const;

    /// @brief get lane type selector
    LaneTypeSelector* getLaneTypeSelector();

    /// @brief get laneType attributes
    GNEFrameAttributesModuls::AttributesCreator* getLaneTypeAttributes() const;

    /// @brief set default to using edge template
    void setUseEdgeTemplate();

protected:
    /// @brief edge type selector
    EdgeTypeSelector* myEdgeTypeSelector = nullptr;

    /// @brief internal edgeType attributes
    GNEFrameAttributesModuls::AttributesCreator* myEdgeTypeAttributes = nullptr;

    /// @brief lane type selector
    GNECreateEdgeFrame::LaneTypeSelector* myLaneTypeSelector = nullptr;

    /// @brief internal laneType attributes
    GNEFrameAttributesModuls::AttributesCreator* myLaneTypeAttributes = nullptr;

    /// @brief Legend
    GNECreateEdgeFrame::Legend* myLegend = nullptr;

private:
    /// @brief objects under snapped cursor
    GNEViewNetHelper::ObjectsUnderCursor myObjectsUnderSnappedCursor;

    /// @brief source junction for new edge
    GNEJunction* myCreateEdgeSource;
};
