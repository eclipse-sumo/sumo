/****************************************************************************/
/// @file    NamedColumnsParser.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// A parser to retrieve information from a table with known column
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

#include <map>
#include <string>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include "NamedColumnsParser.h"

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
NamedColumnsParser::NamedColumnsParser()
{}


NamedColumnsParser::NamedColumnsParser(const std::string &def,
                                       const std::string &defDelim,
                                       const std::string &lineDelim,
                                       bool /*prune !!!*/, bool ignoreCase)
{
    reinitMap(def, defDelim);
    myLineDelimiter = lineDelim;
    myAmCaseInsensitive = ignoreCase;
}


NamedColumnsParser::~NamedColumnsParser()
{}


void
NamedColumnsParser::reinit(const std::string &def,
                           const std::string &defDelim,
                           const std::string &lineDelim,
                           bool prune, bool ignoreCase)
{
    myAmCaseInsensitive = ignoreCase;
    reinitMap(def, defDelim, prune);
    myLineDelimiter = lineDelim;
}


void
NamedColumnsParser::parseLine(const std::string &line)
{
    myLineParser = StringTokenizer(line, myLineDelimiter);
}


std::string
NamedColumnsParser::get(const std::string &name, bool prune) const
    {
        PosMap::const_iterator i = myDefinitionsMap.find(name);
        if (i==myDefinitionsMap.end()) {
            if (myAmCaseInsensitive) {
                i = myDefinitionsMap.find(StringUtils::to_lower_case(name));
            }
            if (i==myDefinitionsMap.end()) {
                throw UnknownElement();
            }
        }
        size_t pos = (*i).second;
        if (myLineParser.size()<=pos) {
            throw OutOfBoundsException();
        }
        std::string ret = myLineParser.get(pos);
        checkPrune(ret, prune);
        return ret;
    }


bool
NamedColumnsParser::know(const std::string &name) const
{
    PosMap::const_iterator i = myDefinitionsMap.find(name);
    if (i==myDefinitionsMap.end()) {
        if (myAmCaseInsensitive) {
            i = myDefinitionsMap.find(StringUtils::to_lower_case(name));
        }
    }
    if (i==myDefinitionsMap.end()) {
        return false;
    }
    size_t pos = (*i).second;
    return myLineParser.size()>pos;
}


void
NamedColumnsParser::reinitMap(std::string s,
                              const std::string &delim, bool prune)
{
    if (myAmCaseInsensitive) {
        s = StringUtils::to_lower_case(s);
    }
    myDefinitionsMap.clear();
    int pos = 0;
    StringTokenizer st(s, delim);
    while (st.hasNext()) {
        std::string next = st.next();
        checkPrune(next, prune);
        myDefinitionsMap.insert(map<string, int>::value_type(next, pos++));
    }
}


void
NamedColumnsParser::checkPrune(std::string &str, bool prune) const
{
    if (!prune) {
        return;
    }
    size_t idx = str.find_first_not_of(" ");
    if (idx!=string::npos) {
        str = str.substr(idx);
    }
    idx = str.find_last_not_of(" ");
    if (idx!=string::npos&&idx!=str.length()-1) {
        str = str.substr(0, idx+1);
    }
}



/****************************************************************************/

