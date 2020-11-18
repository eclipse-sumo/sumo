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
// class definitions
// ===========================================================================
/**
 * @class GNECreateEdgeFrame
 * The Widget for create edges
 */
class GNECreateEdgeFrame : public GNEFrame {

public:

    // ===========================================================================
    // class TemplateSelector
    // ===========================================================================

    class TemplateSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECreateEdgeFrame::TemplateSelector)

    public:
        /// @brief constructor
        TemplateSelector(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~TemplateSelector();

        /// @brief refresh template selector
        void refreshTemplateSelector();

        /// @brief check if we have to use selector
        bool useEdgeTemplate() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press a radio button
        long onCmdRadioButton(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(TemplateSelector)

    private:
        /// @brief pointer to createEdgeFrameParent
        GNECreateEdgeFrame* myCreateEdgeFrameParent;

        /// @brief create default edge
        FXRadioButton* myCreateDefaultEdge = nullptr;

        /// @brief use custom edge
        FXRadioButton* myUseCustomEdge = nullptr;

        /// @brief ComboBox for edge types
        FXComboBox* myEdgeTypesComboBox = nullptr;
    };

    // ===========================================================================
    // class EdgeParameters
    // ===========================================================================

    class EdgeParameters : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECreateEdgeFrame::EdgeParameters)

    public:
        /// @brief constructor
        EdgeParameters(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~EdgeParameters();

        /// @brief show edge parameters
        void showEdgeParameters();

        /// @brief hide edge parameters
        void hideEdgeParameters();

        /// @brief set attributes
        void setAttributes(GNEEdge* edge, GNEUndoList *undoList) const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user change value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when the user open attribute dialog
        long onCmdOpenAttributeDialog(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(EdgeParameters)

        /// @brief fill default parameters
        void fillDefaultParameters();

    private:
        /// @brief pointer to createEdgeFrameParent
        GNECreateEdgeFrame* myCreateEdgeFrameParent;

        /// @brief textField for speed
        FXTextField* mySpeed = nullptr;

        /// @brief textField for priority
        FXTextField* myPriority = nullptr;

        /// @brief textField for numLanes
        FXTextField* myNumLanes = nullptr;

        /// @brief textField for Type
        FXTextField* myType = nullptr;

        /// @brief Button for allow vehicles
        FXButton* myAllowButton = nullptr;

        /// @brief textField for allow vehicles
        FXTextField* myAllow = nullptr;

        /// @brief Button for disallow vehicles
        FXButton* myDisallowButton = nullptr;

        /// @brief textField for disallow vehicles
        FXTextField* myDisallow = nullptr;

        /// @brief ComboBox for spread type
        FXComboBox* mySpreadType = nullptr;

        /// @brief textField for name
        FXTextField* myName = nullptr;

        /// @brief textField for width
        FXTextField* myWidth = nullptr;

        /// @brief textField for distance
        FXTextField* myDistance = nullptr;

        /// @brief map with edge parameters
        std::map<SumoXMLAttr, std::string> myEdgeAttributes;
    };

    // ===========================================================================
    // class LaneParameters
    // ===========================================================================

    class LaneParameters : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECreateEdgeFrame::LaneParameters)

    public:
        /// @brief constructor
        LaneParameters(GNECreateEdgeFrame* createEdgeFrameParent);

        /// @brief destructor
        ~LaneParameters();

        /// @brief show lane parameters
        void showLaneParameters();

        /// @brief hide lane parameters
        void hideLaneParameters();

        /// @brief set attributes
        void setAttributes(GNEEdge* edge, GNEUndoList *undoList) const;

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
        FOX_CONSTRUCTOR(LaneParameters)

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
    // class EdgeTypeFile
    // ===========================================================================

    class EdgeTypeFile : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECreateEdgeFrame::EdgeTypeFile)

    public:
        /// @brief constructor
        EdgeTypeFile(GNECreateEdgeFrame* TLSEditorParent);

        /// @brief destructor
        ~EdgeTypeFile();

        /// @name FOX-callbacks
        /// @{
        /// @brief load TLS Program from an additional file
        long onCmdLoadEdgeProgram(FXObject*, FXSelector, void*);

        /// @brief save TLS Programm to an additional file
        long onCmdSaveEdgeProgram(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(EdgeTypeFile)

    private:
        /// @brief pointer to create edge frame Parent
        GNECreateEdgeFrame* myCreateEdgeFrame;

        /// @brief button for load TLS Programs
        FXButton* myLoadEdgeProgramButton;

        /// @brief button for save TLS Programs
        FXButton* mySaveEdgeProgramButton;
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
    TemplateSelector* getTemplateSelector() const;

protected:
    /// @brief edge parameters
    EdgeParameters* myEdgeParameters = nullptr;

    /// @brief lane parameters
    LaneParameters* myLaneParameters = nullptr;

    /// @brief custom edge selector
    TemplateSelector* myTemplateSelector = nullptr;

    /// @brief edge type file
    EdgeTypeFile* myEdgeTypeFile = nullptr;

    /// @brief edge selector legend
    EdgeSelectorLegend* myEdgeSelectorLegend = nullptr;

private:
    /// @brief objects under snapped cursor
    GNEViewNetHelper::ObjectsUnderCursor myObjectsUnderSnappedCursor;

    /// @brief source junction for new edge
    GNEJunction* myCreateEdgeSource;
};
