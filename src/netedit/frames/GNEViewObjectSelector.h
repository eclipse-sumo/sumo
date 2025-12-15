/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEViewObjectSelector.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// NetworkElement selector module
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/common/GNEGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributeCarrier;
class GNEFrame;
class GNETagProperties;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEViewObjectSelector : protected GNEGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEViewObjectSelector)

public:
    /// @brief constructor
    GNEViewObjectSelector(GNEFrame* frameParent);

    /// @brief destructor
    ~GNEViewObjectSelector();

    /// @brief get tag with selected element type
    SumoXMLTag getTag() const;

    /// @brief check if the given AC is selected
    bool isNetworkElementSelected(const GNEAttributeCarrier* AC) const;

    /// @brief show GNEViewObjectSelector Module
    void showNetworkElementsSelector(const SumoXMLTag tag, const SumoXMLAttr attribute);

    /// @brief hide GNEViewObjectSelector Module
    void hideNetworkElementsSelector();

    /// @brief toggle selected element
    bool toggleSelectedElement(const GNEAttributeCarrier* AC);

    /// @brief toggle selected lane
    bool toggleSelectedLane(const GNELane* lane);

    /// @brief clear selection
    void clearSelection();

    /// @brie fill SUMO base object
    bool fillSumoBaseObject(CommonXMLStructure::SumoBaseObject* baseObject) const;

    /// @name FOX-callbacks
    /// @{
    /// @brief called when user pres button use selected edges
    long onCmdUseSelectedElements(FXObject*, FXSelector, void*);

    /// @brief called when clear selection button is pressed
    long onCmdClearSelection(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX need this
    GNEViewObjectSelector();

private:
    /// @brief pointer to frame parent
    GNEFrame* myFrameParent;

    /// @brief button for use selected edges
    FXButton* myUseSelected = nullptr;

    /// @brief List of GNEViewObjectSelector
    FXList* myList = nullptr;

    /// @brief info label
    FXLabel* myLabel = nullptr;

    /// @brief button for clear selection
    FXButton* myClearSelection = nullptr;

    /// @brief selected ACs
    std::vector<const GNEAttributeCarrier*> mySelectedACs;

    /// @brief network element type
    SumoXMLTag myTag = SUMO_TAG_NOTHING;

    /// @brief attribute vinculated
    SumoXMLAttr myAttribute = SUMO_ATTR_NOTHING;
};
