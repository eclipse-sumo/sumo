/****************************************************************************/
/// @file    GNEDeleteFrame.cpp
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>

#include "GNEDeleteFrame.h"
#include "GNEAdditionalFrame.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNECrossing.h"
#include "GNEPOI.h"
#include "GNEConnection.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEAttributeCarrier.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDeleteFrame) GNEDeleteFrameMap[] = {
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   MID_GNEDELETE_CHILDS,           GNEDeleteFrame::onCmdShowMenu),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_DELETEFRAME_CENTER,     GNEDeleteFrame::onCmdCenterItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_DELETEFRAME_INSPECT,    GNEDeleteFrame::onCmdInspectItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_DELETEFRAME_DELETE,     GNEDeleteFrame::onCmdDeleteItem),
};

// Object implementation
FXIMPLEMENT(GNEDeleteFrame, FXVerticalFrame, GNEDeleteFrameMap, ARRAYNUMBER(GNEDeleteFrameMap))

// ===========================================================================
// method definitions
// ===========================================================================
GNEDeleteFrame::GNEDeleteFrame(FXHorizontalFrame *horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Delete"),
    myMarkedAc(NULL) {
    // Create Groupbox for current element
    myGroupBoxCurrentElement = new FXGroupBox(myContentFrame, "Current and marked element", GUIDesignGroupBoxFrame);
    myCurrentElementLabel = new FXLabel(myGroupBoxCurrentElement, "- ", 0, GUIDesignLabelLeft);
    myMarkedElementLabel = new FXLabel(myGroupBoxCurrentElement, "- ", 0, GUIDesignLabelLeft);

    // Create Groupbox for current element
    myGroupBoxOptions = new FXGroupBox(myContentFrame, "Options", GUIDesignGroupBoxFrame);
    
    // Create groupbox for create crossings 
    myGroupBoxTreeList = new FXGroupBox(myContentFrame, "Childs", GUIDesignGroupBoxFrame);
    myTreelist = new FXTreeList(myGroupBoxTreeList, this, MID_GNEDELETE_CHILDS, TREELIST_SHOWS_LINES | TREELIST_SINGLESELECT | FRAME_SUNKEN | FRAME_GROOVE | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT, 0, 0, 0, 200);
    
    // Create groupbox for help
    myGroupBoxInformation = new FXGroupBox(myContentFrame, "Information", GUIDesignGroupBoxFrame);
    myInformationLabel = new FXLabel(myGroupBoxInformation, " - Left click to delete element.\n - Hold <CTRL> and left click\n   to mark element.\n - Left click in another element\n   dismark marked element.", 0, GUIDesignLabelLeft);
}


GNEDeleteFrame::~GNEDeleteFrame() {

}


void 
GNEDeleteFrame::showAttributeCarrierChilds(GNEAttributeCarrier *ac) {
    // clear items
    myTreelist->clearItems();
    myTreeItemToACMap.clear();
    // change label
    if(ac != NULL) {
        myCurrentElementLabel->setText(("- " + toString(ac->getTag()) + " '" + ac->getID() + "'").c_str());
    } else {
        myCurrentElementLabel->setText("- ");
    }
    // check if currently there is a marked element
    if(myMarkedAc != NULL) {
        ac = myMarkedAc;
    }
    // Switch gl type of ac
    if (ac) {
        switch (dynamic_cast<GUIGlObject*>(ac)->getType()) {
            case GLO_JUNCTION: {
                // insert junction root
                GNEJunction *junction = dynamic_cast<GNEJunction*>(ac);
                FXTreeItem *junctionItem = myTreelist->insertItem(0, 0, toString(junction->getTag()).c_str(), junction->getIcon(), junction->getIcon());
                myTreeItemToACMap[junctionItem] = junction;
                junctionItem->setExpanded(true);
                // insert edges
                for(int i = 0; i < junction->getGNEEdges().size(); i++) {
                    GNEEdge *edge = junction->getGNEEdges().at(i);
                    FXTreeItem *edgeItem = myTreelist->insertItem(0, junctionItem, (toString(edge->getTag()) + " " + toString(i)).c_str(), edge->getIcon(), edge->getIcon());
                    myTreeItemToACMap[edgeItem] = edge;
                    edgeItem->setExpanded(true);
                    // insert lanes
                    for(int j = 0; j < edge->getLanes().size(); j++) {
                        GNELane *lane = edge->getLanes().at(j);
                        FXTreeItem *laneItem = myTreelist->insertItem(0, edgeItem, (toString(lane->getTag()) + " " + toString(j)).c_str(), edge->getIcon(), edge->getIcon());
                        myTreeItemToACMap[laneItem] = lane;
                        laneItem->setExpanded(true);
                        // insert additionals of lanes
                        for(int k = 0; k < lane->getAdditionalChilds().size(); k++) {
                            GNEAdditional* additional = lane->getAdditionalChilds().at(k);
                            FXTreeItem *additionalItem = myTreelist->insertItem(0, laneItem, (toString(additional->getTag()) + " " + toString(k)).c_str(), additional->getIcon(), additional->getIcon());
                            myTreeItemToACMap[additionalItem] = additional;
                            additionalItem->setExpanded(true);
                        }
                    }
                    // insert additionals of edge
                    for(int j = 0; j < edge->getAdditionalChilds().size(); j++) {
                        GNEAdditional* additional = edge->getAdditionalChilds().at(j);
                        FXTreeItem *additionalItem = myTreelist->insertItem(0, edgeItem, (toString(additional->getTag()) + " " + toString(j)).c_str(), additional->getIcon(), additional->getIcon());
                        myTreeItemToACMap[additionalItem] = additional;
                        additionalItem->setExpanded(true);
                    }

                }
                // insert crossings
                for(int i = 0; i < junction->getGNECrossings().size(); i++) {
                    GNECrossing *crossing = junction->getGNECrossings().at(i);
                    FXTreeItem *crossingItem = myTreelist->insertItem(0, junctionItem, (toString(crossing->getTag()) + " " + toString(i)).c_str(), crossing->getIcon(), crossing->getIcon());
                    myTreeItemToACMap[crossingItem] = crossing;
                    crossingItem->setExpanded(true);
                }
                break;
            }
            case GLO_EDGE: {
                // insert edge root
                GNEEdge *edge = dynamic_cast<GNEEdge*>(ac);
                FXTreeItem *edgeItem = myTreelist->insertItem(0, 0, toString(edge->getTag()).c_str(), edge->getIcon(), edge->getIcon());
                myTreeItemToACMap[edgeItem] = edge;
                edgeItem->setExpanded(true);
                // insert lanes
                for(int i = 0; i < edge->getLanes().size(); i++) {
                    GNELane *lane = edge->getLanes().at(i);
                    FXTreeItem *laneItem = myTreelist->insertItem(0, edgeItem, (toString(lane->getTag()) + " " + toString(i)).c_str(), edge->getIcon(), edge->getIcon());
                    myTreeItemToACMap[laneItem] = lane;
                    laneItem->setExpanded(true);
                    // insert additionals of lanes
                    for(int k = 0; k < lane->getAdditionalChilds().size(); k++) {
                        GNEAdditional* additional = lane->getAdditionalChilds().at(k);
                        FXTreeItem *additionalItem = myTreelist->insertItem(0, laneItem, (toString(additional->getTag()) + " " + toString(k)).c_str(), additional->getIcon(), additional->getIcon());
                        myTreeItemToACMap[additionalItem] = additional;
                        additionalItem->setExpanded(true);
                    }
                }
                // insert additionals of edge
                for(int i = 0; i < edge->getAdditionalChilds().size(); i++) {
                    GNEAdditional* additional = edge->getAdditionalChilds().at(i);
                    FXTreeItem *additionalItem = myTreelist->insertItem(0, edgeItem, (toString(additional->getTag()) + " " + toString(i)).c_str(), additional->getIcon(), additional->getIcon());
                    myTreeItemToACMap[additionalItem] = additional;
                    additionalItem->setExpanded(true);
                }
                break;
            }
            case GLO_LANE: {
                // insert edge root
                GNELane *lane = dynamic_cast<GNELane*>(ac);
                FXTreeItem *laneItem = myTreelist->insertItem(0, 0, toString(lane->getTag()).c_str(), lane->getIcon(), lane->getIcon());
                myTreeItemToACMap[laneItem] = lane;
                laneItem->setExpanded(true);
                // insert additionals of lanes
                for(int i = 0; i < lane->getAdditionalChilds().size(); i++) {
                    GNEAdditional* additional = lane->getAdditionalChilds().at(i);
                    FXTreeItem *additionalItem = myTreelist->insertItem(0, laneItem, (toString(additional->getTag()) + " " + toString(i)).c_str(), additional->getIcon(), additional->getIcon());
                    myTreeItemToACMap[additionalItem] = additional;
                    additionalItem->setExpanded(true);
                }
                break;
            }
            case GLO_POI: {

                break;
            }
            case GLO_POLYGON: {
                break;
            }
            case GLO_CROSSING: {

                break;
            }
            case GLO_ADDITIONAL: {

                break;
            }
            case GLO_CONNECTION: {

                break;
            }
            default: {
                break;
            }
        }
    }
}


void 
GNEDeleteFrame::removeAttributeCarrier(GNEAttributeCarrier *ac) {
    // check type of of GL object
    switch (dynamic_cast<GUIGlObject*>(ac)->getType()) {
        case GLO_JUNCTION: {
            myViewNet->getNet()->deleteJunction(dynamic_cast<GNEJunction*>(ac), myViewNet->getUndoList());
            break;
        }
        case GLO_EDGE: {
            myViewNet->getNet()->deleteGeometryOrEdge(dynamic_cast<GNEEdge*>(ac), myViewNet->getPositionInformation(), myViewNet->getUndoList());
            break;
        }
        case GLO_LANE: {
            myViewNet->getNet()->deleteLane(dynamic_cast<GNELane*>(ac), myViewNet->getUndoList());
            break;
        }
        case GLO_POI: {
            // XXX this is a dirty dirty hack! implemente GNEChange_POI
            myViewNet->getNet()->getShapeContainer().removePOI(dynamic_cast<GNEPOI*>(ac)->getMicrosimID());
            break;
        }
        case GLO_POLYGON: {
            //
            break;
        }
        case GLO_CROSSING: {
            myViewNet->getNet()->deleteCrossing(dynamic_cast<GNECrossing*>(ac), myViewNet->getUndoList());
            break;
        }
        case GLO_ADDITIONAL: {
            myViewNet->getViewParent()->getAdditionalFrame()->removeAdditional(dynamic_cast<GNEAdditional*>(ac));
            break;
        }
        case GLO_CONNECTION: {
            myViewNet->getNet()->deleteConnection(dynamic_cast<GNEConnection*>(ac), myViewNet->getUndoList());
            break;
        }
        default: {
            break;
        }
    }
    // update view to show changes
    myViewNet->update();
}


void 
GNEDeleteFrame::markAttributeCarrier(GNEAttributeCarrier *ac) {
    if(ac != NULL) {
        myMarkedAc = ac;
        myMarkedElementLabel->setText(("- " + toString(myMarkedAc->getTag()) + " '" + myMarkedAc->getID() + "'").c_str());
        showAttributeCarrierChilds(ac);
    } else {
        myMarkedAc = NULL;
        myMarkedElementLabel->setText("- ");
    }
}


GNEAttributeCarrier*
GNEDeleteFrame::getMarkedAttributeCarrier() const {
    return myMarkedAc;
}


long 
GNEDeleteFrame::onCmdSelectItem(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNEDeleteFrame::onCmdShowMenu(FXObject*, FXSelector, void* data) {
    // Obtain event
    FXEvent* e = (FXEvent*) data;
    // Check if there are an item in the position and create pop-up menu
    if(myTreelist->getItemAt(e->win_x, e->win_y)) {
        createPopUpMenu(e->root_x, e->root_y, myTreeItemToACMap[myTreelist->getItemAt(e->win_x, e->win_y)]->getID());
    }
    return 1;
}


long 
GNEDeleteFrame::onCmdCenterItem(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNEDeleteFrame::onCmdInspectItem(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNEDeleteFrame::onCmdDeleteItem(FXObject*, FXSelector, void*) {
    return 1;
}


void 
GNEDeleteFrame::createPopUpMenu(int X, int Y, std::string acID) {
    // create FXMenuPane
    FXMenuPane *pane = new FXMenuPane(myTreelist);
    // save type of AC
    if(myViewNet->getNet()->retrieveJunction(acID, false)) {
        mySelectedAc = myViewNet->getNet()->retrieveJunction(acID, false);
    } else if(myViewNet->getNet()->retrieveEdge(acID, false)) {
        mySelectedAc = myViewNet->getNet()->retrieveEdge(acID, false);
    } else if(myViewNet->getNet()->retrieveLane(acID, false)) {
        mySelectedAc = myViewNet->getNet()->retrieveLane(acID, false);
    } else if(myViewNet->getNet()->retrieveAdditional(acID, false)) {
        mySelectedAc = myViewNet->getNet()->retrieveAdditional(acID, false);
    } else {
        mySelectedAc = NULL;
    }

    // Only continue if AC was identified 
    if(mySelectedAc) {
        // set name
        new MFXMenuHeader(pane, myViewNet->getViewParent()->getApp()->getBoldFont(), (toString(mySelectedAc->getTag()) + ": " + mySelectedAc->getID()).c_str(), mySelectedAc->getIcon());
        new FXMenuSeparator(pane);
        // Fill FXMenuCommand
        new FXMenuCommand(pane, "Center", GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_GNE_DELETEFRAME_CENTER);
        new FXMenuCommand(pane, "Inspect", GUIIconSubSys::getIcon(ICON_MODEINSPECT), this, MID_GNE_DELETEFRAME_INSPECT);
        new FXMenuCommand(pane, "Delete", GUIIconSubSys::getIcon(ICON_MODEDELETE), this, MID_GNE_DELETEFRAME_DELETE);
        // Center in the mouse position and create pane
        pane->setX(X);
        pane->setY(Y);
        pane->create();
        pane->show();
    }
}


/****************************************************************************/
