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
// Revision 1.2  2003/03/20 16:41:10  dkrajzew
// periodical car emission implemented; windows eol removed
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <utils/common/UtilExceptions.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include "StringUtils.h"


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
    for(int i=0; i<str.length(); i++) {
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



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "StringUtils.icc"
//#endif

// Local Variables:
// mode:C++
// End:

