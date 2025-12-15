/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEGroupBoxModule.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
///
//
/****************************************************************************/

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <config.h>

#include <utils/foxtools/MFXButtonTooltip.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/frames/GNEFrame.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/frames/GNETLSTable.h>

#include "GNEGroupBoxModule.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEGroupBoxModule) GNEGroupBoxModuleMap[] = {
    FXMAPFUNC(SEL_PAINT,    0,                              GNEGroupBoxModule::onPaint),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_COLLAPSE,    GNEGroupBoxModule::onCmdCollapseButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_EXTEND,      GNEGroupBoxModule::onCmdExtendButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_RESETWIDTH,  GNEGroupBoxModule::onCmdResetButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_SAVE,        GNEGroupBoxModule::onCmdSaveButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_LOAD,        GNEGroupBoxModule::onCmdLoadButton),
    FXMAPFUNC(SEL_UPDATE,   MID_GROUPBOXMODULE_RESETWIDTH,  GNEGroupBoxModule::onUpdResetButton),
};

// Object implementation
FXIMPLEMENT(GNEGroupBoxModule, FXVerticalFrame, GNEGroupBoxModuleMap, ARRAYNUMBER(GNEGroupBoxModuleMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEGroupBoxModule::GNEGroupBoxModule(GNEFrame* frame, const std::string& text, const int options) :
    FXVerticalFrame(frame->getContentFrame(), GUIDesignGroupBoxModule),
    myOptions(options),
    myFrameParent(frame),
    myCollapsed(false) {
    // build button and labels
    FXHorizontalFrame* headerFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    if (myOptions & Options::COLLAPSIBLE) {
        myCollapseButton = GUIDesigns::buildFXButton(headerFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::COLLAPSE), this, MID_GROUPBOXMODULE_COLLAPSE, GUIDesignButtonGNEGroupBoxModule);
    }
    if (myOptions & Options::EXTENSIBLE) {
        myExtendButton = new MFXButtonTooltip(headerFrame,
                                              frame->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                              TL("Expand"), nullptr, this, MID_GROUPBOXMODULE_EXTEND, GUIDesignButtonGNEGroupBoxModuleExtend);
        myExtendButton->setTipText(TL("Expand frame to show all contents"));
        myResetWidthButton = new MFXButtonTooltip(headerFrame,
                frame->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                "", GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GROUPBOXMODULE_RESETWIDTH, GUIDesignButtonGNEGroupBoxModule);
        myResetWidthButton->setTipText(TL("Shrink frame to default width"));
    }
    if (myOptions & Options::SAVE) {
        mySaveButton = GUIDesigns::buildFXButton(headerFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_GROUPBOXMODULE_SAVE, GUIDesignButtonGNEGroupBoxModule);
    }
    if (myOptions & Options::LOAD) {
        myLoadButton = GUIDesigns::buildFXButton(headerFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_GROUPBOXMODULE_LOAD, GUIDesignButtonGNEGroupBoxModule);
    }
    myLabel = new FXLabel(headerFrame, text.c_str(), nullptr, GUIDesignLabelGNEGroupBoxModule);
    // build collapsable frame
    myCollapsableFrame = new FXVerticalFrame(this, GUIDesignCollapsableFrame);
}


GNEGroupBoxModule::GNEGroupBoxModule(FXVerticalFrame* contentFrame, const std::string& text, const int options) :
    FXVerticalFrame(contentFrame, GUIDesignGroupBoxModuleExtendY),
    myOptions(options),
    myCollapsed(false) {
    // build button and labels
    FXHorizontalFrame* headerFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    if (myOptions & Options::COLLAPSIBLE) {
        myCollapseButton = GUIDesigns::buildFXButton(headerFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::COLLAPSE), this, MID_GROUPBOXMODULE_COLLAPSE, GUIDesignButtonGNEGroupBoxModule);
    }
    if (myOptions & Options::EXTENSIBLE) {
        throw ProcessError("This GNEGroupBoxModule doesn't support Extensible flag");
    }
    if (myOptions & Options::SAVE) {
        mySaveButton = GUIDesigns::buildFXButton(headerFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_GROUPBOXMODULE_SAVE, GUIDesignButtonGNEGroupBoxModule);
    }
    if (myOptions & Options::LOAD) {
        myLoadButton = GUIDesigns::buildFXButton(headerFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_GROUPBOXMODULE_LOAD, GUIDesignButtonGNEGroupBoxModule);
    }
    myLabel = new FXLabel(headerFrame, text.c_str(), nullptr, GUIDesignLabelGNEGroupBoxModule);
    // build collapsable frame
    myCollapsableFrame = new FXVerticalFrame(this, GUIDesignCollapsableFrame);
}


GNEGroupBoxModule::~GNEGroupBoxModule() {}


void
GNEGroupBoxModule::setText(const std::string& text) {
    myLabel->setText(text.c_str());
}


FXVerticalFrame*
GNEGroupBoxModule::getCollapsableFrame() {
    return myCollapsableFrame;
}


long
GNEGroupBoxModule::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXDCWindow dc(this, event);
    // Paint background
    dc.setForeground(backColor);
    dc.fillRectangle(event->rect.x, event->rect.y, event->rect.w, event->rect.h);
    // draw groove rectangle
    drawGrooveRectangle(dc, 0, 15, width, height - 15);
    return 1;
}


long
GNEGroupBoxModule::onCmdCollapseButton(FXObject*, FXSelector, void*) {
    if (myCollapsed) {
        myCollapsed = false;
        myCollapseButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::COLLAPSE));
        myCollapsableFrame->show();
    } else {
        myCollapsed = true;
        myCollapseButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::UNCOLLAPSE));
        myCollapsableFrame->hide();
    }
    recalc();
    return 1;
}


long
GNEGroupBoxModule::onCmdExtendButton(FXObject*, FXSelector, void*) {
    if (myFrameParent) {
        int maximumWidth = -1;
        // iterate over all collapsableFrame childrens
        for (auto child = myCollapsableFrame->getFirst(); child != nullptr; child = child->getNext()) {
            // currently only for GNETLSTables
            const auto TLSTable = dynamic_cast<GNETLSTable*>(child);
            if (TLSTable) {
                // get scrollbar width
                const int scrollBarWidth = TLSTable->getTLSPhasesParent()->getTLSEditorParent()->getScrollBarWidth();
                if ((TLSTable->getWidth() + scrollBarWidth) > maximumWidth) {
                    maximumWidth = (TLSTable->getWidth() + scrollBarWidth);
                }
            }
        }
        // set frame ara width
        if (maximumWidth != -1) {
            // add extra padding (30, constant, 15 left, 15 right)
            myFrameParent->getViewNet()->getViewParent()->setFrameAreaWidth(maximumWidth + 30);
        }
    }
    return 1;
}


long
GNEGroupBoxModule::onCmdResetButton(FXObject*, FXSelector, void*) {
    if (myFrameParent) {
        myFrameParent->getViewNet()->getViewParent()->setFrameAreaWidth(220);
    }
    return 1;
}


long
GNEGroupBoxModule::onUpdResetButton(FXObject* sender, FXSelector, void*) {
    if (myFrameParent) {
        if (myFrameParent->getViewNet()->getViewParent()->getFrameAreaWidth() == 220) {
            sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
        } else {
            sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
        }
    }
    return 1;
}


long
GNEGroupBoxModule::onCmdSaveButton(FXObject*, FXSelector, void*) {
    return saveContents();
}


long
GNEGroupBoxModule::onCmdLoadButton(FXObject*, FXSelector, void*) {
    return loadContents();
}


GNEGroupBoxModule::GNEGroupBoxModule() :
    myOptions(Options::NOTHING),
    myCollapsed(false) {
}


bool
GNEGroupBoxModule::saveContents() const {
    // nothing to do
    return false;
}


bool
GNEGroupBoxModule::loadContents() const {
    // nothing to do
    return false;
}


void
GNEGroupBoxModule::toggleSaveButton(const bool value) {
    if (mySaveButton) {
        if (value) {
            mySaveButton->enable();
        } else {
            mySaveButton->disable();
        }
    }
}
