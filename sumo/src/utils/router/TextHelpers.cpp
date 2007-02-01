/****************************************************************************/
/// @file    TextHelpers.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
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
#include <sstream>
#include <utils/common/TplConvert.h>
#include "TextHelpers.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
    if (str.at(pos)==')') {
        isVersion = true;
        while (pos>=0&&str.at(--pos)!='(') {
            if (str.at(pos)>'9'||str.at(pos)<'1') {
                isVersion = false;
            }
        }
        if (str.at(pos)!='(') {
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
    strm << str.substr(0, idx1) << val+1 << ")";
    return strm.str();
}



/****************************************************************************/

