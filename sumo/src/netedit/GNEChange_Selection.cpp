/****************************************************************************/
/// @file    GNEChange_Selection.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2015
/// @version $Id: GNEChange_Selection.cpp 3359 2014-02-22 09:38:00Z behr_mi $
///
// A change to the network selection
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <utils/gui/div/GUIGlobalSelection.h>
#include "GNEChange_Selection.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Selection, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


// Constructor for changing selection
GNEChange_Selection::GNEChange_Selection(const std::set<GUIGlID>& selected, const std::set<GUIGlID>& deselected, bool forward):
    GNEChange(0, forward),
    mySelectedIDs(selected),
    myDeselectedIDs(deselected) {
}


GNEChange_Selection::~GNEChange_Selection() {
}


void GNEChange_Selection::undo() {
    if (myForward) {
        for (std::set<GUIGlID>::const_iterator it = mySelectedIDs.begin(); it != mySelectedIDs.end(); it++) {
            gSelected.deselect(*it);
        }
        for (std::set<GUIGlID>::const_iterator it = myDeselectedIDs.begin(); it != myDeselectedIDs.end(); it++) {
            gSelected.select(*it);
        }
    } else {
        for (std::set<GUIGlID>::const_iterator it = mySelectedIDs.begin(); it != mySelectedIDs.end(); it++) {
            gSelected.select(*it);
        }
        for (std::set<GUIGlID>::const_iterator it = myDeselectedIDs.begin(); it != myDeselectedIDs.end(); it++) {
            gSelected.deselect(*it);
        }
    }
}


void GNEChange_Selection::redo() {
    if (myForward) {
        for (std::set<GUIGlID>::const_iterator it = mySelectedIDs.begin(); it != mySelectedIDs.end(); it++) {
            gSelected.select(*it);
        }
        for (std::set<GUIGlID>::const_iterator it = myDeselectedIDs.begin(); it != myDeselectedIDs.end(); it++) {
            gSelected.deselect(*it);
        }
    } else {
        for (std::set<GUIGlID>::const_iterator it = mySelectedIDs.begin(); it != mySelectedIDs.end(); it++) {
            gSelected.deselect(*it);
        }
        for (std::set<GUIGlID>::const_iterator it = myDeselectedIDs.begin(); it != myDeselectedIDs.end(); it++) {
            gSelected.select(*it);
        }
    }
}


FXString GNEChange_Selection::undoName() const {
    if (myForward) {
        return ("Undo change selection");
    } else {
        return ("Undo change selection");
    }
}


FXString GNEChange_Selection::redoName() const {
    if (myForward) {
        return ("Redo change selection");
    } else {
        return ("Redo change selection");
    }
}
