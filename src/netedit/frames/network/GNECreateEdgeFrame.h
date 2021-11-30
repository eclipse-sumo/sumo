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
    
        /// @brief default edge type
        GNEEdgeType* myDefaultEdgeType;

        /// @brief selected edgeType
        GNEEdgeType* myEdgeTypeSelected;

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

    /// @brief get edgeType selector
    EdgeTypeSelector* getEdgeTypeSelector() const;

    /// @brief get edgeType attributes
    GNEFrameAttributesModuls::AttributesCreator* getEdgeTypeAttributes() const;

    /// @brief set default to using edge template
    void setUseEdgeTemplate();

protected:
    /// @brief custom edge selector
    EdgeTypeSelector* myEdgeTypeSelector = nullptr;

    /// @brief internal edgeType attributes
    GNEFrameAttributesModuls::AttributesCreator* myEdgeTypeAttributes;

    /// @brief edge selector legend
    EdgeTypeSelectorLegend* myEdgeTypeSelectorLegend = nullptr;

private:
    /// @brief objects under snapped cursor
    GNEViewNetHelper::ObjectsUnderCursor myObjectsUnderSnappedCursor;

    /// @brief source junction for new edge
    GNEJunction* myCreateEdgeSource;
};
