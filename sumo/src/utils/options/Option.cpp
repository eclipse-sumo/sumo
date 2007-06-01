/****************************************************************************/
/// @file    Option.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// A class representing a single program option
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * Option - methods
 * ----------------------------------------------------------------------- */
Option::Option(bool set)
            : myAmSet(set), myHaveTheDefaultValue(true), myAmWritable(true),
            myAmMandatory(false)
{}


Option::Option(const Option &s)
        : myAmSet(s.myAmSet), myHaveTheDefaultValue(s.myHaveTheDefaultValue),
        myAmWritable(s.myAmWritable),
        myAmMandatory(false)
{}


Option::~Option()
{}


Option &
Option::operator=(const Option &s)
{
    if (this==&s) {
        return *this;
    }
    myAmSet = s.myAmSet;
    myHaveTheDefaultValue = s.myHaveTheDefaultValue;
    myAmWritable = s.myAmWritable;
    return *this;
}


bool
Option::isSet() const
{
    return myAmSet;
}


SUMOReal
Option::getFloat() const
{
    throw InvalidArgument("This is not a SUMOReal-option");
}


long
Option::getLong() const
{
    throw InvalidArgument("This is not a long-option");
}


int
Option::getInt() const
{
    throw InvalidArgument("This is not an int-option");
}


string
Option::getString() const
{
    throw InvalidArgument("This is not a string-option");
}


bool
Option::getBool() const
{
    throw InvalidArgument("This is not a bool-option");
}


const IntVector &
Option::getIntVector() const
{
    throw InvalidArgument("This is not an int vector-option");
}


bool
Option::set(string, bool)
{
    throw InvalidArgument("This is an abstract class.");
}


bool
Option::set(bool, bool)
{
    throw InvalidArgument("This is an abstract class.");
}


bool
Option::markSet(bool isDefault)
{
    bool ret = myAmWritable;
    myHaveTheDefaultValue = isDefault;
    myAmSet = true;
    myAmWritable = isDefault;
    return ret;
}


string
Option::getValue() const
{
    throw InvalidArgument("This is an abstract class.");
}


bool
Option::isBool() const
{
    return false;
}


bool
Option::isDefault() const
{
    return myHaveTheDefaultValue;
}


bool
Option::isFileName() const
{
    return false;
}


bool
Option::isWriteable() const
{
    return myAmWritable;
}


const std::string &
Option::getDescription() const
{
    return myDescription;
}


const std::string &
Option::getTypeName() const
{
    return myTypeName;
}


bool
Option::isMandatory() const
{
    return myAmMandatory;
}



/* -------------------------------------------------------------------------
 * Option_Integer - methods
 * ----------------------------------------------------------------------- */
Option_Integer::Option_Integer()
        : Option()
{
    myTypeName = "INT";
}


Option_Integer::Option_Integer(int value)
        : Option(true), myValue(value)
{
    myTypeName = "INT";
}


Option_Integer::~Option_Integer()
{}


Option_Integer::Option_Integer(const Option_Integer &s)
        : Option(s)
{
    myValue = s.myValue;
}


Option_Integer &
Option_Integer::operator=(const Option_Integer &s)
{
    if (this==&s) return *this;
    Option::operator=(s);
    myValue = s.myValue;
    return *this;
}


int
Option_Integer::getInt() const
{
    return myValue;
}


bool
Option_Integer::set(string v, bool isDefault)
{
    try {
        myValue = TplConvert<char>::_2int(v.c_str());
        return markSet(isDefault);
    } catch (...) {
        string s = "'" + v + "' is not a valid integer (should be).";
        throw InvalidArgument(s);
    }
}


string
Option_Integer::getValue() const
{
    ostringstream s;
    s << myValue;
    return s.str();
}



/* -------------------------------------------------------------------------
 * Option_Long - methods
 * ----------------------------------------------------------------------- */
Option_Long::Option_Long()
        : Option()
{
    myTypeName = "LONG";
}


Option_Long::Option_Long(long value)
        : Option(true), myValue(value)
{
    myTypeName = "LONG";
}


Option_Long::~Option_Long()
{}


Option_Long::Option_Long(const Option_Long &s)
        : Option(s)
{
    myValue = s.myValue;
}


Option_Long &
Option_Long::operator=(const Option_Long &s)
{
    if (this==&s) {
        return *this;
    }
    Option::operator=(s);
    myValue = s.myValue;
    return *this;
}


long
Option_Long::getLong() const
{
    return myValue;
}


bool
Option_Long::set(string v, bool isDefault)
{
    try {
        myValue = TplConvert<char>::_2long(v.c_str());
        return markSet(isDefault);
    } catch (...) {
        string s = "'" + v + "' is not a valid long (should be).";
        throw InvalidArgument(s);
    }
}


string
Option_Long::getValue() const
{
    ostringstream s;
    s << myValue;
    return s.str();
}



/* -------------------------------------------------------------------------
 * Option_String - methods
 * ----------------------------------------------------------------------- */
Option_String::Option_String()
        : Option()
{
    myTypeName = "STR";
}


Option_String::Option_String(string value)
        : Option(true), myValue(value)
{
    myTypeName = "STR";
}


Option_String::~Option_String()
{}


Option_String::Option_String(const Option_String &s)
        : Option(s)
{
    myValue = s.myValue;
}


Option_String &
Option_String::operator=(const Option_String &s)
{
    if (this==&s) {
        return *this;
    }
    Option::operator=(s);
    myValue = s.myValue;
    return *this;
}


string
Option_String::getString() const
{
    return myValue;
}


bool
Option_String::set(string v, bool isDefault)
{
    myValue = v;
    return markSet(isDefault);
}


string
Option_String::getValue() const
{
    return myValue;
}



/* -------------------------------------------------------------------------
 * Option_Float - methods
 * ----------------------------------------------------------------------- */
Option_Float::Option_Float()
        : Option()
{
    myTypeName = "FLOAT";
}


Option_Float::Option_Float(SUMOReal value)
        : Option(true), myValue(value)
{
    myTypeName = "FLOAT";
}


Option_Float::~Option_Float()
{}


Option_Float::Option_Float(const Option_Float &s)
        : Option(s)
{
    myValue = s.myValue;
}


Option_Float &
Option_Float::operator=(const Option_Float &s)
{
    if (this==&s) {
        return *this;
    }
    Option::operator=(s);
    myValue = s.myValue;
    return *this;
}


SUMOReal
Option_Float::getFloat() const
{
    return myValue;
}


bool
Option_Float::set(string v, bool isDefault)
{
    try {
        myValue = TplConvert<char>::_2SUMOReal(v.c_str());
        return markSet(isDefault);
    } catch (...) {
        string s = "'" + v + "' is not a valid float (should be).";
        throw InvalidArgument(s);
    }
}


string
Option_Float::getValue() const
{
    ostringstream s;
    s << myValue;
    return s.str();
}



/* -------------------------------------------------------------------------
 * Option_Bool - methods
 * ----------------------------------------------------------------------- */
Option_Bool::Option_Bool()
        : Option()
{
    myTypeName = "BOOL";
}


Option_Bool::Option_Bool(bool value)
        : Option(true), myValue(value)
{
    myTypeName = "BOOL";
}


Option_Bool::~Option_Bool()
{}


Option_Bool::Option_Bool(const Option_Bool &s)
        : Option(s)
{
    myValue = s.myValue;
}


Option_Bool &
Option_Bool::operator=(const Option_Bool &s)
{
    if (this==&s) {
        return *this;
    }
    Option::operator=(s);
    myValue = s.myValue;
    return *this;
}


bool
Option_Bool::getBool() const
{
    return myValue;
}


bool
Option_Bool::set(bool v, bool isDefault)
{
    myValue = v;
    return markSet(isDefault);
}


string
Option_Bool::getValue() const
{
    if (myValue) {
        return "true";
    }
    return "false";
}


bool
Option_Bool::isBool() const
{
    return true;
}



/* -------------------------------------------------------------------------
 * Option_FileName - methods
 * ----------------------------------------------------------------------- */
Option_FileName::Option_FileName()
        : Option_String()
{
    myTypeName = "FILE";
}


Option_FileName::Option_FileName(std::string value)
        : Option_String(value)
{
    myTypeName = "FILE";
}


Option_FileName::Option_FileName(const Option_String &s)
        : Option_String(s)
{}


Option_FileName::~Option_FileName()
{}


Option_FileName &
Option_FileName::operator=(const Option_FileName &s)
{
    Option_String::operator=(s);
    return (*this);
}


bool
Option_FileName::isFileName() const
{
    return true;
}



/* -------------------------------------------------------------------------
 * Option_UIntVector - methods
 * ----------------------------------------------------------------------- */
Option_IntVector::Option_IntVector()
        : Option()
{
    myTypeName = "INT[]";
}


Option_IntVector::Option_IntVector(const IntVector &value)
        : Option(true), myValue(value)
{
    myTypeName = "INT[]";
}


Option_IntVector::Option_IntVector(const string &value)
        : Option(true), myValue()
{
    set(value, true);
    myTypeName = "INT[]";
}


Option_IntVector::Option_IntVector(const Option_IntVector &s)
        : Option(s), myValue(s.myValue)
{}


Option_IntVector::~Option_IntVector()
{}


Option_IntVector &
Option_IntVector::operator=(const Option_IntVector &s)
{
    Option::operator=(s);
    myValue = s.myValue;
    return (*this);
}


const IntVector &
Option_IntVector::getIntVector() const
{
    return myValue;
}


bool
Option_IntVector::set(std::string v, bool isDefault)
{
    myValue.clear();
    try {
        if(v.find(';')!=string::npos) {
            MsgHandler::getWarningInstance()->inform("Please note that using ';' as list separator is deprecated.\n From 1.0 onwards, only ',' will be accepted.");
        }
        StringTokenizer st(v, ";,", true);
        while (st.hasNext()) {
            myValue.push_back(TplConvert<char>::_2int(st.next().c_str()));
        }
        return markSet(isDefault);
    } catch (EmptyData &) {
        throw InvalidArgument("Empty element occured in " + v);
    } catch (...) {
        throw InvalidArgument("'" + v + "' is not a valid integer vector.");
    }
    return false;
}


std::string
Option_IntVector::getValue() const
{
    ostringstream s;
    for (IntVector::const_iterator i=myValue.begin(); i!=myValue.end(); i++) {
        if (i!=myValue.begin()) {
            s << ' ';
        }
        s << (*i);
    }
    return s.str();
}


std::string
Option_IntVector::getString() const
{
    return getValue();
}



/****************************************************************************/

