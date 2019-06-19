/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPersonPlanFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
/// @version $Id$
///
// The Widget for add PersonPlan elements
/****************************************************************************/
#ifndef GNEPersonPlanFrame_h
#define GNEPersonPlanFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"


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
        void edgePathCreatorName(const std::string &name);

        /// @brief show PersonPlanCreator
        void showPersonPlanCreator();

        /// @brief show PersonPlanCreator
        void hidePersonPlanCreator();

        /// @brief get current selected edgesm
        std::vector<GNEEdge*> getSelectedEdges() const;

        /// @brief add edge to route
        bool addEdge(GNEEdge* edge);

        /// @brief add busStop to route
        bool addBusStop(GNEAdditional* busStop);

        /// @brief clear edges (and restore colors)
        void clearEdges();

        /// @brief draw temporal route
        void drawTemporalRoute() const;

        /// @brief abort edge path creation
        void abortEdgePathCreation();

        /// @brief finish edge path creation
        void finishEdgePathCreation();

        /// @brief remove last added edge
        void removeLastAddedRoute();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user click over button "Abort route creation"
        long onCmdAbortRouteCreation(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Finish route creation"
        long onCmdFinishRouteCreation(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Remove las inserted edge"
        long onCmdRemoveLastRouteEdge(FXObject*, FXSelector, void*);
        /// @}º

    protected:
        /// @brief FOX needs this
        PersonPlanCreator() {}

    private:
        /// @brief pointer to GNEFrame Parent
        GNEPersonPlanFrame* myFrameParent;

        /// @brief button for finish route creation
        FXButton* myFinishCreationButton;

        /// @brief button for abort route creation
        FXButton* myAbortCreationButton;

        /// @brief button for removing last inserted edge
        FXButton* myRemoveLastInsertedEdge;

        /// @brief current selected edges
        std::vector<GNEEdge*> mySelectedEdges;

        /// @brief vector with temporal route edges
        std::vector<GNEEdge*> myTemporalRoute;
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

    /// @brief finish edge path creation
    void edgePathCreated();

private:
    /// @brief Person selectors
    DemandElementSelector* myPersonSelector;

    /// @brief personPlan selector
    TagSelector* myPersonPlanTagSelector;

    /// @brief internal vehicle attributes
    AttributesCreator* myPersonPlanAttributes;

    /// @brief Person Plan Creator
    PersonPlanCreator* myPersonPlanCreator;

    /// @brief Person Hierarchy
    AttributeCarrierHierarchy* myPersonHierarchy;

    /// @brief Help creation
    HelpCreation* myHelpCreation;
};


#endif

/****************************************************************************/
