/****************************************************************************/
/// @file    GUISelectionLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// Helper for loading selections
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <guisim/GUIEdge.h>
#include <microsim/MSLane.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include "GUISelectionLoader.h"
#include <fstream>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
bool
GUISelectionLoader::loadSelection(const std::string &file, std::string &msg) throw() {
    std::ifstream strm(file.c_str());
    if (!strm.good()) {
        msg = "Could not open '" + file + "'.";
        return false;
    }
    while (strm.good()) {
        std::string line;
        strm >> line;
        if (line.length()==0) {
            continue;
        }

        GUIGlObject *object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(line);
        if (object) {
            gSelected.select(object->getGlID(), false);
        } else {
            msg = "Item '" + line + "' not found";
            continue;
        }
    }
    return true;
}


/****************************************************************************/

