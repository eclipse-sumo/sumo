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
// Revision 1.8  2005/04/28 09:02:49  dkrajzew
// level3 warnings removed
//
// Revision 1.7  2004/11/23 10:35:28  dkrajzew
// debugging
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
#include <map>
#include <string>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include "NamedColumnsParser.h"


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
                                       bool prune)
{
    reinitMap(def, defDelim);
    _lineDelim = lineDelim;
}


NamedColumnsParser::~NamedColumnsParser()
{
}


void
NamedColumnsParser::reinit(const std::string &def,
                           const std::string &defDelim,
                           const std::string &lineDelim,
                           bool prune)
{
    reinitMap(def, defDelim, prune);
    _lineDelim = lineDelim;
}


void
NamedColumnsParser::parseLine(const std::string &line)
{
    _line = StringTokenizer(line, _lineDelim);
}


std::string
NamedColumnsParser::get(const std::string &name, bool prune) const
{
    PosMap::const_iterator i=_defMap.find(name);
    if(i==_defMap.end()) {
        throw UnknownElement();
    }
    size_t pos = (*i).second;
    if(_line.size()<=pos) {
        throw OutOfBoundsException();
    }
    std::string ret = _line.get(pos);
    checkPrune(ret, prune);
    return ret;
}


void
NamedColumnsParser::reinitMap(const std::string &s,
                              const std::string &delim, bool prune)
{
    _defMap.clear();
    int pos = 0;
    StringTokenizer st(s, delim);
    while(st.hasNext()) {
        std::string next = st.next();
        checkPrune(next, prune);
        _defMap.insert(map<string, int>::value_type(next, pos++));
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

