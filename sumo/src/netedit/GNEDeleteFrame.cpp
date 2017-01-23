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
#include "GNEInspectorFrame.h"
#include "GNEAdditionalFrame.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNECrossing.h"
#include "GNEPOI.h"
#include "GNEPoly.h"
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
    myCurrentAC(NULL),
    myMarkedAc(NULL) {
    // Create Groupbox for current element
    myGroupBoxCurrentElement = new FXGroupBox(myContentFrame, "Current and marked element", GUIDesignGroupBoxFrame);
    myCurrentElementLabel = new FXLabel(myGroupBoxCurrentElement, "- ", 0, GUIDesignLabelLeft);
    myMarkedElementLabel = new FXLabel(myGroupBoxCurrentElement, "- ", 0, GUIDesignLabelLeft);

    // Create Groupbox for current element
    myGroupBoxOptions = new FXGroupBox(myContentFrame, "Options", GUIDesignGroupBoxFrame);
    
    // Create groupbox for create crossings 
    myGroupBoxTreeList = new FXGroupBox(myContentFrame, "Childs", GUIDesignGroupBoxFrame);
    myTreelist = new FXTreeList(myGroupBoxTreeList, this, MID_GNEDELETE_CHILDS, TREELIST_SHOWS_LINES | TREELIST_SHOWS_BOXES | TREELIST_SINGLESELECT | FRAME_SUNKEN | FRAME_GROOVE | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT, 0, 0, 0, 200);
    
    // Create groupbox for help
    myGroupBoxInformation = new FXGroupBox(myContentFrame, "Information", GUIDesignGroupBoxFrame);
    myInformationLabel = new FXLabel(myGroupBoxInformation, " - Left click to delete element.\n - Hold <CTRL> and left click\n   to mark element.\n - Left click in another element\n   dismark marked element.", 0, GUIDesignLabelLeft);
}


GNEDeleteFrame::~GNEDeleteFrame() {}


void 
GNEDeleteFrame::showAttributeCarrierChilds(GNEAttributeCarrier *ac) {
    myCurrentAC = ac;
    // clear items
    myTreelist->clearItems();
    myTreeItemToACMap.clear();
    myTreeItesmWithoutAC.clear();
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
                        FXTreeItem *laneItem = myTreelist->insertItem(0, edgeItem, (toString(lane->getTag()) + " " + toString(j)).c_str(), lane->getIcon(), lane->getIcon());
                        myTreeItemToACMap[laneItem] = lane;
                        laneItem->setExpanded(true);
                        // insert additionals of lanes
                        for(int k = 0; k < lane->getAdditionalChilds().size(); k++) {
                            GNEAdditional* additional = lane->getAdditionalChilds().at(k);
                            FXTreeItem *additionalItem = myTreelist->insertItem(0, laneItem, (toString(additional->getTag()) + " " + toString(k)).c_str(), additional->getIcon(), additional->getIcon());
                            myTreeItemToACMap[additionalItem] = additional;
                            additionalItem->setExpanded(true);
                        }
                        // insert incoming connections of lanes (by default isn't expanded)
                        if(lane->getGNEIncomingConnections().size() > 0) {
                            FXTreeItem *incomingConnections = myTreelist->insertItem(0, laneItem, "Incomings", lane->getGNEIncomingConnections().front()->getIcon(), lane->getGNEIncomingConnections().front()->getIcon());
                            myTreeItesmWithoutAC.insert(incomingConnections);
                            incomingConnections->setExpanded(false);
                            for(int k = 0; k < lane->getGNEIncomingConnections().size(); k++) {
                                GNEConnection* connection = lane->getGNEIncomingConnections().at(k);
                                FXTreeItem *connectionItem = myTreelist->insertItem(0, incomingConnections, (toString(connection->getTag()) + " " + toString(k)).c_str(), connection->getIcon(), connection->getIcon());
                                myTreeItemToACMap[connectionItem] = connection;
                                connectionItem->setExpanded(true);
                            }
                        }
                        // insert outcoming connections of lanes (by default isn't expanded)
                        if(lane->getGNEOutcomingConnections().size() > 0) {
                            FXTreeItem *outgoingConnections = myTreelist->insertItem(0, laneItem, "Outcomings", lane->getGNEOutcomingConnections().front()->getIcon(), lane->getGNEOutcomingConnections().front()->getIcon());
                            myTreeItesmWithoutAC.insert(outgoingConnections);
                            outgoingConnections->setExpanded(false);
                            for(int k = 0; k < lane->getGNEOutcomingConnections().size(); k++) {
                                GNEConnection* connection = lane->getGNEOutcomingConnections().at(k);
                                FXTreeItem *connectionItem = myTreelist->insertItem(0, outgoingConnections, (toString(connection->getTag()) + " " + toString(k)).c_str(), connection->getIcon(), connection->getIcon());
                                myTreeItemToACMap[connectionItem] = connection;
                                connectionItem->setExpanded(true);
                            }
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
                    FXTreeItem *laneItem = myTreelist->insertItem(0, edgeItem, (toString(lane->getTag()) + " " + toString(i)).c_str(), lane->getIcon(), lane->getIcon());
                    myTreeItemToACMap[laneItem] = lane;
                    laneItem->setExpanded(true);
                    // insert additionals of lanes
                    for(int j = 0; j < lane->getAdditionalChilds().size(); j++) {
                        GNEAdditional* additional = lane->getAdditionalChilds().at(j);
                        FXTreeItem *additionalItem = myTreelist->insertItem(0, laneItem, (toString(additional->getTag()) + " " + toString(j)).c_str(), additional->getIcon(), additional->getIcon());
                        myTreeItemToACMap[additionalItem] = additional;
                        additionalItem->setExpanded(true);
                    }
                    // insert incoming connections of lanes (by default isn't expanded)
                    if(lane->getGNEIncomingConnections().size() > 0) {
                        FXTreeItem *incomingConnections = myTreelist->insertItem(0, laneItem, "Incomings", lane->getGNEIncomingConnections().front()->getIcon(), lane->getGNEIncomingConnections().front()->getIcon());
                        myTreeItesmWithoutAC.insert(incomingConnections);
                        incomingConnections->setExpanded(false);
                        for(int j = 0; j < lane->getGNEIncomingConnections().size(); j++) {
                            GNEConnection* connection = lane->getGNEIncomingConnections().at(j);
                            FXTreeItem *connectionItem = myTreelist->insertItem(0, incomingConnections, (toString(connection->getTag()) + " " + toString(j)).c_str(), connection->getIcon(), connection->getIcon());
                            myTreeItemToACMap[connectionItem] = connection;
                            connectionItem->setExpanded(true);
                        }
                    }
                    // insert outcoming connections of lanes (by default isn't expanded)
                    if(lane->getGNEOutcomingConnections().size() > 0) {
                        FXTreeItem *outgoingConnections = myTreelist->insertItem(0, laneItem, "Outcomings", lane->getGNEOutcomingConnections().front()->getIcon(), lane->getGNEOutcomingConnections().front()->getIcon());
                        myTreeItesmWithoutAC.insert(outgoingConnections);
                        outgoingConnections->setExpanded(false);
                        for(int j = 0; j < lane->getGNEOutcomingConnections().size(); j++) {
                            GNEConnection* connection = lane->getGNEOutcomingConnections().at(j);
                            FXTreeItem *connectionItem = myTreelist->insertItem(0, outgoingConnections, (toString(connection->getTag()) + " " + toString(j)).c_str(), connection->getIcon(), connection->getIcon());
                            myTreeItemToACMap[connectionItem] = connection;
                            connectionItem->setExpanded(true);
                        }
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
                // insert lane root
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
                // insert incoming connections of lanes (by default isn't expanded)
                if(lane->getGNEIncomingConnections().size() > 0) {
                    FXTreeItem *incomingConnections = myTreelist->insertItem(0, laneItem, "Incomings", lane->getGNEIncomingConnections().front()->getIcon(), lane->getGNEIncomingConnections().front()->getIcon());
                    myTreeItesmWithoutAC.insert(incomingConnections);
                    incomingConnections->setExpanded(false);
                    for(int i = 0; i < lane->getGNEIncomingConnections().size(); i++) {
                        GNEConnection* connection = lane->getGNEIncomingConnections().at(i);
                        FXTreeItem *connectionItem = myTreelist->insertItem(0, incomingConnections, (toString(connection->getTag()) + " " + toString(i)).c_str(), connection->getIcon(), connection->getIcon());
                        myTreeItemToACMap[connectionItem] = connection;
                        connectionItem->setExpanded(true);
                    }
                }
                // insert outcoming connections of lanes (by default isn't expanded)
                if(lane->getGNEOutcomingConnections().size() > 0) {
                    FXTreeItem *outgoingConnections = myTreelist->insertItem(0, laneItem, "Outcomings", lane->getGNEOutcomingConnections().front()->getIcon(), lane->getGNEOutcomingConnections().front()->getIcon());
                    myTreeItesmWithoutAC.insert(outgoingConnections);
                    outgoingConnections->setExpanded(false);
                    for(int i = 0; i < lane->getGNEOutcomingConnections().size(); i++) {
                        GNEConnection* connection = lane->getGNEOutcomingConnections().at(i);
                        FXTreeItem *connectionItem = myTreelist->insertItem(0, outgoingConnections, (toString(connection->getTag()) + " " + toString(i)).c_str(), connection->getIcon(), connection->getIcon());
                        myTreeItemToACMap[connectionItem] = connection;
                        connectionItem->setExpanded(true);
                    }
                }
                break;
            }
            case GLO_POI: {
                // insert POI root
                GNEPOI *POI = dynamic_cast<GNEPOI*>(ac);
                FXTreeItem *POIItem = myTreelist->insertItem(0, 0, toString(POI->getTag()).c_str(), POI->getIcon(), POI->getIcon());
                myTreeItemToACMap[POIItem] = POI;
                POIItem->setExpanded(true);
                break;
            }
            case GLO_POLYGON: {
                // insert polygon root
                GNEPoly *polygon = dynamic_cast<GNEPoly*>(ac);
                FXTreeItem *polygonItem = myTreelist->insertItem(0, 0, toString(polygon->getTag()).c_str(), polygon->getIcon(), polygon->getIcon());
                myTreeItemToACMap[polygonItem] = polygon;
                polygonItem->setExpanded(true);
                break;
            }
            case GLO_CROSSING: {
                // insert crossing root
                GNECrossing *crossing = dynamic_cast<GNECrossing*>(ac);
                FXTreeItem *crossingItem = myTreelist->insertItem(0, 0, toString(crossing->getTag()).c_str(), crossing->getIcon(), crossing->getIcon());
                myTreeItemToACMap[crossingItem] = crossing;
                crossingItem->setExpanded(true);
                break;
            }
            case GLO_ADDITIONAL: {
                // insert additional root
                GNEAdditional *additional = dynamic_cast<GNEAdditional*>(ac);
                FXTreeItem *additionalItem = myTreelist->insertItem(0, 0, toString(additional->getTag()).c_str(), additional->getIcon(), additional->getIcon());
                myTreeItemToACMap[additionalItem] = additional;
                additionalItem->setExpanded(true);
                break;
            }
            case GLO_CONNECTION: {
                // insert connection root
                GNEConnection *connection = dynamic_cast<GNEConnection*>(ac);
                FXTreeItem *connectionItem = myTreelist->insertItem(0, 0, toString(connection->getTag()).c_str(), connection->getIcon(), connection->getIcon());
                myTreeItemToACMap[connectionItem] = connection;
                connectionItem->setExpanded(true);
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
    // To remove an attribute carrier deleteFrame must be visible
    if(shown() == false) {
        // Hide inspector frame and show delete frame
        myViewNet->getViewParent()->getInspectorFrame()->hide();
        show();
    } else {
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
    }
    // update view to show changes
    myViewNet->update();
}


void 
GNEDeleteFrame::markAttributeCarrier(GNEAttributeCarrier *ac) {
    // Check if under cursor there are an Attribute Carrier
    if(ac != NULL) {
        myMarkedElementLabel->setText(("- " + toString(ac->getTag()) + " '" + ac->getID() + "'").c_str());
    } else {
        myMarkedElementLabel->setText("- ");
    }
    // mark ac
    myMarkedAc = ac;
}


void 
GNEDeleteFrame::updateCurrentLabel(GNEAttributeCarrier *ac) {
    // change label
    if(ac != NULL) {
        myCurrentElementLabel->setText(("- " + toString(ac->getTag()) + " '" + ac->getID() + "'").c_str());
    } else {
        myCurrentElementLabel->setText("- ");
    }
}


GNEAttributeCarrier*
GNEDeleteFrame::getMarkedAttributeCarrier() const {
    return myMarkedAc;
}


long 
GNEDeleteFrame::onCmdShowMenu(FXObject*, FXSelector, void* data) {
    // Obtain event
    FXEvent* e = (FXEvent*) data;
    FXTreeItem* item = myTreelist->getItemAt(e->win_x, e->win_y);
    // Check if there are an item in the position and create pop-up menu
    if(item && (myTreeItesmWithoutAC.find(item) == myTreeItesmWithoutAC.end())) {
        createPopUpMenu(e->root_x, e->root_y, myTreeItemToACMap[myTreelist->getItemAt(e->win_x, e->win_y)]);
    }
    return 1;
}


long 
GNEDeleteFrame::onCmdCenterItem(FXObject*, FXSelector, void*) {
    if(dynamic_cast<GNENetElement*>(myClickedAc)) {
        myViewNet->centerTo(dynamic_cast<GNENetElement*>(myClickedAc)->getGlID(), false); 
    } else if (dynamic_cast<GNEAdditional*>(myClickedAc)) {
        myViewNet->centerTo(dynamic_cast<GNEAdditional*>(myClickedAc)->getGlID(), false); 
    } else if(dynamic_cast<GNEPOI*>(myClickedAc)) {
        myViewNet->centerTo(dynamic_cast<GNEPOI*>(myClickedAc)->getGlID(), false); 
    } else if(dynamic_cast<GNEPoly*>(myClickedAc)) {
        myViewNet->centerTo(dynamic_cast<GNEPoly*>(myClickedAc)->getGlID(), false); 
    }
    myViewNet->update();
    return 1;
}


long 
GNEDeleteFrame::onCmdInspectItem(FXObject*, FXSelector, void*) {
    if(myMarkedAc != NULL) {
        myViewNet->getViewParent()->getInspectorFrame()->show();
        myViewNet->getViewParent()->getInspectorFrame()->inspectFromDeleteFrame(myClickedAc, myMarkedAc, true);
        // Hide delete frame and show inspector frame
        hide();

    } else if(myCurrentAC != NULL) {
        myViewNet->getViewParent()->getInspectorFrame()->show();
        myViewNet->getViewParent()->getInspectorFrame()->inspectFromDeleteFrame(myClickedAc, myCurrentAC, false);
        // Hide delete frame and show inspector frame
        hide();
    }
    return 1;
}


long 
GNEDeleteFrame::onCmdDeleteItem(FXObject*, FXSelector, void*) {
    removeAttributeCarrier(myClickedAc);
    showAttributeCarrierChilds(myCurrentAC);
    return 1;
}


void 
GNEDeleteFrame::createPopUpMenu(int X, int Y, GNEAttributeCarrier* ac) {
    // create FXMenuPane
    FXMenuPane *pane = new FXMenuPane(myTreelist);
    // set current clicked AC
    myClickedAc = ac;
    // set name
    new MFXMenuHeader(pane, myViewNet->getViewParent()->getApp()->getBoldFont(), (toString(myClickedAc->getTag()) + ": " + myClickedAc->getID()).c_str(), myClickedAc->getIcon());
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


/****************************************************************************/
