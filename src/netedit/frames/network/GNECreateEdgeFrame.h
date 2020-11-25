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
/// @file    GNECreateEdgeFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
///
// The Widget for create edges (and junctions)
/****************************************************************************/
#pragma once

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
    // class EdgeSelector
    // ===========================================================================

    class EdgeSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECreateEdgeFrame::EdgeSelector)

    public:
        /// @brief constructor
        EdgeSelector(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~EdgeSelector();

        /// @brief refresh template selector
        void refreshEdgeSelector();

        /// @brief check if we have to use selector
        bool useEdgeTemplate() const;

        /// @brief getselected edgeType
        GNEEdgeType* getSelectedEdgeType() const;

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

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(EdgeSelector)

    private:
        /// @brief pointer to createEdgeFrameParent
        GNECreateEdgeFrame* myCreateEdgeFrameParent;

        /// @brief selected edgeType
        GNEEdgeType* mySelectedEdgeType;

        /// @brief create default edge
        FXRadioButton* myCreateDefaultEdge = nullptr;

        /// @brief use custom edge
        FXRadioButton* myUseCustomEdge = nullptr;

        /// @brief ComboBox for edge types
        FXComboBox* myEdgeTypesComboBox = nullptr;

        /// @brief button for create new edge type
        FXButton* myNewEdgeTypeButton;

        /// @brief button for delete edge type
        FXButton* myDeleteEdgeTypeButton;
    };

    // ===========================================================================
    // class EdgeTypeParameters
    // ===========================================================================

    class EdgeTypeParameters : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECreateEdgeFrame::EdgeTypeParameters)

    public:
        /// @brief constructor
        EdgeTypeParameters(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~EdgeTypeParameters();

        /// @brief show edge parameters
        void showEdgeTypeParameters();

        /// @brief hide edge parameters
        void hideEdgeTypeParameters();

        /// @brief enable edge parameters
        void enableEdgeTypeParameters();

        /// @brief disable edge parameters
        void disableEdgeTypeParameters();

        /// @brief set edgeType
        void setEdgeType(GNEEdgeType* edgeType);

        /// @brief set default values
        void setDefaultValues();

        /// @brief set template values
        void setTemplateValues();

        /// @brief set current attributes
        void setCurrentEdgeTypeAttributesInEdge(GNEEdge* edge, GNEUndoList* undoList) const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user change value
        long onCmdSetAttribute(FXObject* obj, FXSelector, void*);

        /// @brief Called when the user open attribute dialog
        long onCmdOpenAttributeDialog(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(EdgeTypeParameters)

        /// @brief fill default parameters
        void fillDefaultParameters();

        /// @brief set attribute for default parameters
        void setAttributeDefaultParameters(FXObject* obj);

        /// @brief set attribute for existent edge type
        void setAttributeExistentEdgeType(FXObject* obj);

    private:
        /// @brief pointer to createEdgeFrameParent
        GNECreateEdgeFrame* myCreateEdgeFrameParent;

        // @brief label frame for ID/template
        FXLabel* myLabelID;

        /// @brief textField for id
        FXTextField* myID = nullptr;

        /// @brief textField for numLanes
        FXTextField* myNumLanes = nullptr;

        /// @brief textField for speed
        FXTextField* mySpeed = nullptr;

        /// @brief Button for allow vehicles
        FXButton* myAllowButton = nullptr;

        /// @brief textField for allow vehicles
        FXTextField* myAllow = nullptr;

        /// @brief Button for disallow vehicles
        FXButton* myDisallowButton = nullptr;

        /// @brief textField for disallow vehicles
        FXTextField* myDisallow = nullptr;

        /// @brief textField for width
        FXTextField* myWidth = nullptr;

        /// @brief textField for Parameters
        FXTextField* myParameters = nullptr;

        /// @brief map with edge parameters
        std::map<SumoXMLAttr, std::string> myEdgeAttributes;
    };

    // ===========================================================================
    // class LaneTypeParameters
    // ===========================================================================

    class LaneTypeParameters : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECreateEdgeFrame::LaneTypeParameters)

    public:
        /// @brief constructor
        LaneTypeParameters(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~LaneTypeParameters();

        /// @brief show lane parameters
        void showLaneTypeParameters();

        /// @brief hide lane parameters
        void hideLaneTypeParameters();

        /// @brief enable lane parameters
        void enableLaneTypeParameters();

        /// @brief disable lane parameters
        void disableLaneTypeParameters();

        /// @brief set default values
        void setDefaultValues();

        /// @brief set attributes
        void setAttributes(GNEEdge* edge, GNEUndoList* undoList) const;

        /// @brief update numLanes
        void updateNumLanes(int numLanes);

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user change value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when the user open attribute dialog
        long onCmdOpenAttributeDialog(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(LaneTypeParameters)

        /// @brief fill default parameters
        void fillDefaultParameters(int laneIndex);

    private:
        /// @brief pointer to createEdgeFrameParent
        GNECreateEdgeFrame* myCreateEdgeFrameParent;

        /// @brief ComboBox for laneIndex
        FXComboBox* myLaneIndex = nullptr;

        /// @brief textField for speed
        FXTextField* mySpeed = nullptr;

        /// @brief Button for allow vehicles
        FXButton* myAllowButton = nullptr;

        /// @brief textField for allow vehicles
        FXTextField* myAllow = nullptr;

        /// @brief Button for disallow vehicles
        FXButton* myDisallowButton = nullptr;

        /// @brief textField for disallow vehicles
        FXTextField* myDisallow = nullptr;

        /// @brief textField for width
        FXTextField* myWidth = nullptr;

        /// @brief ComboBox for endOffset
        FXComboBox* myEndOffset = nullptr;

        /// @brief checkBox for acceleration
        FXCheckButton* myAcceleration = nullptr;

        /// @brief textField for opposite
        FXTextField* myOpposite = nullptr;

        /// @brief map with lane parameters
        std::map<int, std::map<SumoXMLAttr, std::string> > myLaneAttributes;
    };

    // ===========================================================================
    // class EdgeSelectorLegend
    // ===========================================================================

    class EdgeSelectorLegend : protected FXGroupBox {

    public:
        /// @brief constructor
        EdgeSelectorLegend(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~EdgeSelectorLegend();
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

    /// @brief getcustom edge selector
    EdgeSelector* getEdgeSelector() const;

protected:
    /// @brief edge parameters
    EdgeTypeParameters* myEdgeTypeParameters = nullptr;

    /// @brief lane parameters
    LaneTypeParameters* myLaneTypeParameters = nullptr;

    /// @brief custom edge selector
    EdgeSelector* myEdgeSelector = nullptr;

    /// @brief edge selector legend
    EdgeSelectorLegend* myEdgeSelectorLegend = nullptr;

private:
    /// @brief objects under snapped cursor
    GNEViewNetHelper::ObjectsUnderCursor myObjectsUnderSnappedCursor;

    /// @brief source junction for new edge
    GNEJunction* myCreateEdgeSource;
};
