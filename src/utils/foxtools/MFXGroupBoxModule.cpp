/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXGroupBoxModule.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
///
//
/****************************************************************************/

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <config.h>

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/frames/GNEFrame.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/GNETLSTable.h>

#include "MFXGroupBoxModule.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXGroupBoxModule) MFXGroupBoxModuleMap[] = {
    FXMAPFUNC(SEL_PAINT,    0,                              MFXGroupBoxModule::onPaint),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_COLLAPSE,    MFXGroupBoxModule::onCmdCollapseButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_EXTEND,      MFXGroupBoxModule::onCmdExtendButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_RESETWIDTH,  MFXGroupBoxModule::onCmdResetButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_SAVE,        MFXGroupBoxModule::onCmdSaveButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_LOAD,        MFXGroupBoxModule::onCmdLoadButton),
    FXMAPFUNC(SEL_UPDATE,   MID_GROUPBOXMODULE_RESETWIDTH,  MFXGroupBoxModule::onUpdResetButton),
};

// Object implementation
FXIMPLEMENT(MFXGroupBoxModule, FXVerticalFrame, MFXGroupBoxModuleMap, ARRAYNUMBER(MFXGroupBoxModuleMap))

// ===========================================================================
// method definitions
// ===========================================================================

MFXGroupBoxModule::MFXGroupBoxModule(GNEFrame* frame, const std::string& text, const int options) :
    FXVerticalFrame(frame->getContentFrame(), GUIDesignGroupBoxModule),
    myOptions(options),
    myFrameParent(frame),
    myCollapsed(false) {
    // build button and labels
    FXHorizontalFrame* headerFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    if (myOptions & Options::COLLAPSIBLE) {
        myCollapseButton = new FXButton(headerFrame, "", GUIIconSubSys::getIcon(GUIIcon::COLLAPSE), this, MID_GROUPBOXMODULE_COLLAPSE, GUIDesignButtonMFXGroupBoxModule);
    }
    if (myOptions & Options::EXTENSIBLE) {
        myExtendButton = new FXButton(headerFrame, "Expand", nullptr, this, MID_GROUPBOXMODULE_EXTEND, GUIDesignButtonMFXGroupBoxModuleExtend);
        myResetWidthButton = new FXButton(headerFrame, "", GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GROUPBOXMODULE_RESETWIDTH, GUIDesignButtonMFXGroupBoxModule);
    }
    if (myOptions & Options::SAVE) {
        mySaveButton = new FXButton(headerFrame, "", GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_GROUPBOXMODULE_SAVE, GUIDesignButtonMFXGroupBoxModule);
    }
    if (myOptions & Options::LOAD) {
        myLoadButton = new FXButton(headerFrame, "", GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_GROUPBOXMODULE_LOAD, GUIDesignButtonMFXGroupBoxModule);
    }
    myLabel = new FXLabel(headerFrame, text.c_str(), nullptr, GUIDesignLabelMFXGroupBoxModule);
    // build collapsable frame
    myCollapsableFrame = new FXVerticalFrame(this, GUIDesignCollapsableFrame);
}


MFXGroupBoxModule::MFXGroupBoxModule(FXVerticalFrame* contentFrame, const std::string& text, const int options) :
    FXVerticalFrame(contentFrame, GUIDesignGroupBoxModuleExtendY),
    myOptions(options),
    myCollapsed(false) {
    // build button and labels
    FXHorizontalFrame* headerFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    if (myOptions & Options::COLLAPSIBLE) {
        myCollapseButton = new FXButton(headerFrame, "", GUIIconSubSys::getIcon(GUIIcon::COLLAPSE), this, MID_GROUPBOXMODULE_COLLAPSE, GUIDesignButtonMFXGroupBoxModule);
    }
    if (myOptions & Options::EXTENSIBLE) {
        throw ProcessError("This MFXGroupBoxModule doesn't support Extensible flag");
    }
    if (myOptions & Options::SAVE) {
        mySaveButton = new FXButton(headerFrame, "", GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_GROUPBOXMODULE_SAVE, GUIDesignButtonMFXGroupBoxModule);
    }
    if (myOptions & Options::LOAD) {
        myLoadButton = new FXButton(headerFrame, "", GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_GROUPBOXMODULE_LOAD, GUIDesignButtonMFXGroupBoxModule);
    }
    myLabel = new FXLabel(headerFrame, text.c_str(), nullptr, GUIDesignLabelMFXGroupBoxModule);
    // build collapsable frame
    myCollapsableFrame = new FXVerticalFrame(this, GUIDesignCollapsableFrame);
}


MFXGroupBoxModule::~MFXGroupBoxModule() {}


void
MFXGroupBoxModule::setText(const std::string& text) {
    myLabel->setText(text.c_str());
}


FXVerticalFrame*
MFXGroupBoxModule::getCollapsableFrame() {
    return myCollapsableFrame;
}


long
MFXGroupBoxModule::onPaint(FXObject*, FXSelector, void* ptr) {
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
MFXGroupBoxModule::onCmdCollapseButton(FXObject*, FXSelector, void*) {
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
MFXGroupBoxModule::onCmdExtendButton(FXObject*, FXSelector, void*) {
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
MFXGroupBoxModule::onCmdResetButton(FXObject*, FXSelector, void*) {
    if (myFrameParent) {
        myFrameParent->getViewNet()->getViewParent()->setFrameAreaWidth(220);
    }
    return 1;
}


long
MFXGroupBoxModule::onUpdResetButton(FXObject* sender, FXSelector, void*) {
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
MFXGroupBoxModule::onCmdSaveButton(FXObject*, FXSelector, void*) {
    return saveContents();
}


long
MFXGroupBoxModule::onCmdLoadButton(FXObject*, FXSelector, void*) {
    return loadContents();
}


MFXGroupBoxModule::MFXGroupBoxModule() :
    myOptions(Options::NOTHING),
    myCollapsed(false) {
}


bool
MFXGroupBoxModule::saveContents() const {
    // nothing to do
    return false;
}


bool
MFXGroupBoxModule::loadContents() const {
    // nothing to do
    return false;
}


void
MFXGroupBoxModule::toggleSaveButton(const bool value) {
    if (mySaveButton) {
        if (value) {
            mySaveButton->enable();
        } else {
            mySaveButton->disable();
        }
    }
}
