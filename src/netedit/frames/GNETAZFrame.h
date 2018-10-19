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

public:

    /// @brief enum with all possible values after try to create an shape using frame
    enum AddTAZResult {
        ADDTAZ_SUCCESS,             // TAZ was successfully created
        ADDTAZ_UPDATEDTEMPORALTAZ,  // Added or removed a new point to temporal shape
        ADDTAZ_INVALID,             // TAZ wasn't created
        ADDTAZ_NOTHING              // Nothing to do
    };

    // ===========================================================================
    // class CurrentTAZ
    // ===========================================================================

    class CurrentTAZ : public FXGroupBox {

    public:
        /// @brief constructor
        CurrentTAZ(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~CurrentTAZ();

        /// @brief set current TAZ
        void setCurrentTAZ(GNETAZ* currentTAZ);

        /// @brief get current TAZ
        GNETAZ* getCurrentTAZ() const;

    private:
        /// @brief pointer to TAZ Frame
        GNETAZFrame* myTAZFrameParent;

        /// @brief current TAZ
        GNETAZ* myCurrentTAZ;

        /// @brief Label for current TAZ
        FXLabel* myCurrentTAZLabel;
    };

    // ===========================================================================
    // class EdgesSelector
    // ===========================================================================

    class EdgesSelector : public FXGroupBox {
        /*
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::EdgesSelector)
        */
    public:
        /// @brief constructor
        EdgesSelector(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~EdgesSelector();

        /// @brief get current junction
        GNEJunction* getCurrentJunction() const;

        /// @brief enable edgeSelector
        void enableEdgeSelector(GNEJunction* currentJunction);

        /// @brief disable edgeSelector
        void disableEdgeSelector();

        /// @brief restore colors of all edges
        void restoreEdgeColors();

        /// @name FOX-callbacks
        /// @{
        /// @brief called when useSelectedEdges button edge is pressed
        long onCmdUseSelectedEdges(FXObject*, FXSelector, void*);

        /// @brief called when clear selection button is pressed
        long onCmdClearSelection(FXObject*, FXSelector, void*);

        /// @brief called when invert selection button is pressed
        long onCmdInvertSelection(FXObject*, FXSelector, void*);
        /// @}
    /*
    protected:
        /// @brief FOX needs this
        EdgesSelector() {}
    */
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

    class TAZParameters : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZFrame::TAZParameters)

    public:
        /// @brief constructor
        TAZParameters(GNETAZFrame* TAZFrameParent);

        /// @brief destructor
        ~TAZParameters();

        /// @brief show TAZ parameters and set the default value of parameters
        void showTAZParametersModul();

        /// @brief hide TAZ parameters
        void hideTAZParametersModul();

        /// @brief check if current parameters are valid
        bool isCurrentParametersValid() const;

        /// @brief get a map with attributes and their values
        std::map<SumoXMLAttr, std::string> getAttributesAndValues() const;

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
        /// @brief FOX needs this
        TAZParameters() {}

    private:
        /// @brief pointer to GNETAZFrame parent
        GNETAZFrame* myTAZFrameParent;

        /// @brief Button for open color editor
        FXButton* myColorEditor;

        /// @brief textField to modify the default value of color parameter
        FXTextField* myTextFieldColor;

        /// @brief button for help
        FXButton* myHelpTAZAttribute;
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

    /// @brief get drawing mode editor
    DrawingShape* getDrawingShape() const;

    /// @brief get Current TAZ
    CurrentTAZ* getCurrentTAZ() const;

protected:
    /**@brief build a shaped element using the drawed shape
     * return true if was sucesfully created
     * @note called when user stop drawing shape
     */
    bool buildShape();

     /// @brief enable moduls depending of item selected in ItemSelector
    void enableModuls(const GNEAttributeCarrier::TagValues &tagValue);

    /// @brief disable moduls if element selected in itemSelector isn't valid
    void disableModuls();

private:
    /// @brief current TAZ
    CurrentTAZ* myCurrentTAZ;

    /// @brief TAZ parameters
    TAZParameters* myTAZParameters;

    /// @brief Netedit parameter
    NeteditAttributes* myNeteditAttributes;

    /// @brief Drawing shape
    DrawingShape* myDrawingShape;

    /// @brief edge selector
    EdgesSelector* myEdgeSelector;
};


#endif

/****************************************************************************/
