/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEStopFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
/// @version $Id$
///
// The Widget for add Stops elements
/****************************************************************************/
#ifndef GNEStopFrame_h
#define GNEStopFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEStopFrame
 */
class GNEStopFrame : public GNEFrame {

public:

    // ===========================================================================
    // class StopParentSelector
    // ===========================================================================

    class StopParentSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEStopFrame::StopParentSelector)

    public:
        /// @brief constructor
        StopParentSelector(GNEStopFrame* StopFrameParent);

        /// @brief destructor
        ~StopParentSelector();

        /// @brief get current route
        GNEDemandElement* getCurrentStopParent() const;

        /// @brief select StopParent manually
        void setStopParent(GNEDemandElement* stopParent);

        /// @brief show VType selector
        void showStopParentSelector();

        /// @brief hide VType selector
        void hideStopParentSelector();

        /// @brief refresh VType selector
        void refreshStopParentSelector();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another element in ComboBox
        long onCmdSelectStopParent(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        StopParentSelector() {}

    private:
        /// @brief pointer to Stop Frame Parent
        GNEStopFrame* myStopFrameParent;

        /// @brief comboBox with the list of routes
        FXComboBox* myStopParentMatchBox;

        /// @brief current stop parent
        GNEDemandElement* myCurrentStopParent;

        /// @brief list of posible candidates
        std::vector<GNEDemandElement*> myStopParentCandidates;
    };

    // ===========================================================================
    // class HelpCreation
    // ===========================================================================

    class HelpCreation : protected FXGroupBox {

    public:
        /// @brief constructor
        HelpCreation(GNEStopFrame* StopFrameParent);

        /// @brief destructor
        ~HelpCreation();

        /// @brief show HelpCreation
        void showHelpCreation();

        /// @brief hide HelpCreation
        void hideHelpCreation();

        /// @brief update HelpCreation
        void updateHelpCreation();

    private:
        /// @brief pointer to Stop Frame Parent
        GNEStopFrame* myStopFrameParent;

        /// @brief Label with creation information
        FXLabel* myInformationLabel;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEStopFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEStopFrame();

    /// @brief show Frame
    void show();

    /**@brief add Stop element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @param shiftPressed flag to check if during clicking shift key was pressed
     * @return true if Stop was sucesfully added
     */
    bool addStop(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, bool shiftPressed);

protected:
    /// @brief Tag selected in TagSelector
    void tagSelected();

private:
    /// @brief Stop parent selectors
    StopParentSelector* myStopParentSelector;

    /// @brief stop tag selector selector (used to select diffent kind of Stops)
    TagSelector* myStopTagSelector;

    /// @brief internal Stop attributes
    AttributesCreator* myStopAttributes;

    /// @brief Netedit parameter
    NeteditAttributes* myNeteditAttributes;

    /// @brief Help creation
    HelpCreation* myHelpCreation;
};


#endif

/****************************************************************************/
