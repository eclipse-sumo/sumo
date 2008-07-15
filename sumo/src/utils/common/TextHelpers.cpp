/****************************************************************************/
/// @file    TextHelpers.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id:TextHelpers.cpp 4690 2007-11-09 13:19:45Z dkrajzew $
///
// Some text helping functions
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <sstream>
#include <utils/common/TplConvert.h>
#include "TextHelpers.h"

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
std::string
TextHelpers::version(std::string str)
{
    bool isVersion = false;
    // check whether this already is a version
    int pos = (int) str.length()-1;
    if (str[pos]==')') {
        isVersion = true;
        while (pos>=0&&str[--pos]!='(') {
            if (str[pos]>'9'||str[pos]<'1') {
                isVersion = false;
            }
        }
        if (str[pos]!='(') {
            isVersion = false;
        }
    }
    // make a version if it's not yet one
    if (!isVersion)
        return str + " (2)";
    // change the version
    size_t idx1 = str.rfind('(');
    size_t idx2 = str.rfind(')');
    long val = TplConvert<char>::_2long(str.substr(idx1+1, idx2-idx1-1).c_str());
    ostringstream strm;
    strm << str.substr(0, idx1+1) << val+1 << ")";
    return strm.str();
}



/****************************************************************************/

