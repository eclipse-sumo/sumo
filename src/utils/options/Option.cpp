/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    Option.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 17 Dec 2001
///
// A class representing a single program option
/****************************************************************************/
#include <config.h>

#include <string>
#include <exception>
#include <sstream>
#include "Option.h"
#include <utils/common/StringUtils.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>


// ===========================================================================
// method definitions
// ===========================================================================

// -------------------------------------------------------------------------
// Option - methods
// -------------------------------------------------------------------------

Option::Option(bool set) :
    myAmSet(set) {
}


Option::~Option() {}


bool
Option::isSet() const {
    return myAmSet;
}


double
Option::getFloat() const {
    throw InvalidArgument("This is not a double-option");
}


int
Option::getInt() const {
    throw InvalidArgument("This is not an int-option");
}


std::string
Option::getString() const {
    throw InvalidArgument("This is not a string-option");
}


bool
Option::getBool() const {
    throw InvalidArgument("This is not a bool-option");
}


const IntVector&
Option::getIntVector() const {
    throw InvalidArgument("This is not an int vector-option");
}


const StringVector&
Option::getStringVector() const {
    throw InvalidArgument("This is not a string vector-option");
}


bool
Option::markSet(const std::string& orig) {
    bool ret = myAmWritable;
    myHaveTheDefaultValue = false;
    myAmSet = true;
    myAmWritable = false;
    myValueString = orig;
    return ret;
}


const std::string&
Option::getValueString() const {
    return myValueString;
}


bool
Option::isDefault() const {
    return myHaveTheDefaultValue;
}


bool
Option::isInteger() const {
    return false;
}


bool
Option::isFloat() const {
    return false;
}


bool
Option::isBool() const {
    return false;
}


bool
Option::isFileName() const {
    return false;
}


bool
Option::isNetwork() const {
    return false;
}


bool
Option::isAdditional() const {
    return false;
}


bool
Option::isRoute() const {
    return false;
}


bool
Option::isData() const {
    return false;
}


bool
Option::isWriteable() const {
    return myAmWritable;
}


void
Option::resetWritable() {
    myAmWritable = true;
}


void
Option::resetDefault() {
    myHaveTheDefaultValue = true;
}


const std::string&
Option::getDescription() const {
    return myDescription;
}


void
Option::setDescription(const std::string& desc) {
    myDescription = desc;
}


bool
Option::isRequired() const {
    return myRequired;
}


void
Option::setRequired() {
    myRequired = true;
}

bool
Option::isPositional() const {
    return myPositional;
}

void
Option::setPositional() {
    myPositional = true;
}

const std::string&
Option::getListSeparator() const {
    return myListSeparator;
}

void
Option::setListSeparator(const std::string& listSep) {
    myListSeparator = listSep;
}

const std::string&
Option::getSubTopic() const {
    return mySubTopic;
}


void
Option::setSubtopic(const std::string& subtopic) {
    mySubTopic = subtopic;
}


const std::string&
Option::getTypeName() const {
    return myTypeName;
}

// -------------------------------------------------------------------------
// Option_Integer - methods
// -------------------------------------------------------------------------

Option_Integer::Option_Integer(int value) :
    Option(true),
    myValue(value) {
    myTypeName = "INT";
    myValueString = toString(value);
}


int
Option_Integer::getInt() const {
    return myValue;
}


bool
Option_Integer::set(const std::string& v, const std::string& orig, const bool /* append */) {
    try {
        myValue = StringUtils::toInt(v);
        return markSet(orig);
    } catch (...) {
        std::string s = "'" + v + "' is not a valid integer.";
        throw ProcessError(s);
    }
}


bool
Option_Integer::isInteger() const {
    return true;
}

// -------------------------------------------------------------------------
// Option_String - methods
// -------------------------------------------------------------------------

Option_String::Option_String() :
    Option() {
    myTypeName = "STR";
}


Option_String::Option_String(const std::string& value, std::string typeName) :
    Option(true),
    myValue(value) {
    myTypeName = typeName;
    myValueString = value;
}


std::string
Option_String::getString() const {
    return myValue;
}


bool
Option_String::set(const std::string& v, const std::string& orig, const bool /* append */) {
    myValue = v;
    return markSet(orig);
}

// -------------------------------------------------------------------------
// Option_Float - methods
// -------------------------------------------------------------------------

Option_Float::Option_Float(double value) :
    Option(true),
    myValue(value) {
    myTypeName = "FLOAT";
    std::ostringstream oss;
    oss << value;
    myValueString = oss.str();
}


double
Option_Float::getFloat() const {
    return myValue;
}


bool
Option_Float::set(const std::string& v, const std::string& orig, const bool /* append */) {
    try {
        myValue = StringUtils::toDouble(v);
        return markSet(orig);
    } catch (...) {
        throw ProcessError(TLF("'%' is not a valid float.", v));
    }
}


bool
Option_Float::isFloat() const {
    return true;
}

// -------------------------------------------------------------------------
// Option_Bool - methods
// -------------------------------------------------------------------------

Option_Bool::Option_Bool(bool value) :
    Option(true),
    myValue(value) {
    myTypeName = "BOOL";
    myValueString = value ? "true" : "false";
}


bool
Option_Bool::getBool() const {
    return myValue;
}


bool
Option_Bool::set(const std::string& v, const std::string& orig, const bool /* append */) {
    try {
        myValue = StringUtils::toBool(v);
        return markSet(orig);
    } catch (...) {
        throw ProcessError(TLF("'%' is not a valid bool.", v));
    }
}


bool
Option_Bool::isBool() const {
    return true;
}

// -------------------------------------------------------------------------
// Option_BoolExtended - methods
// -------------------------------------------------------------------------

Option_BoolExtended::Option_BoolExtended(bool value) :
    Option_Bool(value) {
}


bool
Option_BoolExtended::set(const std::string& v, const std::string& orig, const bool /* append */) {
    try {
        myValue = StringUtils::toBool(v);
        return markSet("");
    } catch (...) {
        myValue = true;
    }
    return markSet(orig);
}

// -------------------------------------------------------------------------
// Option_IntVector - methods
// -------------------------------------------------------------------------

Option_IntVector::Option_IntVector() :
    Option() {
    myTypeName = "INT[]";
}


Option_IntVector::Option_IntVector(const IntVector& value)
    : Option(true), myValue(value) {
    myTypeName = "INT[]";
    myValueString = joinToString(value, ",");
}


const IntVector&
Option_IntVector::getIntVector() const {
    return myValue;
}


bool
Option_IntVector::set(const std::string& v, const std::string& orig, const bool append) {
    if (!append) {
        myValue.clear();
    }
    try {
        if (v.find(';') != std::string::npos) {
            WRITE_WARNING(TL("Please note that using ';' as list separator is deprecated and not accepted anymore."));
        }
        StringTokenizer st(v, ",", true);
        while (st.hasNext()) {
            myValue.push_back(StringUtils::toInt(st.next()));
        }
        return markSet(orig);
    } catch (EmptyData&) {
        throw ProcessError("Empty element occurred in " + v);
    } catch (...) {
        throw ProcessError(TLF("'%' is not a valid integer vector.", v));
    }
}

// -------------------------------------------------------------------------
// Option_StringVector - methods
// -------------------------------------------------------------------------

Option_StringVector::Option_StringVector() :
    Option() {
    myTypeName = "STR[]";
}


Option_StringVector::Option_StringVector(const StringVector& value) :
    Option(true), myValue(value) {
    myTypeName = "STR[]";
    myValueString = joinToString(value, ",");
}


const StringVector&
Option_StringVector::getStringVector() const {
    return myValue;
}


bool
Option_StringVector::set(const std::string& v, const std::string& orig, const bool append) {
    if (!append) {
        myValue.clear();
    }
    StringTokenizer st(v, ",");
    while (st.hasNext()) {
        myValue.push_back(StringUtils::prune(st.next()));
    }
    return markSet(append && getValueString() != "" ? getValueString() + "," + orig : orig);
}

// -------------------------------------------------------------------------
// Option_FileName - methods
// -------------------------------------------------------------------------

Option_FileName::Option_FileName() :
    Option_StringVector() {
    myTypeName = "FILE";
}


Option_FileName::Option_FileName(const StringVector& value) :
    Option_StringVector(value) {
    myTypeName = "FILE";
}


bool
Option_FileName::isFileName() const {
    return true;
}


std::string
Option_FileName::getString() const {
    return joinToString(getStringVector(), ",");
}

// -------------------------------------------------------------------------
// Option_Network - methods
// -------------------------------------------------------------------------

Option_Network::Option_Network(const std::string& value) :
    Option_String(value, "NETWORK") {
}


bool Option_Network::isNetwork() const {
    return true;
}


// -------------------------------------------------------------------------
// Option_Additional - methods
// -------------------------------------------------------------------------

Option_Additional::Option_Additional(const std::string& value) :
    Option_String(value, "ADDITIONAL") {
}


bool
Option_Additional::isAdditional() const {
    return true;
}


// -------------------------------------------------------------------------
// Option_Route - methods
// -------------------------------------------------------------------------

Option_Route::Option_Route(const std::string& value) :
    Option_String(value, "ROUTE") {
}


bool
Option_Route::isRoute() const {
    return true;
}


// -------------------------------------------------------------------------
// Option_Data - methods
// -------------------------------------------------------------------------

Option_Data::Option_Data(const std::string& value) :
    Option_String(value, "DATA") {
}


bool
Option_Data::isData() const {
    return true;
}

/****************************************************************************/
