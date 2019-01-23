/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBSign.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Nov 2012
/// @version $Id$
///
// A class representing a street sign
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <cassert>
#include <utils/common/RGBColor.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "NBEdge.h"
#include "NBSign.h"


// ===========================================================================
// static members
// ===========================================================================
StringBijection<NBSign::SignType>::Entry signTypeStringsInitializer[] = {
    {"speed limit",       NBSign::SIGN_TYPE_SPEED},
    {"yield",             NBSign::SIGN_TYPE_YIELD},
    {"stop",              NBSign::SIGN_TYPE_STOP},
    {"allway_stop",       NBSign::SIGN_TYPE_ALLWAY_STOP},
    {"on ramp",           NBSign::SIGN_TYPE_ON_RAMP},
    {"priority",          NBSign::SIGN_TYPE_PRIORITY},
    {"right before left", NBSign::SIGN_TYPE_RIGHT_BEFORE_LEFT},
    {"roundabout",        NBSign::SIGN_TYPE_ROUNDABOUT},
    {"rail crossing",     NBSign::SIGN_TYPE_RAIL_CROSSING},
    {"slope",             NBSign::SIGN_TYPE_SLOPE},
    {"city limits",       NBSign::SIGN_TYPE_CITY},
    {"info",              NBSign::SIGN_TYPE_INFO},
};

StringBijection<NBSign::SignType> NBSign::SignTypeStrings(
    signTypeStringsInitializer, NBSign::SIGN_TYPE_INFO);


// ===========================================================================
// member method definitions
// ===========================================================================

NBSign::NBSign(SignType type, double offset, const std::string label) :
    myType(type),
    myOffset(offset),
    myLabel(label) {
}


NBSign::~NBSign() {}


void
NBSign::writeAsPOI(OutputDevice& into, const NBEdge* edge) const {
    PositionVector shp = edge->getLanes()[0].shape;
    try {
        shp.move2side(3);
    } catch (InvalidArgument&) {
        // we do not write anything, maybe we should
    }
    Position pos = shp.positionAtOffset(myOffset);
    into.openTag(SUMO_TAG_POI);
    into.writeAttr(SUMO_ATTR_ID, edge->getID() + "." + toString(myOffset));
    into.writeAttr(SUMO_ATTR_TYPE, SignTypeStrings.getString(myType));
    switch (myType) { /// XXX @todo add default colors
        case SIGN_TYPE_SPEED:
        case SIGN_TYPE_SLOPE:
        case SIGN_TYPE_CITY:
        case SIGN_TYPE_INFO:
            into.writeAttr(SUMO_ATTR_COLOR, RGBColor::GREY);
            break;
        case SIGN_TYPE_YIELD:
        case SIGN_TYPE_STOP:
        case SIGN_TYPE_ALLWAY_STOP:
        case SIGN_TYPE_ON_RAMP:
        case SIGN_TYPE_RAIL_CROSSING:
            into.writeAttr(SUMO_ATTR_COLOR, RGBColor::RED);
            break;
        case SIGN_TYPE_PRIORITY:
            into.writeAttr(SUMO_ATTR_COLOR, RGBColor::YELLOW);
            break;
        case SIGN_TYPE_RIGHT_BEFORE_LEFT:
            into.writeAttr(SUMO_ATTR_COLOR, RGBColor(255, 153, 0, 255));
            break;
        case SIGN_TYPE_ROUNDABOUT:
            into.writeAttr(SUMO_ATTR_COLOR, RGBColor::BLUE);
            break;
    }
    into.writeAttr(SUMO_ATTR_X, pos.x());
    into.writeAttr(SUMO_ATTR_Y, pos.y());
    into.writeAttr(SUMO_ATTR_ANGLE, 0); // XXX use road angle?
    // @todo add image resources and default images for all signs
    //into.writeAttr(SUMO_ATTR_IMGFILE, p->getImgFile());
    //into.writeAttr(SUMO_ATTR_WIDTH, p->getWidth());
    //into.writeAttr(SUMO_ATTR_HEIGHT, p->getHeight());
    into.closeTag();
}


/****************************************************************************/

