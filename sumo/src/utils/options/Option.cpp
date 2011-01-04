/****************************************************************************/
/// @file    Option.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// A class representing a single program option
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

#include <algorithm>
#include <string>
#include <exception>
#include <sstream>
#include "Option.h"
#include <utils/common/TplConvert.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * Option - methods
 * ----------------------------------------------------------------------- */
Option::Option(bool set) throw()
        : myAmSet(set), myHaveTheDefaultValue(true), myAmWritable(true) {}


Option::Option(const Option &s) throw()
        : myAmSet(s.myAmSet), myHaveTheDefaultValue(s.myHaveTheDefaultValue),
        myAmWritable(s.myAmWritable) {}


Option::~Option() throw() {}


Option &
Option::operator=(const Option &s) throw() {
    if (this==&s) {
        return *this;
    }
    myAmSet = s.myAmSet;
    myHaveTheDefaultValue = s.myHaveTheDefaultValue;
    myAmWritable = s.myAmWritable;
    return *this;
}


bool
Option::isSet() const throw() {
    return myAmSet;
}


SUMOReal
Option::getFloat() const throw(InvalidArgument) {
    throw InvalidArgument("This is not a SUMOReal-option");
}


int
Option::getInt() const throw(InvalidArgument) {
    throw InvalidArgument("This is not an int-option");
}


std::string
Option::getString() const throw(InvalidArgument) {
    throw InvalidArgument("This is not a string-option");
}


bool
Option::getBool() const throw(InvalidArgument) {
    throw InvalidArgument("This is not a bool-option");
}


const IntVector &
Option::getIntVector() const throw(InvalidArgument) {
    throw InvalidArgument("This is not an int vector-option");
}


bool
Option::markSet() throw() {
    bool ret = myAmWritable;
    myHaveTheDefaultValue = false;
    myAmSet = true;
    myAmWritable = false;
    return ret;
}


bool
Option::isBool() const throw() {
    return false;
}


bool
Option::isDefault() const throw() {
    return myHaveTheDefaultValue;
}


bool
Option::isFileName() const throw() {
    return false;
}


bool
Option::isWriteable() const throw() {
    return myAmWritable;
}


void
Option::resetWritable() throw() {
    myAmWritable = true;
}


const std::string &
Option::getDescription() const throw() {
    return myDescription;
}


void
Option::setDescription(const std::string &desc) throw() {
    myDescription = desc;
}


const std::string &
Option::getTypeName() const throw() {
    return myTypeName;
}




/* -------------------------------------------------------------------------
 * Option_Integer - methods
 * ----------------------------------------------------------------------- */
Option_Integer::Option_Integer() throw()
        : Option() {
    myTypeName = "INT";
}


Option_Integer::Option_Integer(int value) throw()
        : Option(true), myValue(value) {
    myTypeName = "INT";
}


Option_Integer::~Option_Integer() throw() {}


Option_Integer::Option_Integer(const Option_Integer &s) throw()
        : Option(s) {
    myValue = s.myValue;
}


Option_Integer &
Option_Integer::operator=(const Option_Integer &s) throw() {
    if (this==&s) return *this;
    Option::operator=(s);
    myValue = s.myValue;
    return *this;
}


int
Option_Integer::getInt() const throw(InvalidArgument) {
    return myValue;
}


bool
Option_Integer::set(const std::string &v) throw(InvalidArgument) {
    try {
        myValue = TplConvert<char>::_2int(v.c_str());
        return markSet();
    } catch (...) {
        std::string s = "'" + v + "' is not a valid integer (should be).";
        throw InvalidArgument(s);
    }
}


std::string
Option_Integer::getValueString() const throw(InvalidArgument) {
    std::ostringstream s;
    s << myValue;
    return s.str();
}



/* -------------------------------------------------------------------------
 * Option_String - methods
 * ----------------------------------------------------------------------- */
Option_String::Option_String() throw()
        : Option() {
    myTypeName = "STR";
}


Option_String::Option_String(const std::string &value, std::string typeName) throw()
        : Option(true), myValue(value) {
    myTypeName = typeName;
}


Option_String::~Option_String() throw() {}


Option_String::Option_String(const Option_String &s) throw()
        : Option(s) {
    myValue = s.myValue;
}


Option_String &
Option_String::operator=(const Option_String &s) throw() {
    if (this==&s) {
        return *this;
    }
    Option::operator=(s);
    myValue = s.myValue;
    return *this;
}


std::string
Option_String::getString() const throw(InvalidArgument) {
    return myValue;
}


bool
Option_String::set(const std::string &v) throw(InvalidArgument) {
    myValue = v;
    return markSet();
}


std::string
Option_String::getValueString() const throw(InvalidArgument) {
    return myValue;
}



/* -------------------------------------------------------------------------
 * Option_Float - methods
 * ----------------------------------------------------------------------- */
Option_Float::Option_Float() throw()
        : Option() {
    myTypeName = "FLOAT";
}


Option_Float::Option_Float(SUMOReal value) throw()
        : Option(true), myValue(value) {
    myTypeName = "FLOAT";
}


Option_Float::~Option_Float() throw() {}


Option_Float::Option_Float(const Option_Float &s) throw()
        : Option(s) {
    myValue = s.myValue;
}


Option_Float &
Option_Float::operator=(const Option_Float &s) throw() {
    if (this==&s) {
        return *this;
    }
    Option::operator=(s);
    myValue = s.myValue;
    return *this;
}


SUMOReal
Option_Float::getFloat() const throw(InvalidArgument) {
    return myValue;
}


bool
Option_Float::set(const std::string &v) throw(InvalidArgument) {
    try {
        myValue = TplConvert<char>::_2SUMOReal(v.c_str());
        return markSet();
    } catch (...) {
        std::string s = "'" + v + "' is not a valid float (should be).";
        throw InvalidArgument(s);
    }
}


std::string
Option_Float::getValueString() const throw(InvalidArgument) {
    std::ostringstream s;
    s << myValue;
    return s.str();
}



/* -------------------------------------------------------------------------
 * Option_Bool - methods
 * ----------------------------------------------------------------------- */
Option_Bool::Option_Bool() throw()
        : Option() {
    myTypeName = "BOOL";
}


Option_Bool::Option_Bool(bool value) throw()
        : Option(true), myValue(value) {
    myTypeName = "BOOL";
}


Option_Bool::~Option_Bool() throw() {}


Option_Bool::Option_Bool(const Option_Bool &s) throw()
        : Option(s) {
    myValue = s.myValue;
}


Option_Bool &
Option_Bool::operator=(const Option_Bool &s) throw() {
    if (this==&s) {
        return *this;
    }
    Option::operator=(s);
    myValue = s.myValue;
    return *this;
}


bool
Option_Bool::getBool() const throw(InvalidArgument) {
    return myValue;
}


bool
Option_Bool::set(const std::string &v) throw(InvalidArgument) {
    std::string value = v;
    std::transform(value.begin(), value.end(), value.begin(), tolower);
    if (value=="1"||value=="yes"||value=="true"||value=="on"||value=="x") {
        myValue = true;
    } else if (value=="0"||value=="no"||value=="false"||value=="off") {
        myValue = false;
    } else {
        throw InvalidArgument("Invalid boolean value for option.");
    }
    return markSet();
}


std::string
Option_Bool::getValueString() const throw(InvalidArgument) {
    if (myValue) {
        return "true";
    }
    return "false";
}


bool
Option_Bool::isBool() const throw() {
    return true;
}



/* -------------------------------------------------------------------------
 * Option_FileName - methods
 * ----------------------------------------------------------------------- */
Option_FileName::Option_FileName() throw()
        : Option_String() {
    myTypeName = "FILE";
}


Option_FileName::Option_FileName(const std::string &value) throw()
        : Option_String(value) {
    myTypeName = "FILE";
}


Option_FileName::Option_FileName(const Option_String &s) throw()
        : Option_String(s) {}


Option_FileName::~Option_FileName() throw() {}


Option_FileName &
Option_FileName::operator=(const Option_FileName &s) throw() {
    Option_String::operator=(s);
    return (*this);
}


bool
Option_FileName::isFileName() const throw() {
    return true;
}



/* -------------------------------------------------------------------------
 * Option_UIntVector - methods
 * ----------------------------------------------------------------------- */
Option_IntVector::Option_IntVector() throw()
        : Option() {
    myTypeName = "INT[]";
}


Option_IntVector::Option_IntVector(const IntVector &value) throw()
        : Option(true), myValue(value) {
    myTypeName = "INT[]";
}


Option_IntVector::Option_IntVector(const Option_IntVector &s) throw()
        : Option(s), myValue(s.myValue) {}


Option_IntVector::~Option_IntVector() throw() {}


Option_IntVector &
Option_IntVector::operator=(const Option_IntVector &s) throw() {
    Option::operator=(s);
    myValue = s.myValue;
    return (*this);
}


const IntVector &
Option_IntVector::getIntVector() const throw(InvalidArgument) {
    return myValue;
}


bool
Option_IntVector::set(const std::string &v) throw(InvalidArgument) {
    myValue.clear();
    try {
        if (v.find(';')!=std::string::npos) {
            MsgHandler::getWarningInstance()->inform("Please note that using ';' as list separator is deprecated.\n From 1.0 onwards, only ',' will be accepted.");
        }
        StringTokenizer st(v, ";,", true);
        while (st.hasNext()) {
            myValue.push_back(TplConvert<char>::_2int(st.next().c_str()));
        }
        return markSet();
    } catch (EmptyData &) {
        throw InvalidArgument("Empty element occured in " + v);
    } catch (...) {
        throw InvalidArgument("'" + v + "' is not a valid integer vector.");
    }
}


std::string
Option_IntVector::getValueString() const throw(InvalidArgument) {
    std::ostringstream s;
    for (IntVector::const_iterator i=myValue.begin(); i!=myValue.end(); i++) {
        if (i!=myValue.begin()) {
            s << ',';
        }
        s << (*i);
    }
    return s.str();
}



/****************************************************************************/

