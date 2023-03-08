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
/// @file    GUIDialog_Feedback.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
// The application's "Feedback" dialog
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/foxtools/MFXLinkLabel.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GUIDialog_Feedback.h"


// ===========================================================================
// method definitions
// ===========================================================================

GUIDialog_Feedback::GUIDialog_Feedback(FXWindow* parent) :
    FXDialogBox(parent, "Feedback", GUIDesignDialogBox) {
    // set dialog icon
    setIcon(GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI));
    // create frame for main info
    FXHorizontalFrame* mainInfoFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // SUMO Icon
    new FXLabel(mainInfoFrame, "", GUIIconSubSys::getIcon(GUIIcon::SUMO_LOGO), GUIDesignLabelIcon);
    // create frame for links
    FXVerticalFrame* linksFrame = new FXVerticalFrame(mainInfoFrame, GUIDesignLabelAboutInfo);
    // general problem solving
    (new MFXLinkLabel(linksFrame, TL("- General problem solving"), nullptr, GUIDesignLabel(JUSTIFY_LEFT)))->setTipText("https://sumo.dlr.de/docs/FAQ.html#general_problem_solving");
    // empty label
    new FXLabel(linksFrame, " ", nullptr, GUIDesignLabelAboutInfo);
    // mailing list
    (new MFXLinkLabel(linksFrame, TL("- Sumo-user mailing list"), nullptr, GUIDesignLabel(JUSTIFY_LEFT)))->setTipText("https://accounts.eclipse.org/mailing-list/sumo-user");
    // empty label
    new FXLabel(linksFrame, " ", nullptr, GUIDesignLabelAboutInfo);
    // link to Email page
    (new MFXLinkLabel(linksFrame, TL("- Send us an Email"), nullptr, GUIDesignLabel(JUSTIFY_LEFT)))->setTipText("https://www.dlr.de/ts/en/desktopdefault.aspx/tabid-1231/mailcontact-30303/");
    // centered ok-button
    FXHorizontalFrame* buttonFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(buttonFrame, (TL("OK") + std::string("\t\t")).c_str(), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(buttonFrame, GUIDesignAuxiliarHorizontalFrame);
}


void
GUIDialog_Feedback::create() {
    FXDialogBox::create();
}

/****************************************************************************/
