/***************************************************************************
                          OptionsSaveStorage.cpp
              Stores options and allows to save them into a xml-file
                             -------------------
    project              : SUMO
    begin                : 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.4  2005/10/07 11:46:56  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/15 12:21:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2004/11/23 10:36:02  dkrajzew
// debugging
//
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
#include "OptionsSaveStorage.h"
#include <utils/common/StringTokenizer.h>
#include <fstream>

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
OptionsSaveStorage::OptionsSaveStorage(const std::string &structure)
    : myStructure(structure)
{
}


OptionsSaveStorage::~OptionsSaveStorage()
{
}


void
OptionsSaveStorage::insert(const std::string &name, const std::string &value)
{
    myMap[name] = value;
}


bool
OptionsSaveStorage::save(const std::string &file) const
{
    StringTokenizer st(myStructure, ";");
    ofstream os(file.c_str());
    if(!os.good()) {
        return false;
    }
    os << "<configuration>" << endl;
    while(st.hasNext()) {
        string pre = st.next();
        os << "   <" << pre << ">" << endl;
        for(StringMap::const_iterator i=myMap.begin(); i!=myMap.end(); i++) {
            string name = (*i).first;
            if(name.substr(0, pre.length())!=pre) {
                continue;
            }
            string value = (*i).second;
            os << "      <" << name.substr(pre.length()+1) << ">" << value
                << "</" << name.substr(pre.length()+1) << ">" << endl;
        }
        os << "   </" << pre << ">" << endl;
    }
    os << "</configuration>" << endl;
    bool ret = os.good();
    os.close();
    return ret;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
