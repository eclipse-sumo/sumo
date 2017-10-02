/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNENetElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// A abstract class for netElements
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include "GNENetElement.h"
#include "GNENet.h"

// ===========================================================================
// method definitions
// ===========================================================================


GNENetElement::GNENetElement(GNENet* net, const std::string& id, GUIGlObjectType type, SumoXMLTag tag, GUIIcon icon) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag, icon),
    myNet(net) {
}


GNENetElement::~GNENetElement() {}


GNENet*
GNENetElement::getNet() const {
    return myNet;
}


const std::string&
GNENetElement::getParentName() const {
    return myNet->getMicrosimID();
}


/****************************************************************************/
