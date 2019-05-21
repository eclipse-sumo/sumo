/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEXMLChild.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
/// @version $Id$
///
// A abstract class for representation of elements that are written within another element
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/options/OptionsCont.h>

#include "GNEXMLChild.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEXMLChild::GNEXMLChild(GNEAttributeCarrier* AC) :
    myACParent(nullptr),
    myACChild(nullptr),
    myAC(AC) {
}


GNEXMLChild::~GNEXMLChild() {}


GNEXMLChild* 
GNEXMLChild::getXMLParent() const {
    return myACParent;
}


GNEXMLChild* 
GNEXMLChild::getXMLChild() const {
    return myACChild;
}

void 
GNEXMLChild::setXMLChild(GNEXMLChild* ACParent) {
    // first remove reference in old ace parent
    if (myACParent) {
        myACParent->myACChild = nullptr;
    }
    myACParent = ACParent;
    if (myACParent) {
        myACParent->myACChild = this;
    }
}

/****************************************************************************/
