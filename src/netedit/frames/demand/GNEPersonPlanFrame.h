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
/// @file    GNEPersonPlanFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// The Widget for add PersonPlan elements
/****************************************************************************/
#pragma once

#include <netedit/frames/GNEFrame.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEPersonPlanFrame
 */
class GNEPersonPlanFrame : public GNEFrame {

public:

    // ===========================================================================
    // class HelpCreation
    // ===========================================================================

    class HelpCreation : protected FXGroupBox {

    public:
        /// @brief constructor
        HelpCreation(GNEPersonPlanFrame* vehicleFrameParent);

        /// @brief destructor
        ~HelpCreation();

        /// @brief show HelpCreation
        void showHelpCreation();

        /// @brief hide HelpCreation
        void hideHelpCreation();

        /// @brief update HelpCreation
        void updateHelpCreation();

    private:
        /// @brief pointer to PersonPlan Frame Parent
        GNEPersonPlanFrame* myPersonPlanFrameParent;

        /// @brief Label with creation information
        FXLabel* myInformationLabel;
    };

    // ===========================================================================
    // class PersonPlanCreator
    // ===========================================================================

    class PersonPlanCreator : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEPersonPlanFrame::PersonPlanCreator)

    public:
        /// @brief default constructor
        PersonPlanCreator(GNEPersonPlanFrame* frameParent);

        /// @brief destructor
        ~PersonPlanCreator();

        /// @brief update PersonPlanCreator name
        void edgePathCreatorName(const std::string& name);

        /// @brief show PersonPlanCreator
        void showPersonPlanCreator();

        /// @brief show PersonPlanCreator
        void hidePersonPlanCreator();

        /// @brief show PersonPlanCreator
        void refreshPersonPlanCreator();

        /// @brief get clicked edges
        std::vector<GNEEdge*> getClickedEdges() const;

        /// @brief get current edge path
        const std::vector<GNEEdge*>& getEdgePath() const;

        /// @brief add edge to route
        bool addEdge(GNEEdge* edge);

        /// @brief draw temporal route
        void drawTemporalRoute() const;

        /// @brief abort person plan creation
        void abortPersonPlanCreation();

        /// @brief finish person plan creation
        void finishPersonPlanCreation();

        /// @brief remove last added element
        void removeLastAddedElement();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user click over button "Abort route creation"
        long onCmdAbortPersonPlanCreation(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Finish route creation"
        long onCmdFinishPersonPlanCreation(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Remove last inserted edge"
        long onCmdRemoveLastElement(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(PersonPlanCreator)

    private:
        /// @brief pointer to Person Plan Frame Parent
        GNEPersonPlanFrame* myPersonPlanFrameParent;

        /// @brief Label with current begin element
        FXLabel* myCurrentBeginElementLabel;

        /// @brief button for finish route creation
        FXButton* myFinishCreationButton;

        /// @brief button for abort route creation
        FXButton* myAbortCreationButton;

        /// @brief button for removing last edge
        FXButton* myRemoveLastEdge;

        /// @brief current clicked edges
        std::vector<GNEEdge*> myClickedEdges;

        /// @brief vector with temporal edge path
        std::vector<GNEEdge*> myTemporalEdgePath;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEPersonPlanFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEPersonPlanFrame();

    /// @brief show Frame
    void show();

    /// @brief hide Frame
    void hide();

    /**@brief add vehicle element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if vehicle was sucesfully added
     */
    bool addPersonPlan(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief get PersonPlanCreator modul
    PersonPlanCreator* getPersonPlanCreator() const;

protected:
    /// @brief Tag selected in TagSelector
    void tagSelected();

    /// @brief selected demand element in DemandElementSelector
    void demandElementSelected();

    /// @brief finish person plan creation
    bool personPlanCreated(GNEAdditional* busStop, GNEDemandElement* route);

private:
    /// @brief Person selectors
    GNEFrameModuls::DemandElementSelector* myPersonSelector;

    /// @brief personPlan selector
    GNEFrameModuls::TagSelector* myPersonPlanTagSelector;

    /// @brief internal vehicle attributes
    GNEFrameAttributesModuls::AttributesCreator* myPersonPlanAttributes;

    /// @brief Person Plan Creator
    PersonPlanCreator* myPersonPlanCreator;

    /// @brief Person Hierarchy
    GNEFrameModuls::AttributeCarrierHierarchy* myPersonHierarchy;

    /// @brief Help creation
    HelpCreation* myHelpCreation;
};
