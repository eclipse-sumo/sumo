/****************************************************************************/
/// @file    GNEDeleteFrame.h
/// @author  Pablo Alvarez Lopex
/// @date    Dec 2016
/// @version $Id$
///
// The Widget for remove network-elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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

    /// @brief show child of attributeCarrier in frame if previously there isn't a marked element
    void showAttributeCarrierChilds(GNEAttributeCarrier* ac);

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
    long onCmdShowChildMenu(FXObject*, FXSelector, void* data);

    /// @brief called when user select option "center item" of child Menu
    long onCmdCenterItem(FXObject*, FXSelector, void*);

    /// @brief called when user select option "inspect item" of child menu
    long onCmdInspectItem(FXObject*, FXSelector, void*);

    /// @brief called when user select option "delte item" of child menu
    long onCmdDeleteItem(FXObject*, FXSelector, void*);
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

    /// @brief checkbox for enable/disble automatically delete additionals childs
    FXMenuCheck* myCheckBoxAutomaticallyDeleteAdditionals;

    /// @brief tree list to show the childs of the element to erase
    FXTreeList* myTreelist;

    /// @brief map used to save the Tree items with their AC
    std::map<FXTreeItem*, GNEAttributeCarrier*> myTreeItemToACMap;

    /// @brief set used to save tree items without AC assigned (for example, Incoming/Outcoming connections)
    std::set<FXTreeItem*> myTreeItesmWithoutAC;

    /// @brief pointer to current AC
    GNEAttributeCarrier* myCurrentAC;

    /// @brief pointer to marked attributeCarrier (element)
    GNEAttributeCarrier* myMarkedAC;

    /// @brief current attribute carrier selected in three
    GNEAttributeCarrier* myClickedAC;
};


#endif

/****************************************************************************/

