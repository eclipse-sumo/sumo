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
// $Log: Option.cpp,v $
// Revision 1.12  2006/08/01 07:38:46  dkrajzew
// revalidation of options messaging
//
// Revision 1.11  2006/04/07 10:41:50  dkrajzew
// code beautifying: embedding string in strings removed
//
// Revision 1.10  2005/10/17 09:25:12  dkrajzew
// got rid of the old MSVC memory leak checker
//
// Revision 1.9  2005/10/07 11:46:56  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/23 06:11:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 12:21:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 09:28:00  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.5  2004/11/23 10:36:02  dkrajzew
// debugging
//
// Revision 1.4  2004/07/02 09:41:32  dkrajzew
// debugging the repeated setting of a value
//
// Revision 1.3  2003/08/20 11:49:55  dkrajzew
// allowed the retrival of an uint-vector encoded as string; not the best, but the fastest solution
//
// Revision 1.2  2003/02/07 10:51:59  dkrajzew
// updated
//
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <exception>
#include <sstream>
#include "Option.h"
#include <utils/common/TplConvert.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
    : myAmSet(set), myHaveTheDefaultValue(true), myAmWritable(true),
    myAmMandatory(false)
{
}


Option::Option(const Option &s)
    : myAmSet(s.myAmSet), myHaveTheDefaultValue(s.myHaveTheDefaultValue), 
    myAmWritable(s.myAmWritable),
    myAmMandatory(false)
{
}


Option::~Option()
{
}


Option &
Option::operator=(const Option &s)
{
    if(this==&s) {
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
{
}


Option_Integer::Option_Integer(const Option_Integer &s)
    : Option(s)
{
    myValue = s.myValue;
}


Option_Integer &
Option_Integer::operator=(const Option_Integer &s)
{
    if(this==&s) return *this;
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
{
}


Option_Long::Option_Long(const Option_Long &s)
    : Option(s)
{
    myValue = s.myValue;
}


Option_Long &
Option_Long::operator=(const Option_Long &s)
{
    if(this==&s) {
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
{
}


Option_String::Option_String(const Option_String &s)
    : Option(s)
{
    myValue = s.myValue;
}


Option_String &
Option_String::operator=(const Option_String &s)
{
    if(this==&s) {
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
{
}


Option_Float::Option_Float(const Option_Float &s)
    : Option(s)
{
    myValue = s.myValue;
}


Option_Float &
Option_Float::operator=(const Option_Float &s)
{
    if(this==&s) {
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
{
}


Option_Bool::Option_Bool(const Option_Bool &s)
    : Option(s)
{
    myValue = s.myValue;
}


Option_Bool &
Option_Bool::operator=(const Option_Bool &s)
{
    if(this==&s) {
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
    if(myValue) {
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
{
}


Option_IntVector::~Option_IntVector()
{
}


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
        StringTokenizer st(v, ';');
        while(st.hasNext()) {
            myValue.push_back(TplConvert<char>::_2int(st.next().c_str()));
        }
        return markSet(isDefault);
    } catch (EmptyData &) {
        throw InvalidArgument("Empty element occured in " + v);
    } catch (...) {
        throw InvalidArgument("'" + v + "' is not a valid integer vector (should be).");
    }
    return false;
}


std::string
Option_IntVector::getValue() const
{
    ostringstream s;
    for(IntVector::const_iterator i=myValue.begin(); i!=myValue.end(); i++) {
        if(i!=myValue.begin()) {
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



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:








