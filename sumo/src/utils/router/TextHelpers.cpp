//---------------------------------------------------------------------------//
//                        TextHelpers.cpp -
//  Some text helping functions
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2003/07/18 12:35:06  dkrajzew
// removed some warnings
//
// Revision 1.2  2003/02/07 10:52:57  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <string>
#include <sstream>
#include <utils/convert/TplConvert.h>
#include "TextHelpers.h"

using namespace std;


std::string
TextHelpers::version(std::string str)
{
    bool isVersion = false;
    // check whether this already is a version
    int pos = (int) str.length()-1;
    if(str.at(pos)==')') {
        isVersion = true;
        while(pos>=0&&str.at(--pos)!='(') {
            if(str.at(pos)>'9'||str.at(pos)<'1') {
                isVersion = false;
            }
        }
        if(str.at(pos)!='(') {
            isVersion = false;
        }
    }
    // make a version if it's not yet one
    if(!isVersion)
        return str + " (2)";
    // change the version
    size_t idx1 = str.rfind('(');
    size_t idx2 = str.rfind(')');
    long val = TplConvert<char>::_2long(str.substr(idx1, idx2-idx1).c_str());
    ostringstream strm;
    strm << str.substr(0, idx1) << val+1 << ")";
    return strm.str();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "TextHelpers.icc"
//#endif

// Local Variables:
// mode:C++
// End:


