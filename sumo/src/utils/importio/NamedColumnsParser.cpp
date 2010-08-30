/****************************************************************************/
/// @file    NamedColumnsParser.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// A parser to retrieve information from a table with known column
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <map>
#include <string>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include "NamedColumnsParser.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NamedColumnsParser::NamedColumnsParser() throw() {}


NamedColumnsParser::NamedColumnsParser(const std::string &def,
                                       const std::string &defDelim,
                                       const std::string &lineDelim,
                                       bool prune, bool ignoreCase) throw()
        : myLineDelimiter(lineDelim), myAmCaseInsensitive(ignoreCase) {
    reinitMap(def, defDelim, prune);
}


NamedColumnsParser::~NamedColumnsParser() throw() {}


void
NamedColumnsParser::reinit(const std::string &def,
                           const std::string &defDelim,
                           const std::string &lineDelim,
                           bool prune, bool ignoreCase) throw() {
    myAmCaseInsensitive = ignoreCase;
    reinitMap(def, defDelim, prune);
    myLineDelimiter = lineDelim;
}


void
NamedColumnsParser::parseLine(const std::string &line) throw() {
    myLineParser = StringTokenizer(line, myLineDelimiter);
}


std::string
NamedColumnsParser::get(const std::string &name, bool prune) const throw(UnknownElement, OutOfBoundsException) {
    PosMap::const_iterator i = myDefinitionsMap.find(name);
    if (i==myDefinitionsMap.end()) {
        if (myAmCaseInsensitive) {
            i = myDefinitionsMap.find(StringUtils::to_lower_case(name));
        }
        if (i==myDefinitionsMap.end()) {
            throw UnknownElement(name);
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
NamedColumnsParser::know(const std::string &name) const throw() {
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


bool
NamedColumnsParser::hasFullDefinition() const throw() {
    return myDefinitionsMap.size() == myLineParser.size();
}


void
NamedColumnsParser::reinitMap(std::string s,
                              const std::string &delim,
                              bool prune) throw() {
    if (myAmCaseInsensitive) {
        s = StringUtils::to_lower_case(s);
    }
    myDefinitionsMap.clear();
    int pos = 0;
    StringTokenizer st(s, delim);
    while (st.hasNext()) {
        std::string next = st.next();
        checkPrune(next, prune);
        myDefinitionsMap.insert(std::map<std::string, int>::value_type(next, pos++));
    }
}


void
NamedColumnsParser::checkPrune(std::string &str, bool prune) const throw() {
    if (!prune) {
        return;
    }
    size_t idx = str.find_first_not_of(" ");
    if (idx!=std::string::npos) {
        str = str.substr(idx);
    }
    idx = str.find_last_not_of(" ");
    if (idx!=std::string::npos&&idx!=str.length()-1) {
        str = str.substr(0, idx+1);
    }
}



/****************************************************************************/

