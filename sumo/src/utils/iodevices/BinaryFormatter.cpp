/****************************************************************************/
/// @file    BinaryFormatter.cpp
/// @author  Michael Behrisch
/// @date    2012
/// @version $Id$
///
// Static storage of an output device and its base (abstract) implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/RGBColor.h>
#include <utils/common/ToString.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Boundary.h>
#include "BinaryFormatter.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
BinaryFormatter::BinaryFormatter() {
}


void
BinaryFormatter::writeStringList(std::ostream& into, const std::vector<std::string>& list) {
    FileHelpers::writeByte(into, BF_LIST);
    FileHelpers::writeInt(into, (int)list.size());
    for (std::vector<std::string>::const_iterator it = list.begin(); it != list.end(); ++it) {
        FileHelpers::writeByte(into, BF_STRING);
        FileHelpers::writeString(into, *it);
    }
}

bool
BinaryFormatter::writeXMLHeader(std::ostream& into,
                                const std::string& rootElement,
                                const std::string& /* attrs */,
                                const std::string& /* comment */) {
    if (myXMLStack.empty()) {
        FileHelpers::writeByte(into, BF_BYTE);
        FileHelpers::writeByte(into, 1);
        FileHelpers::writeByte(into, BF_STRING);
        FileHelpers::writeString(into, VERSION_STRING);
        writeStringList(into, SUMOXMLDefinitions::Tags.getStrings());
        writeStringList(into, SUMOXMLDefinitions::Attrs.getStrings());
        writeStringList(into, SUMOXMLDefinitions::NodeTypes.getStrings());
        writeStringList(into, SUMOXMLDefinitions::EdgeFunctions.getStrings());
        writeStringList(into, std::vector<std::string>());
        writeStringList(into, std::vector<std::string>());

        if (SUMOXMLDefinitions::Tags.hasString(rootElement)) {
            openTag(into, rootElement);
            return true;
        }
    }
    return false;
}


void
BinaryFormatter::openTag(std::ostream& into, const std::string& xmlElement) {
    if (SUMOXMLDefinitions::Tags.hasString(xmlElement)) {
        openTag(into, (const SumoXMLTag)(SUMOXMLDefinitions::Tags.get(xmlElement)));
    }
}


void
BinaryFormatter::openTag(std::ostream& into, const SumoXMLTag& xmlElement) {
    myXMLStack.push_back(xmlElement);
    FileHelpers::writeByte(into, BF_XML_TAG_START);
    FileHelpers::writeByte(into, static_cast<unsigned char>(xmlElement));
}


bool
BinaryFormatter::closeTag(std::ostream& into) {
    if (!myXMLStack.empty()) {
        FileHelpers::writeByte(into, BF_XML_TAG_END);
        FileHelpers::writeByte(into, static_cast<unsigned char>(myXMLStack.back()));
        myXMLStack.pop_back();
        return true;
    }
    return false;
}


template<>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const bool& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_BYTE);
    FileHelpers::writeByte(into, val);
}


template<>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const SUMOReal& val) {
    if (into.precision() == 2 && val < 2e7 && val > -2e7) { // 2e7 is roughly INT_MAX/100
        BinaryFormatter::writeAttrHeader(into, attr, BF_SCALED2INT);
        FileHelpers::writeInt(into, int(val * 100. + .5));
    } else {
        BinaryFormatter::writeAttrHeader(into, attr, BF_FLOAT);
        FileHelpers::writeFloat(into, val);
    }
}


template<>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const int& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_INTEGER);
    FileHelpers::writeInt(into, val);
}


template<>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const unsigned int& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_INTEGER);
    FileHelpers::writeInt(into, val);
}


template<>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const SumoXMLNodeType& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_NODE_TYPE);
    FileHelpers::writeByte(into, static_cast<unsigned char>(val));
}


template<>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const SumoXMLEdgeFunc& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_EDGE_FUNCTION);
    FileHelpers::writeByte(into, static_cast<unsigned char>(val));
}


void BinaryFormatter::writePosition(std::ostream& into, const Position& val) {
    if (val.z() != 0.) {
        if (into.precision() == 2 && val.x() < 2e7 && val.x() > -2e7 &&
                val.y() < 2e7 && val.y() > -2e7 && val.z() < 2e7 && val.z() > -2e7) { // 2e7 is roughly INT_MAX/100
            FileHelpers::writeByte(into, BF_SCALED2INT_POSITION_3D);
            FileHelpers::writeInt(into, int(val.x() * 100. + .5));
            FileHelpers::writeInt(into, int(val.y() * 100. + .5));
            FileHelpers::writeInt(into, int(val.z() * 100. + .5));
        } else {
            FileHelpers::writeByte(into, BF_POSITION_3D);
            FileHelpers::writeFloat(into, val.x());
            FileHelpers::writeFloat(into, val.y());
            FileHelpers::writeFloat(into, val.z());
        }
    } else {
        if (into.precision() == 2 && val.x() < 2e7 && val.x() > -2e7 &&
                val.y() < 2e7 && val.y() > -2e7) { // 2e7 is roughly INT_MAX/100
            FileHelpers::writeByte(into, BF_SCALED2INT_POSITION_2D);
            FileHelpers::writeInt(into, int(val.x() * 100. + .5));
            FileHelpers::writeInt(into, int(val.y() * 100. + .5));
        } else {
            FileHelpers::writeByte(into, BF_POSITION_2D);
            FileHelpers::writeFloat(into, val.x());
            FileHelpers::writeFloat(into, val.y());
        }
    }
}


template<>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const Position& val) {
    FileHelpers::writeByte(into, BF_XML_ATTRIBUTE);
    FileHelpers::writeByte(into, static_cast<unsigned char>(attr));
    writePosition(into, val);
}


template<>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const PositionVector& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_LIST);
    FileHelpers::writeInt(into, static_cast<int>(val.size()));
    for (PositionVector::const_iterator pos = val.begin(); pos != val.end(); ++pos) {
        writePosition(into, *pos);
    }
}


template<>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const Boundary& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_BOUNDARY);
    FileHelpers::writeFloat(into, val.xmin());
    FileHelpers::writeFloat(into, val.ymin());
    FileHelpers::writeFloat(into, val.xmax());
    FileHelpers::writeFloat(into, val.ymax());
}


template<>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const RGBColor& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_COLOR);
    FileHelpers::writeByte(into, val.red());
    FileHelpers::writeByte(into, val.green());
    FileHelpers::writeByte(into, val.blue());
    FileHelpers::writeByte(into, val.alpha());
}


template<>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr /* attr */, const std::vector<int>& val) {
    FileHelpers::writeByte(into, BF_LIST);
    FileHelpers::writeInt(into, (int)val.size());
    for (std::vector<int>::const_iterator it = val.begin(); it != val.end(); ++it) {
        FileHelpers::writeByte(into, BF_INTEGER);
        FileHelpers::writeInt(into, *it);
    }
}


/****************************************************************************/
