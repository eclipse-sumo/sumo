/****************************************************************************/
/// @file    GNEDialog_About.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// The "About" - dialog for NETEDIT, (adapted from GUIDialog_AboutSUMO)
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

#include <utils/common/StdDefs.h>
#include <utils/foxtools/FXLinkLabel.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>
#include "GNEDialog_About.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GNEDialog_About::GNEDialog_About(FXWindow* parent) :
    FXDialogBox(parent, "About NETEDIT", GUIDesignDialogBox) {
    FXVerticalFrame* f1 = new FXVerticalFrame(this, GUIDesignAboutFrame1);

    // DLR Icon
    FXHorizontalFrame* f2 = new FXHorizontalFrame(f1, GUIDesignAboutFrame2);
    new FXLabel(f2, "\tDLR\t.", GUIIconSubSys::getIcon(ICON_DLR), GUIDesignAboutLabelIcon);

    // "NETEDIT <VERSION>"
    FXVerticalFrame* f4 = new FXVerticalFrame(f2, GUIDesignAboutFrame3);
    myHeadlineFont = new FXFont(getApp(), "Arial", 18, FXFont::Bold);
    new FXLabel(f2, "", GUIIconSubSys::getIcon(ICON_NETEDIT), GUIDesignAboutLabelIcon);
    FXLabel* l = new FXLabel(f4, "NETEDIT " VERSION_STRING, 0, GUIDesignAboutLabelDescription);
    l->setFont(myHeadlineFont);
    new FXLabel(f4, "Network editor for SUMO, the Simulation of Urban MObility", 0, GUIDesignAboutLabelDescription);
    new FXLabel(f4, HAVE_ENABLED, 0, GUIDesignAboutLabelDescription);

    // additional infos
    FXVerticalFrame* f3 = new FXVerticalFrame(f1, GUIDesignAboutFrame3);

    // copyright notice
    new FXLabel(f3, "Part of SUMO, the Simulation of Urban MObility.", 0, GUIDesignAboutLabelInformation);
    new FXLabel(f3, "Copyright (C) 2001-2017 DLR / Institute of Transportation Systems", 0, GUIDesignAboutLabelInformation);

    FXLinkLabel* link = new FXLinkLabel(f3, "http://sumo.dlr.de", 0, GUIDesignAboutLinkLabel);
    link->setTipText("http://sumo.dlr.de");

    // ok-button
    new FXButton(f1, "OK\t\t", GUIIconSubSys::getIcon(ICON_ACCEPT), this, ID_ACCEPT, GUIDesignButtonOK);
    setIcon(GUIIconSubSys::getIcon(ICON_NETEDIT));
}


void
GNEDialog_About::create() {
    FXDialogBox::create();
}


GNEDialog_About::~GNEDialog_About() {
    delete myHeadlineFont;
}


/****************************************************************************/
