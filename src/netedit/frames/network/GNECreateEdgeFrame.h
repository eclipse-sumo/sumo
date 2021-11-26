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

    class EdgeTypeSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECreateEdgeFrame::EdgeTypeSelector)

    public:
        /// @brief constructor
        EdgeTypeSelector(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~EdgeTypeSelector();

        /// @brief refresh edge type selector
        void refreshEdgeTypeSelector(const bool show = false);

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

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press a radio button
        long onCmdRadioButton(FXObject*, FXSelector, void*);

        /// @brief Called when the user press button for add a new edge type
        long onCmdAddEdgeType(FXObject*, FXSelector, void*);

        /// @brief Called when the user press button for delete edge type
        long onCmdDeleteEdgeType(FXObject*, FXSelector, void*);

        /// @brief Called when the user press button for reset edge type
        long onCmdResetEdgeType(FXObject*, FXSelector, void*);

        /// @brief Called when the user press select an edgeType in comboBox
        long onCmdSelectEdgeType(FXObject*, FXSelector, void*);

        /// @}

        void useTemplate();

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(EdgeTypeSelector);

        /// @brief fill comboBox
        void fillComboBox();

        /// @brief fill default parameters
        void fillDefaultParameters();

    private:
        /// @brief pointer to createEdgeFrameParent
        GNECreateEdgeFrame* myCreateEdgeFrameParent;

        /// @brief selected edgeType
        GNEEdgeType* myEdgeTypeSelected;

        /// @brief default edge type
        GNEEdgeType* myDefaultEdgeType;

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

        /// @brief button for reset edge type
        FXButton* myResetEdgeTypeButton = nullptr;
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

        /// @brief refresh lane paramters
        void refreshLaneTypeParameters();

        /// @brief show lane parameters
        void showLaneTypeParameters();

        /// @brief hide lane parameters
        void hideLaneTypeParameters();

        /// @brief enable lane parameters
        void enableLaneTypeParameters();

        /// @brief disable lane parameters
        void disableLaneTypeParameters();

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

        /// @brief Called when the user press button for delete lane type
        long onCmdDeleteLaneType(FXObject*, FXSelector, void*);

        /// @brief Called when the user press button for reset lane type
        long onCmdResetLaneType(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(LaneTypeParameters);

        /// @brief set attribute for default parameters
        void setAttributeDefaultParameters(FXObject* obj);

        /// @brief set attribute for existent lane type
        void setAttributeExistentLaneType(FXObject* obj);

    private:
        /// @brief pointer to createEdgeFrameParent
        GNECreateEdgeFrame* myCreateEdgeFrameParent;

        /// @brief button for delete lane type
        FXButton* myDeleteLaneTypeButton = nullptr;

        /// @brief button for reset lane type
        FXButton* myResetLaneTypeButton = nullptr;

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

        /// @brief Button for edit generic parameters
        FXButton* myParametersButton = nullptr;

        /// @brief textField for Parameters
        FXTextField* myParameters = nullptr;
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
        void setEdgeType(GNEEdgeType* edgeType, bool showID);

        /// @brief set template values
        void setTemplateValues();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user change value
        long onCmdSetAttribute(FXObject* obj, FXSelector, void*);

        /// @brief Called when the user open attribute dialog
        long onCmdOpenAttributeDialog(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(EdgeTypeParameters);

        /// @brief set attribute for default parameters
        void setAttributeDefaultParameters(FXObject* obj);

        /// @brief set attribute for existent edge type
        void setAttributeExistentEdgeType(FXObject* obj);

    private:
        /// @brief pointer to createEdgeFrameParent
        GNECreateEdgeFrame* myCreateEdgeFrameParent;

        /// @brief horizontal frame for ID
        FXHorizontalFrame* myHorizontalFrameID;

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

        /// @brief comboBox for spreadType
        FXComboBox* mySpreadType = nullptr;

        /// @brief textField for width
        FXTextField* myWidth = nullptr;

        /// @brief textField for priority
        FXTextField* myPriority = nullptr;

        /// @brief Button for edit generic parameters
        FXButton* myParametersButton = nullptr;

        /// @brief textField for Parameters
        FXTextField* myParameters = nullptr;

        /// @brief lane parameters
        LaneTypeParameters* myLaneTypeParameters = nullptr;
    };

    // ===========================================================================
    // class EdgeTypeSelectorLegend
    // ===========================================================================

    class EdgeTypeSelectorLegend : protected FXGroupBox {

    public:
        /// @brief constructor
        EdgeTypeSelectorLegend(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~EdgeTypeSelectorLegend();
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
    EdgeTypeSelector* getEdgeTypeSelector() const;

    /// @brief set default to using edge template
    void setUseEdgeTemplate();

protected:
    /// @brief custom edge selector
    EdgeTypeSelector* myEdgeTypeSelector = nullptr;

    /// @brief internal edgeType attributes
    GNEFrameAttributesModuls::AttributesCreator* myEdgeTypeAttributes;

    /// @brief edge parameters
    EdgeTypeParameters* myEdgeTypeParameters = nullptr;

    /// @brief edge selector legend
    EdgeTypeSelectorLegend* myEdgeTypeSelectorLegend = nullptr;

private:
    /// @brief objects under snapped cursor
    GNEViewNetHelper::ObjectsUnderCursor myObjectsUnderSnappedCursor;

    /// @brief source junction for new edge
    GNEJunction* myCreateEdgeSource;
};
