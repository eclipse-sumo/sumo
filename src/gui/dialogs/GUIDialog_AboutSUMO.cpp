/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIDialog_AboutSUMO.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// The application's "About" - dialog
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

#include "GUIDialog_AboutSUMO.h"
#include <utils/foxtools/FXLinkLabel.h>
#include <utils/common/StdDefs.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>

// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_AboutSUMO::GUIDialog_AboutSUMO(FXWindow* parent) :
    FXDialogBox(parent, "About SUMO", GUIDesignDialogBox) {
    // set dialog icon
    setIcon(GUIIconSubSys::getIcon(ICON_SUMO));

    // create frame for main info
    FXHorizontalFrame* mainInfoFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // DLR Icon
    new FXLabel(mainInfoFrame, "", GUIIconSubSys::getIcon(ICON_DLR), GUIDesignLabelIcon64x64noSpacing);

    // "SUMO <VERSION>"
    FXVerticalFrame* descriptionFrame = new FXVerticalFrame(mainInfoFrame, GUIDesignLabelAboutInfo);
    myHeadlineFont = new FXFont(getApp(), "Arial", 18, FXFont::Bold);
    FXLabel* neteditLabel = new FXLabel(descriptionFrame, "SUMO " VERSION_STRING, 0, GUIDesignLabelAboutInfo);
    neteditLabel->setFont(myHeadlineFont);
    new FXLabel(descriptionFrame, "Simulation of Urban MObility", 0, GUIDesignLabelAboutInfo);
    new FXLabel(descriptionFrame, HAVE_ENABLED, 0, GUIDesignLabelAboutInfo);

    // Netedit icon
    new FXLabel(mainInfoFrame, "", GUIIconSubSys::getIcon(ICON_SUMO), GUIDesignLabelIcon64x64noSpacing);

    // copyright notice
    new FXLabel(this, "A microscopic, multi-modal, open source road traffic simulation.", 0, GUIDesignLabelAboutInfo);
    new FXLabel(this, "Copyright (C) 2001-2017 DLR / Institute of Transportation Systems", 0, GUIDesignLabelAboutInfo);

    // link to homepage
    FXLinkLabel* link = new FXLinkLabel(this, "http://sumo.dlr.de", 0, GUIDesignLabelCenter);
    link->setTipText("http://sumo.dlr.de");

    // centered ok-button
    FXHorizontalFrame* buttonFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(buttonFrame, "OK\t\t", GUIIconSubSys::getIcon(ICON_ACCEPT), this, ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(buttonFrame, GUIDesignAuxiliarHorizontalFrame);
}


void
GUIDialog_AboutSUMO::create() {
    FXDialogBox::create();
}


GUIDialog_AboutSUMO::~GUIDialog_AboutSUMO() {
    delete myHeadlineFont;
}



/****************************************************************************/

