/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDeleteFrame.h
/// @author  Pablo Alvarez Lopex
/// @date    Dec 2016
/// @version $Id$
///
// The Widget for remove network-elements
/****************************************************************************/
#ifndef GNEDeleteFrame_h
#define GNEDeleteFrame_h

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
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDeleteFrame
 * The Widget for deleting elements
 */
class GNEDeleteFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNEDeleteFrame)

public:

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEDeleteFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEDeleteFrame();

    /// @brief show childs of marked attribute carrier
    void showChildsOfMarkedAttributeCarrier();

    /// @brief remove attribute carrier (element)
    void removeAttributeCarrier(GNEAttributeCarrier* ac);

    /**@brief mark attribute carrier (element)
     * @note if ac == NULL current marked attribute carrier will be dismarked
     */
    void markAttributeCarrier(GNEAttributeCarrier* ac);

    /// @brief update current label
    void updateCurrentLabel(GNEAttributeCarrier* ac);

    /// @brief returns marked atrribute carrier
    GNEAttributeCarrier* getMarkedAttributeCarrier() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief called when user press right click over an item of list of childs
    long onCmdShowChildMenu(FXObject*, FXSelector, void* eventData);

    /// @brief called when user select option "center item" of child Menu
    long onCmdCenterChildItem(FXObject*, FXSelector, void*);

    /// @brief called when user select option "inspect item" of child menu
    long onCmdInspectChildItem(FXObject*, FXSelector, void*);

    /// @brief called when user select option "delte item" of child menu
    long onCmdDeleteChildItem(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNEDeleteFrame() {}

    // @brief create pop-up menu in the positions X-Y for the attribute carrier ac
    void createPopUpMenu(int X, int Y, GNEAttributeCarrier* ac);

private:
    /// @brief groupbox for current element
    FXGroupBox* myGroupBoxCurrentElement;

    /// @brief label for current element
    FXLabel* myCurrentElementLabel;

    /// @brief label for marked element
    FXLabel* myMarkedElementLabel;

    /// @brief groupbox options
    FXGroupBox* myGroupBoxOptions;

    /// @brief groupbox for tree list childs
    FXGroupBox* myGroupBoxTreeList;

    /// @brief groupbox for information
    FXGroupBox* myGroupBoxInformation;

    /// @brief Label for information 1
    FXLabel* myInformationLabel;

    /// @brief checkbox for enable/disable automatically delete additionals childs
    FXCheckButton* myAutomaticallyDeleteAdditionalsCheckButton;

    /// @brief checkbox for enable/disable delete only geometry points
    FXCheckButton* myDeleteOnlyGeometryPoints;

    /// @brief tree list to show the childs of the element to erase
    FXTreeList* myTreelist;

    /// @brief map used to save the Tree items with their AC
    std::map<FXTreeItem*, GNEAttributeCarrier*> myTreeItemToACMap;

    /// @brief set used to save tree items without AC assigned (for example, Incoming/Outcoming connections)
    std::set<FXTreeItem*> myTreeItemsWithoutAC;

    /// @brief pointer to marked attributeCarrier (element)
    GNEAttributeCarrier* myMarkedAC;

    /// @brief current attribute carrier selected in three
    GNEAttributeCarrier* myClickedAC;
};


#endif

/****************************************************************************/

