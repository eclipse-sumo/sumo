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

    /**@brief add TAZ element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if a GNETAZ was added, false in other case
     */
    bool addTAZ(const GNEViewNet::ObjectsUnderCursor &objectsUnderCursor);

    /**@brief remove an TAZ element previously added
     * @param[in] TAZ element to erase
     */
    void removeTAZ(GNETAZ* TAZ);

    /// @brief enable or disable button create edges
    void setCreateTAZButton(bool value);

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user press the button create edge
    long onCmdCreateTAZ(FXObject*, FXSelector, void*);
    /// @}

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

    /// @brief get edge selector
    GNETAZFrame::edgesSelector* getEdgeSelector() const;

    /// @brief get TAZ parameters
    GNETAZFrame::TAZParameters* getTAZParameters() const;

protected:
    /// @brief FOX needs this
    GNETAZFrame() {}

private:
    /// @brief edge selector
    GNETAZFrame::edgesSelector* myEdgeSelector;

    /// @brief TAZ parameters
    GNETAZFrame::TAZParameters* myTAZParameters;

    /// @brief groupbox for the junction label
    FXGroupBox* myGroupBoxLabel;

    /// @brief Label for current Junction
    FXLabel* myCurrentJunctionLabel;

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
