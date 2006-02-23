/***************************************************************************
                          NamedColumnsParser.cpp
             A parser to retrieve information from a table with known column
             names where the comlumn order may change
                             -------------------
    project              : SUMO
    begin                : Fri, 19 Jul 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.11  2006/02/23 11:36:23  dkrajzew
// VISION import added
//
// Revision 1.4  2005/10/06 13:39:51  dksumo
// using of a configuration file rechecked
//
// Revision 1.3  2005/09/09 12:56:15  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.2  2005/04/26 08:11:58  dksumo
// level3 warnings patched; debugging
//
// Revision 1.1.2.1  2005/04/15 10:13:36  dksumo
// level3 warnings removed
//
// Revision 1.1  2004/10/22 12:50:57  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.6  2004/01/26 07:18:25  dkrajzew
// some code style work
//
// Revision 1.5  2003/06/05 14:29:12  dkrajzew
// building problems under Linux patched
//
// Revision 1.4  2003/05/20 09:51:13  dkrajzew
// further work and debugging
//
// Revision 1.3  2003/03/20 17:31:49  dkrajzew
// StringUtils moved from utils/importio to utils/common
//
// Revision 1.2  2003/02/07 10:51:26  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:59:13  dkrajzew
// initial commit for classes that handle import functions
//
// Revision 1.1  2002/07/25 08:55:42  dkrajzew
// support for Visum7.5 & Cell import added
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

#include <map>
#include <string>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include "NamedColumnsParser.h"

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
NamedColumnsParser::NamedColumnsParser()
{
}


NamedColumnsParser::NamedColumnsParser(const std::string &def,
                                       const std::string &defDelim,
                                       const std::string &lineDelim,
                                       bool prune, bool ignoreCase)
{
    reinitMap(def, defDelim);
    myLineDelimiter = lineDelim;
    myAmCaseInsensitive = ignoreCase;
}


NamedColumnsParser::~NamedColumnsParser()
{
}


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
    if(i==myDefinitionsMap.end()) {
        if(myAmCaseInsensitive) {
            i = myDefinitionsMap.find(StringUtils::to_lower_case(name));
        }
    }
    if(i==myDefinitionsMap.end()) {
        throw UnknownElement();
    }
    size_t pos = (*i).second;
    if(myLineParser.size()<=pos) {
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
    if(i==myDefinitionsMap.end()) {
        if(myAmCaseInsensitive) {
            i = myDefinitionsMap.find(StringUtils::to_lower_case(name));
        }
    }
    if(i==myDefinitionsMap.end()) {
        return false;
    }
    size_t pos = (*i).second;
    return myLineParser.size()>pos;
}


void
NamedColumnsParser::reinitMap(std::string s,
                              const std::string &delim, bool prune)
{
    if(myAmCaseInsensitive) {
        s = StringUtils::to_lower_case(s);
    }
    myDefinitionsMap.clear();
    int pos = 0;
    StringTokenizer st(s, delim);
    while(st.hasNext()) {
	    std::string next = st.next();
	    checkPrune(next, prune);
        myDefinitionsMap.insert(map<string, int>::value_type(next, pos++));
    }
}


void
NamedColumnsParser::checkPrune(std::string &str, bool prune) const
{
    if(!prune) {
        return;
    }
    size_t idx = str.find_first_not_of(" ");
    if(idx!=string::npos) {
        str = str.substr(idx);
    }
    idx = str.find_last_not_of(" ");
    if(idx!=string::npos&&idx!=str.length()-1) {
        str = str.substr(0, idx+1);
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

