/****************************************************************************/
/// @file    GNECrossingFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
/// The Widget for add Crossing elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNECrossingFrame_h
#define GNECrossingFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEFrame.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNENetElement;
class GNECrossing;
class GNEJunction;
class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECrossingFrame
 * The Widget for setting default parameters of Crossing elements
 */
class GNECrossingFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNECrossingFrame)

public:

    // ===========================================================================
    // class edgesSelector
    // ===========================================================================

    class edgesSelector : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECrossingFrame::edgesSelector)

    public:
        /// @brief constructor
        edgesSelector(FXComposite* parent, GNECrossingFrame* crossingFrameParent);

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

        /// @brief called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        edgesSelector() {}

    private:
        /// @brief pointer to GNECrossingFrame parent
        GNECrossingFrame* myCrossingFrameParent;

        /// @brief CheckBox for selected edges
        FXButton* myUseSelectedEdges;

        /// @brief button for help
        FXButton* helpEdges;

        /// @brief button for clear selection
        FXButton* myClearEdgesSelection;

        /// @brief button for invert selection
        FXButton* myInvertEdgesSelection;

        /// @brief current Junction
        GNEJunction* myCurrentJunction;
    };

    // ===========================================================================
    // class crossingParameters
    // ===========================================================================

    class crossingParameters : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECrossingFrame::crossingParameters)

    public:
        /// @brief constructor
        crossingParameters(GNECrossingFrame* crossingFrameParent, GNECrossingFrame::edgesSelector* es);

        /// @brief destructor
        ~crossingParameters();

        /// @brief enable crossing parameters and set the default value of parameters
        void enableCrossingParameters();

        /// @brief disable crossing parameters and clear parameters
        void disableCrossingParameters();

        /// @brief check if currently the crossingParameters is enabled
        bool isCrossingParametersEnabled() const;

        /// @brief mark or dismark edge
        void markEdge(GNEEdge* edge);

        /// @brief clear edges
        void clearEdges();

        /// @brief invert edges
        void invertEdges(GNEJunction* parentJunction);

        /// @brief use selected eges
        void useSelectedEdges(GNEJunction* parentJunction);

        /// @brief get crossing NBedges
        std::vector<NBEdge*> getCrossingEdges() const;

        /// @brief get crossing priority
        bool getCrossingPriority() const;

        /// @brief get crossing width
        SUMOReal getCrossingWidth() const;

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
        crossingParameters() {}

    private:
        /// @brief pointer to GNECrossingFrame parent
        GNECrossingFrame* myCrossingFrameParent;

        /// @brief pointer to edge selector
        GNECrossingFrame::edgesSelector* myEdgeSelector;

        /// @brief current selected edges
        std::vector<GNEEdge*> myCurrentSelectedEdges;

        /// @brief FXMatrix for place attributes
        FXMatrix* myAttributesMatrix;

        /// @brief Label for edges
        FXLabel* myCrossingEdgesLabel;

        /// @brief TextField for edges
        FXTextField* myCrossingEdges;

        /// @brief Label for Priority
        FXLabel* myCrossingPriorityLabel;

        /// @brief CheckBox for Priority
        FXMenuCheck* myCrossingPriority;

        /// @brief Label for width
        FXLabel* myCrossingWidthLabel;

        /// @brief TextField for width
        FXTextField* myCrossingWidth;

        /// @brief button for help
        FXButton* myHelpCrossingAttribute;

        /// @brief flag to check if current parameters are valid
        bool myCurrentParametersValid;

        /// @brief color for candidate edges
        static RGBColor myCandidateColor;

        /// @brief color for selected edges
        static RGBColor mySelectedColor;
    };


    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNECrossingFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNECrossingFrame();

    /// @brief hide crossing frame
    void hide();

    /**@brief add Crossing element
     * @param[in] netElement clicked netElement. if user didn't clicked over a GNENetElement in view, netElement will be NULL
     * @return true if a GNECrossing was added, false in other case
     */
    bool addCrossing(GNENetElement* netElement);

    /**@brief remove an Crossing element previously added
     * @param[in] Crossing element to erase
     */
    void removeCrossing(GNECrossing* Crossing);

    /// @brief enable or disable button create edges
    void setCreateCrossingButton(bool value);

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user press the button create edge
    long onCmdCreateCrossing(FXObject*, FXSelector, void*);
    /// @}

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

    /// @brief get edge selector
    GNECrossingFrame::edgesSelector* getEdgeSelector() const;

    /// @brief get Crossing parameters
    GNECrossingFrame::crossingParameters* getCrossingParameters() const;

protected:
    /// @brief FOX needs this
    GNECrossingFrame() {}

private:
    /// @brief edge selector
    GNECrossingFrame::edgesSelector* myEdgeSelector;

    /// @brief crossing parameters
    GNECrossingFrame::crossingParameters* myCrossingParameters;

    /// @brief groupbox for the junction label
    FXGroupBox* myGroupBoxLabel;

    /// @brief Label for current Junction
    FXLabel* myCurrentJunctionLabel;

    /// @brief groupbox for buttons
    FXGroupBox* myGroupBoxButtons;

    /// @field FXButton for create Crossing
    FXButton* myCreateCrossingButton;

    /// @brief groupbox for Legend
    FXGroupBox* myGroupBoxLegend;

    /// @brief Label for color candidate
    FXLabel* myColorCandidateLabel;

    /// @brief Label for color selected
    FXLabel* myColorSelectedLabel;
};


#endif

/****************************************************************************/
