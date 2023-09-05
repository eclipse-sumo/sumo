/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GUIDialog_HallOfFame.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Angelo Banse
/// @date    Thu, 15 Oct 2020
///
// The SUMO User Conference "Hall of Fame" - dialog / easter egg
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/foxtools/MFXLinkLabel.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>
#include "GUIDialog_HallOfFame.h"


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_HallOfFame::GUIDialog_HallOfFame(FXWindow* parent) :
    FXDialogBox(parent, "Conference Hall of Fame", GUIDesignDialogBox) {
    // set dialog icon
    setIcon(GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI));

    // create frame for main info
    FXHorizontalFrame* mainInfoFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // main text
    FXVerticalFrame* descriptionFrame = new FXVerticalFrame(mainInfoFrame, GUIDesignLabelAboutInfo);
    myHeadlineFont = new FXFont(getApp(), "Arial", 12, FXFont::Bold);
    (new FXLabel(descriptionFrame, "SUMO User Conference 2023 - Hall of Fame", nullptr, GUIDesignLabelAboutInfo))->setFont(myHeadlineFont);
    new FXLabel(descriptionFrame, "", GUIIconSubSys::getIcon(GUIIcon::HALL_OF_FAME), GUIDesignLabelIcon);
    new FXLabel(descriptionFrame, "Voted best presentation:\nComparing Measured Driver Behavior Distributions to Results from Car-Following Models using SUMO and Real-World Vehicle Trajectories from Radar\n(Maxwell Schrader, Mahdi Al Abdraboh and Joshua Bittle)\n", nullptr, GUIDesignLabelAboutInfo);
    new FXLabel(descriptionFrame, "Voted best poster:\nOpen-Source Framework for Modeling Emission Concentrations in Urban Areas\n(Mario Ilic, Seyed M. Saghaeiannejad, Sasan Amini and Klaus Bogenberger)\n", nullptr, GUIDesignLabelAboutInfo);

    // link to conference website
    (new MFXLinkLabel(this, "Visit conference website", nullptr, GUIDesignLabel(JUSTIFY_NORMAL)))->setTipText("https://www.eclipse.dev/sumo/conference");

    // centered ok-button
    FXHorizontalFrame* buttonFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonFrame, GUIDesignAuxiliarHorizontalFrame);
    GUIDesigns::buildFXButton(buttonFrame, TL("OK"), "", "", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(buttonFrame, GUIDesignAuxiliarHorizontalFrame);
}


void
GUIDialog_HallOfFame::create() {
    FXDialogBox::create();
}


GUIDialog_HallOfFame::~GUIDialog_HallOfFame() {
    delete myHeadlineFont;
}


/****************************************************************************/
