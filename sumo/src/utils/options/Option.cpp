/***************************************************************************
                          Option.cpp
			  A class representing a single program option
			  together with her derivates to represent different
			  value types
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2002/10/16 14:58:18  dkrajzew
// initial release for utilities that handle program options
//
// Revision 1.6  2002/07/31 17:30:06  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.6  2002/07/11 07:42:58  dkrajzew
// Usage of relative pathnames within configuration files implemented
//
// Revision 1.6  2002/06/21 10:47:47  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.5  2002/06/11 15:58:25  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/10 06:54:30  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized; options now recognize false numerical input
//
// Revision 1.3  2002/05/14 04:45:49  dkrajzew
// Bresenham added; some minor changes; windows eol removed
//
// Revision 1.2  2002/04/26 10:08:38  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.4  2002/04/09 12:20:37  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.3  2002/03/22 10:59:37  dkrajzew
// Memory leak tracing added; ostrstreams replaces by ostringstreams
//
// Revision 1.2  2002/03/20 08:50:37  dkrajzew
// Revisions patched
//
// Revision 1.1  2002/02/13 15:48:18  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <exception>
#include <sstream>
#include "Option.h"
#include <utils/convert/TplConvert.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>

/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * Option - methods
 * ----------------------------------------------------------------------- */
Option::Option(bool set) 
    : _set(set), _default(true)
{
}


Option::Option(const Option &s) 
    : _set(s._set) 
{
}


Option::~Option() 
{
}


Option &Option::operator=(const Option &s) 
{
    if(this==&s) {
        return *this;
    }
    _set = s._set;
    return *this;
}


bool 
Option::isSet() const 
{
    return _set;
}


float 
Option::getFloat() const 
{
    throw InvalidArgument("This is not a float-option");
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


const UIntVector &
Option::getUIntVector() const
{
    throw InvalidArgument("This is not a bool-option");
}


bool 
Option::set(string v, bool isDefault) 
{
    v = v;
    isDefault = isDefault;
    throw InvalidArgument("This is an abstract class.");
}


bool 
Option::set(bool v, bool isDefault) 
{
    v = v;
    isDefault = isDefault;
    throw InvalidArgument("This is an abstract class.");
}


bool 
Option::markSet(bool isDefault) 
{
    bool ret = _default;
    _default = isDefault;
    _set = true;
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
    return _default;
}


bool
Option::isFileName() const 
{
    return false;
}



/* -------------------------------------------------------------------------
 * Option_Integer - methods
 * ----------------------------------------------------------------------- */
Option_Integer::Option_Integer() 
    : Option() 
{
}


Option_Integer::Option_Integer(int value) 
    : Option(true), _value(value) 
{
}


Option_Integer::~Option_Integer() 
{
}


Option_Integer::Option_Integer(const Option_Integer &s) 
    : Option(s) 
{
    _value = s._value;
}


Option_Integer &
Option_Integer::operator=(const Option_Integer &s) 
{
    if(this==&s) return *this;
    Option::operator=(s);
    _value = s._value;
    return *this;
}


int 
Option_Integer::getInt() const 
{
    return _value;
}


bool 
Option_Integer::set(string v, bool isDefault) 
{
    try {
        _value = TplConvert<char>::_2int(v.c_str());
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
    s << _value;
    return string(s.str());
}



/* -------------------------------------------------------------------------
 * Option_Long - methods
 * ----------------------------------------------------------------------- */
Option_Long::Option_Long() 
    : Option() 
{
}


Option_Long::Option_Long(long value) 
    : Option(true), _value(value) 
{
}


Option_Long::~Option_Long() 
{
}


Option_Long::Option_Long(const Option_Long &s) 
    : Option(s) 
{
    _value = s._value;
}


Option_Long &
Option_Long::operator=(const Option_Long &s) 
{
    if(this==&s) {
        return *this;
    }
    Option::operator=(s);
    _value = s._value;
    return *this;
}


long 
Option_Long::getLong() const 
{
    return _value;
}


bool 
Option_Long::set(string v, bool isDefault) 
{
    try {
	    _value = TplConvert<char>::_2long(v.c_str());
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
    s << _value;
    return string(s.str());
}


/* -------------------------------------------------------------------------
 * Option_String - methods
 * ----------------------------------------------------------------------- */
Option_String::Option_String() 
    : Option() 
{
}


Option_String::Option_String(string value) 
    : Option(true), _value(value) 
{
}


Option_String::~Option_String() 
{
}


Option_String::Option_String(const Option_String &s) 
    : Option(s) 
{
    _value = s._value;
}


Option_String &
Option_String::operator=(const Option_String &s) 
{
    if(this==&s) {
        return *this;
    }
    Option::operator=(s);
    _value = s._value;
    return *this;
}


string 
Option_String::getString() const 
{
    return _value;
}


bool 
Option_String::set(string v, bool isDefault) 
{
    _value = v;
    return markSet(isDefault);
}


string 
Option_String::getValue() const 
{
    return _value;
}



/* -------------------------------------------------------------------------
 * Option_Float - methods
 * ----------------------------------------------------------------------- */
Option_Float::Option_Float() 
    : Option() 
{
}


Option_Float::Option_Float(float value) 
    : Option(true), _value(value) 
{
}


Option_Float::~Option_Float() 
{
}


Option_Float::Option_Float(const Option_Float &s) 
    : Option(s) 
{
    _value = s._value;
}


Option_Float &
Option_Float::operator=(const Option_Float &s) 
{
    if(this==&s) {
        return *this;
    }
    Option::operator=(s);
    _value = s._value;
    return *this;
}


float 
Option_Float::getFloat() const 
{
    return _value;
}


bool 
Option_Float::set(string v, bool isDefault) 
{
    try {
	    _value = TplConvert<char>::_2float(v.c_str());
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
    s << _value;
    return string(s.str());
}



/* -------------------------------------------------------------------------
 * Option_Bool - methods
 * ----------------------------------------------------------------------- */
Option_Bool::Option_Bool() 
    : Option() 
{
}


Option_Bool::Option_Bool(bool value) 
    : Option(true), _value(value) 
{
}


Option_Bool::~Option_Bool() 
{
}


Option_Bool::Option_Bool(const Option_Bool &s) 
    : Option(s) 
{
    _value = s._value;
}


Option_Bool &
Option_Bool::operator=(const Option_Bool &s) 
{
    if(this==&s) {
        return *this;
    }
    Option::operator=(s);
    _value = s._value;
    return *this;
}


bool 
Option_Bool::getBool() const 
{
    return _value;
}


bool 
Option_Bool::set(bool v, bool isDefault) 
{
    _value = v;
    return markSet(isDefault);
}


string 
Option_Bool::getValue() const 
{
    if(_value) {
	    return string("true");
    }
    return string("false");
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
}


Option_FileName::Option_FileName(std::string value)
    : Option_String(value)
{
}


Option_FileName::Option_FileName(const Option_String &s)
    : Option_String(s)
{
}


Option_FileName::~Option_FileName()
{
}


Option_FileName &
Option_FileName::operator=(const Option_FileName &s)
{
    Option_String::operator=(s);
    return (*this);
}


bool Option_FileName::isFileName() const {
    return true;
}


/* -------------------------------------------------------------------------
 * Option_UIntVector - methods
 * ----------------------------------------------------------------------- */
Option_UIntVector::Option_UIntVector()
    : Option()
{
}


Option_UIntVector::Option_UIntVector(const UIntVector &value)
    : Option(true), _value(value)
{
}


Option_UIntVector::Option_UIntVector(const string &value)
    : Option(true), _value()
{
    set(value, true);
}


Option_UIntVector::Option_UIntVector(const Option_UIntVector &s)
    : Option(s), _value(s._value)
{
}


Option_UIntVector::~Option_UIntVector()
{
}


Option_UIntVector &
Option_UIntVector::operator=(const Option_UIntVector &s)
{
    Option::operator=(s);
    _value = s._value;
    return (*this);
}


const UIntVector &
Option_UIntVector::getUIntVector() const
{
    return _value;
}


bool 
Option_UIntVector::set(std::string v, bool isDefault)
{
    _value.clear();
    try {
        StringTokenizer st(v, ';');
        while(st.hasNext()) {
	        _value.push_back(TplConvert<char>::_2int(st.next().c_str()));
        }
        return markSet(isDefault);
    } catch (...) {
	    string s = "'" + v + "' is not a valid long (should be).";
	    throw InvalidArgument(s);
    }
}


std::string 
Option_UIntVector::getValue() const
{
    ostringstream s;
    for(UIntVector::const_iterator i=_value.begin(); i!=_value.end(); i++) {
        if(i!=_value.begin()) {
            s << ' ';
        }
        s << (*i);
    }
    return string(s.str());
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "Option.icc"
//#endif

// Local Variables:
// mode:C++
// End:








