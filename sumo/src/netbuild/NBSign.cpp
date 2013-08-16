/****************************************************************************/
/// @file    NBSign.cpp
/// @author  Jakob Erdmann
/// @date    Nov 2012
/// @version $Id$
///
// A class representing a street sign
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "NBEdge.h"
#include "NBSign.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
StringBijection<NBSign::SignType>::Entry signTypeStringsInitializaer[] = {
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
    signTypeStringsInitializaer, NBSign::SIGN_TYPE_INFO);


StringBijection<NBSign::SignType>::Entry SignTypeColorsInitializaer[] = {
    {"0.5,0.5,0.5", NBSign::SIGN_TYPE_SPEED},
    {"1.0,0.0,0.0", NBSign::SIGN_TYPE_YIELD},
    {"1.0,0.0,0.0", NBSign::SIGN_TYPE_STOP},
    {"1.0,0.0,0.0", NBSign::SIGN_TYPE_ALLWAY_STOP},
    {"1.0,0.0,0.0", NBSign::SIGN_TYPE_ON_RAMP},
    {"1.0,1.0,0.0", NBSign::SIGN_TYPE_PRIORITY},
    {"1.0,0.6,0.0", NBSign::SIGN_TYPE_RIGHT_BEFORE_LEFT},
    {"0.0,0.0,1.0", NBSign::SIGN_TYPE_ROUNDABOUT},
    {"1.0,0.0,0.0", NBSign::SIGN_TYPE_RAIL_CROSSING},
    {"0.5,0.5,0.5", NBSign::SIGN_TYPE_SLOPE},
    {"0.5,0.5,0.5", NBSign::SIGN_TYPE_CITY},
    {"0.5,0.5,0.5", NBSign::SIGN_TYPE_INFO},
};

StringBijection<NBSign::SignType> NBSign::SignTypeColors(
    SignTypeColorsInitializaer, NBSign::SIGN_TYPE_INFO);

// ===========================================================================
// member method definitions
// ===========================================================================

NBSign::NBSign(SignType type, SUMOReal offset, const std::string label) :
    myType(type),
    myOffset(offset),
    myLabel(label)
{ }


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
    into.writeAttr(SUMO_ATTR_COLOR, SignTypeColors.getString(myType)); /// XXX @todo add default colors
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

