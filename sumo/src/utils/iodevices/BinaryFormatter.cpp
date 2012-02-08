/****************************************************************************/
/// @file    BinaryFormatter.cpp
/// @author  Michael Behrisch
/// @date    2012
/// @version $Id$
///
// Static storage of an output device and its base (abstract) implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
    FileHelpers::writeInt(into, list.size());
    for (std::vector<std::string>::const_iterator it = list.begin(); it != list.end(); ++it) {
        FileHelpers::writeByte(into, BF_STRING);
        FileHelpers::writeString(into, *it);
    }
}

bool
BinaryFormatter::writeXMLHeader(std::ostream& into,
                                const std::string& rootElement,
                                const std::string xmlParams,
                                const std::string& attrs,
                                const std::string& comment) {
    if (myXMLStack.empty()) {
        FileHelpers::writeByte(into, BF_BYTE);
        FileHelpers::writeByte(into, 1);
        FileHelpers::writeByte(into, BF_STRING);
        FileHelpers::writeString(into, VERSION_STRING);
        writeStringList(into, SUMOXMLDefinitions::Tags.getStrings());
        writeStringList(into, SUMOXMLDefinitions::Attrs.getStrings());
        writeStringList(into, SUMOXMLDefinitions::NodeTypes.getStrings());
        writeStringList(into, SUMOXMLDefinitions::EdgeFunctions.getStrings());
        if (SUMOXMLDefinitions::Tags.hasString(rootElement)) {
            openTag(into, (const SumoXMLTag)(SUMOXMLDefinitions::Tags.get(rootElement)));
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
    FileHelpers::writeInt(into, xmlElement);
}


void
BinaryFormatter::closeOpener(std::ostream& into) {
}


bool
BinaryFormatter::closeTag(std::ostream& into, bool abbreviated) {
    if (!myXMLStack.empty()) {
        FileHelpers::writeByte(into, BF_XML_TAG_END);
        FileHelpers::writeInt(into, myXMLStack.back());
        myXMLStack.pop_back();
        return true;
    }
    return false;
}


void
BinaryFormatter::writeAttr(std::ostream& into, const std::string& attr, const std::string& val) {
    if (SUMOXMLDefinitions::Attrs.hasString(attr)) {
        writeAttr(into, (const SumoXMLAttr)(SUMOXMLDefinitions::Attrs.get(attr)), val);
    }
}


void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const SUMOReal& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_FLOAT);
    FileHelpers::writeFloat(into, val);
}


void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const int& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_INTEGER);
    FileHelpers::writeInt(into, val);
}


void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const SumoXMLNodeType& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_NODE_TYPE);
    FileHelpers::writeByte(into, val);
}


void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const SumoXMLEdgeFunc& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_EDGE_FUNCTION);
    FileHelpers::writeByte(into, val);
}


void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const Position& val) {
    if (val.z() != 0.) {
        BinaryFormatter::writeAttrHeader(into, attr, BF_POSITION_3D);
        FileHelpers::writeFloat(into, val.x());
        FileHelpers::writeFloat(into, val.y());
        FileHelpers::writeFloat(into, val.z());
    } else {
        BinaryFormatter::writeAttrHeader(into, attr, BF_POSITION_2D);
        FileHelpers::writeFloat(into, val.x());
        FileHelpers::writeFloat(into, val.y());
    }
}


void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const PositionVector& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_LIST);
    FileHelpers::writeInt(into, val.size());
    for (PositionVector::ContType::const_iterator pos = val.begin(); pos != val.end(); ++pos) {
        if (pos->z() != 0.) {
            FileHelpers::writeByte(into, BF_POSITION_3D);
            FileHelpers::writeFloat(into, pos->x());
            FileHelpers::writeFloat(into, pos->y());
            FileHelpers::writeFloat(into, pos->z());
        } else {
            FileHelpers::writeByte(into, BF_POSITION_2D);
            FileHelpers::writeFloat(into, pos->x());
            FileHelpers::writeFloat(into, pos->y());
        }
    }
}


void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const Boundary& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_LIST);
    FileHelpers::writeInt(into, 2);
    FileHelpers::writeByte(into, BF_POSITION_2D);
    FileHelpers::writeFloat(into, val.xmin());
    FileHelpers::writeFloat(into, val.ymin());
    FileHelpers::writeByte(into, BF_POSITION_2D);
    FileHelpers::writeFloat(into, val.xmax());
    FileHelpers::writeFloat(into, val.ymax());
}

/****************************************************************************/

