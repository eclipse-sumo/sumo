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

// temporal declaration
class GNETAZ;

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
    /// @brief FOX-declaration
    FXDECLARE(GNETAZFrame)

public:

    /// @brief enum with all possible values after try to create an shape using frame
    enum AddTAZResult {
        ADDTAZ_SUCCESS,             // TAZ was successfully created
        ADDTAZ_UPDATEDTEMPORALTAZ,  // Added or removed a new point to temporal shape
        ADDTAZ_INVALID,             // TAZ wasn't created
        ADDTAZ_NOTHING              // Nothing to do
    };

    // ===========================================================================
    // class TAZSelector
    // ===========================================================================

    class TAZSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZSelector)

    public:
        /// @brief constructor
        TAZSelector(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZSelector();

        /// @brief get current TAZ type
        SumoXMLTag getCurrentTAZType() const;

        /// @brief set parameters depending of the given TAZType
        void setCurrentTAZ(SumoXMLTag actualTAZType);

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another TAZ Type
        long onCmdselectNewTAZ(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        TAZSelector() {}

    private:
        /// @brief pointer to TAZ Frame Parent
        GNETAZFrame* myTAZFrameParent;

        /// @brief combo box with the list of TAZ elements
        FXComboBox* myTAZMatchBox;

        /// @brief actual TAZ type selected in the match Box
        SumoXMLTag myCurrentTAZType;
    };

    // ===========================================================================
    // class edgesSelector
    // ===========================================================================

    class edgesSelector : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::edgesSelector)

    public:
        /// @brief constructor
        edgesSelector(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~edgesSelector();

        /// @brief get current junction
        GNEJunction* getCurrentJunction() const;

        /// @brief enable edgeSelector
        void enableEdgeSelector(GNEJunction* currentJunction);

        /// @brief disable edgeSelector
        void disableEdgeSelector();

        /// @brief restore colors of all edges
        void restoreEdgeColors();

        /// @brief return candidate color
        const RGBColor& getCandidateColor() const;

        /// @brief return selected color
        const RGBColor& getSelectedColor() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when useSelectedEdges button edge is pressed
        long onCmdUseSelectedEdges(FXObject*, FXSelector, void*);

        /// @brief called when clear selection button is pressed
        long onCmdClearSelection(FXObject*, FXSelector, void*);

        /// @brief called when invert selection button is pressed
        long onCmdInvertSelection(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        edgesSelector() {}

    private:
        /// @brief pointer to GNETAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @brief CheckBox for selected edges
        FXButton* myUseSelectedEdges;

        /// @brief button for clear selection
        FXButton* myClearEdgesSelection;

        /// @brief button for invert selection
        FXButton* myInvertEdgesSelection;

        /// @brief current Junction
        GNEJunction* myCurrentJunction;
    };

    // ===========================================================================
    // class TAZParameters
    // ===========================================================================

    class TAZParameters : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZParameters)

    public:
        /// @brief constructor
        TAZParameters(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZParameters();

        /// @brief enable TAZ parameters and set the default value of parameters
        void enableTAZParameters(bool hasTLS);

        /// @brief disable TAZ parameters and clear parameters
        void disableTAZParameters();

        /// @brief check if currently the TAZParameters is enabled
        bool isTAZParametersEnabled() const;

        /// @brief mark or dismark edge
        void markEdge(GNEEdge* edge);

        /// @brief clear edges
        void clearEdges();

        /// @brief invert edges
        void invertEdges(GNEJunction* parentJunction);

        /// @brief use selected eges
        void useSelectedEdges(GNEJunction* parentJunction);

        /// @brief get TAZ NBedges
        std::vector<NBEdge*> getTAZEdges() const;

        /// @brief get TAZ priority
        bool getTAZPriority() const;

        /// @brief get TAZ width
        double getTAZWidth() const;

        /// @brief get candidate color
        const RGBColor& getCandidateColor() const;

        /// @brief get selected color
        const RGBColor& getSelectedColor() const;

        /// @brief check if current parameters are valid
        bool isCurrentParametersValid() const;

        /// @name FOX-callbacks
        /// @{
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

        /// @brief current selected edges
        std::vector<GNEEdge*> myCurrentSelectedEdges;

        /// @brief Label for edges
        FXLabel* myTAZEdgesLabel;

        /// @brief TextField for edges
        FXTextField* myTAZEdges;

        /// @brief Label for Priority
        FXLabel* myTAZPriorityLabel;

        /// @brief CheckBox for Priority
        FXCheckButton* myTAZPriorityCheckButton;

        /// @brief Label for width
        FXLabel* myTAZWidthLabel;

        /// @brief TextField for width
        FXTextField* myTAZWidth;

        /// @brief button for help
        FXButton* myHelpTAZAttribute;

        /// @brief flag to check if current parameters are valid
        bool myCurrentParametersValid;

        /// @brief color for candidate edges
        RGBColor myCandidateColor;

        /// @brief color for selected edges
        RGBColor mySelectedColor;
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
    * @param[in] edge clicked edge
    * @return AddShapeStatus with the result of operation
    */
    AddTAZResult processClick(const Position& clickedPosition, GNEEdge* edge);

    /// @brief enable or disable button create edges
    void setCreateTAZButton(bool value);

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user press the button create edge
    long onCmdCreateTAZ(FXObject*, FXSelector, void*);
    /// @}

    /// @brief get drawing mode editor
    DrawingShape* getDrawingShape() const;

protected:
    /// @brief FOX needs this
    GNETAZFrame() {}

private:
    /// @brief TAZ Selector
    GNETAZFrame::TAZSelector* myTAZSelector;

    /// @brief TAZ parameters
    GNETAZFrame::TAZParameters* myTAZParameters;

    /// @brief Drawing shape
    DrawingShape* myDrawingShape;

    /// @brief edge selector
    GNETAZFrame::edgesSelector* myEdgeSelector;

    /// @brief groupbox for buttons
    FXGroupBox* myGroupBoxButtons;

    /// @field FXButton for create TAZ
    FXButton* myCreateTAZButton;

    /// @brief groupbox for Legend
    FXGroupBox* myGroupBoxLegend;

    /// @brief Label for color candidate
    FXLabel* myColorCandidateLabel;

    /// @brief Label for color selected
    FXLabel* myColorSelectedLabel;
};


#endif

/****************************************************************************/
