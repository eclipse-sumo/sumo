/***************************************************************************
                          StringUtils.h  -
                          Some helping function for string processing
                             -------------------
    begin                : unknown
    copyright            : (C) 2003 by Daniel Krajzewicz
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

// $Log$
// Revision 1.11  2005/10/07 11:43:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/23 06:05:45  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/04/28 09:02:46  dkrajzew
// level3 warnings removed
//
// Revision 1.7  2004/11/23 10:27:45  dkrajzew
// debugging
//
// Revision 1.6  2004/01/12 14:38:19  dkrajzew
// to-upper method added
//
// Revision 1.5  2003/09/05 15:26:39  dkrajzew
// umlaute conversion added
//
// Revision 1.4  2003/07/30 09:27:40  dkrajzew
// the SUMOReal-trimming function debugged
//
// Revision 1.3  2003/05/20 09:49:43  dkrajzew
// further work and debugging
//
// Revision 1.2  2003/04/01 15:28:12  dkrajzew
// some further functions added
//
// Revision 1.1  2003/03/20 17:31:45  dkrajzew
// StringUtils moved from utils/importio to utils/common
//
// Revision 1.2  2003/03/20 16:41:10  dkrajzew
// periodical car emission implemented; windows eol removed
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
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include "StringUtils.h"

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
std::string
StringUtils::prune(std::string str) {
    size_t idx = str.find_first_not_of(" ");
    if(idx!=string::npos) {
        str = str.substr(idx);
    }
    idx = str.find_last_not_of(" ");
    if(idx!=string::npos&&idx!=str.length()-1) {
        str = str.substr(0, idx+1);
    }
    return str;
}


std::string
StringUtils::to_lower_case(std::string str)
{
    for(size_t i=0; i<str.length(); i++) {
        if(str[i]>='A'&&str[i]<='Z') {
            str[i] = str[i] + 'a' - 'A';
        }
    }
    return str;
}


std::string
StringUtils::version1(std::string str)
{
    size_t idx = str.rfind('_');
    if(idx!=string::npos) {
        try {
            int no = TplConvert<char>::_2int(str.substr(idx+1).c_str());
            str = str.substr(0, idx+1) + toString<int>(no+1);
        } catch (NumberFormatException) {
            str = str + "_0";
        }
    } else {
        str = str + "_0";
    }
    return str;
}


std::string
StringUtils::convertUmlaute(std::string str)
{
    str = replace(str, "ä", "ae");
    str = replace(str, "Ä", "Ae");
    str = replace(str, "ö", "oe");
    str = replace(str, "Ö", "Oe");
    str = replace(str, "ü", "ue");
    str = replace(str, "Ü", "Ue");
    str = replace(str, "ß", "ss");
    str = replace(str, "É", "E");
    str = replace(str, "é", "e");
    str = replace(str, "È", "E");
    str = replace(str, "è", "e");
    return str;
}



std::string
StringUtils::replace(std::string str, const char *what,
                     const char *by)
{
    string what_tmp(what);
    string by_tmp(by);
    size_t idx = str.find(what);
    size_t what_len = what_tmp.length();
    size_t by_len = by_tmp.length();
    while(idx!=string::npos) {
        str = str.replace(idx, what_len, by);
        idx = str.find(what, idx+by_len-what_len);
    }
    return str;
}


std::string
StringUtils::trim(SUMOReal val, size_t to)
{
    string ret = toString<SUMOReal>(val);
    size_t idx = ret.rfind('.');
    if(idx!=string::npos) {
        return ret.substr(0, ret.find('.') + to);
    } else {
        return ret;
    }
}


void
StringUtils::upper(std::string &str)
{
    for(size_t i=0; i<str.length(); i++) {
        if(str[i]>='a'&&str[i]<='z') {
            str[i] = str[i] - 'a' + 'A';
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

