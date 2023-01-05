/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEElementTree.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for show hierarchical elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/foxtools/MFXTreeListDynamic.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;
class GNEDataSet;
class GNEDataInterval;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEElementTree : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEElementTree)

public:
    /// @brief constructor
    GNEElementTree(GNEFrame* frameParent);

    /// @brief destructor
    ~GNEElementTree();

    /// @brief show GNEElementTree
    void showHierarchicalElementTree(GNEAttributeCarrier* AC);

    /// @brief hide GNEElementTree
    void hideHierarchicalElementTree();

    /// @brief refresh GNEElementTree
    void refreshHierarchicalElementTree();

    /// @brief if given AttributeCarrier is the same of myHE, set it as nullptr
    void removeCurrentEditedAttributeCarrier(const GNEAttributeCarrier* HE);

    /// @name FOX-callbacks
    /// @{
    /// @brief called when user press right click over an item of list of children
    long onCmdShowChildMenu(FXObject*, FXSelector, void* data);

    /// @brief called when user click over option "center" of child Menu
    long onCmdCenterItem(FXObject*, FXSelector, void*);

    /// @brief called when user click over option "inspect" of child menu
    long onCmdInspectItem(FXObject*, FXSelector, void*);

    /// @brief called when user click over option "delete" of child menu
    long onCmdDeleteItem(FXObject*, FXSelector, void*);

    /// @brief called when user click over option "Move up" of child menu
    long onCmdMoveItemUp(FXObject*, FXSelector, void*);

    /// @brief called when user click over option "Move down" of child menu
    long onCmdMoveItemDown(FXObject*, FXSelector, void*);
    /// @}

protected:
    FOX_CONSTRUCTOR(GNEElementTree)

    // @brief create pop-up menu in the positions X-Y for the clicked attribute carrier
    void createPopUpMenu(int X, int Y, GNEAttributeCarrier* clickedAC);

    /// @brief show child of current attributeCarrier
    FXTreeItem* showAttributeCarrierParents();

    /// @brief show children of given hierarchical element
    void showHierarchicalElementChildren(GNEHierarchicalElement* HE, FXTreeItem* itemParent);

    /// @brief add item into list
    FXTreeItem* addListItem(GNEAttributeCarrier* AC, FXTreeItem* itemParent = nullptr, std::string prefix = "", std::string sufix = "");

    /// @brief add item into list
    FXTreeItem* addListItem(FXTreeItem* itemParent, const std::string& text, FXIcon* icon, bool expanded);

private:
    /// @brief frame Parent
    GNEFrame* myFrameParent;

    /// @brief hierarchical element
    GNEHierarchicalElement* myHE;

    /// @brief pointer to current clicked Attribute Carrier
    GNEAttributeCarrier* myClickedAC;

    /// @brief junction (casted from myClickedAC)
    GNEJunction* myClickedJunction;

    /// @brief edge (casted from myClickedAC)
    GNEEdge* myClickedEdge;

    /// @brief lane (casted from myClickedAC)
    GNELane* myClickedLane;

    /// @brief crossing (casted from myClickedAC)
    GNECrossing* myClickedCrossing;

    /// @brief junction (casted from myClickedAC)
    GNEConnection* myClickedConnection;

    /// @brief additional (casted from myClickedAC)
    GNEAdditional* myClickedAdditional;

    /// @brief demand element (casted from myClickedAC)
    GNEDemandElement* myClickedDemandElement;

    /// @brief data set element (casted from myClickedAC)
    GNEDataSet* myClickedDataSet;

    /// @brief data interval element (casted from myClickedAC)
    GNEDataInterval* myClickedDataInterval;

    /// @brief generic data element (casted from myClickedAC)
    GNEGenericData* myClickedGenericData;

    /// @brief tree list dynamic to show the children of the element to erase
    MFXTreeListDynamic* myTreeListDynamic = nullptr;

    /// @brief map used to save the FXTreeItems items with their vinculated AC
    std::map<FXTreeItem*, GNEAttributeCarrier*> myTreeItemToACMap;

    /// @brief set used to save tree items without AC assigned, the Incoming/Outcoming connections
    std::set<FXTreeItem*> myTreeItemsConnections;
};
