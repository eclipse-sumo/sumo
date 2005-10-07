//---------------------------------------------------------------------------//
//                        NIVissimVehTypeClass.cpp -  ccc
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
// Revision 1.7  2005/10/07 11:40:10  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:02:57  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.4  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
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
#include <utils/gfx/RGBColor.h>
#include <utils/common/IntVector.h>
#include "NIVissimVehTypeClass.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


NIVissimVehTypeClass::DictType NIVissimVehTypeClass::myDict;

NIVissimVehTypeClass::NIVissimVehTypeClass(int id,
                                           const std::string &name,
                                           const RGBColor &color,
                                           IntVector &types)
    : myID(id), myName(name), myColor(color), myTypes(types)
{
}

NIVissimVehTypeClass::~NIVissimVehTypeClass()
{
}


bool
NIVissimVehTypeClass::dictionary(int id, const std::string &name,
                                 const RGBColor &color,
                                 IntVector &types)
{
    NIVissimVehTypeClass *o = new NIVissimVehTypeClass(id, name, color, types);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}




bool
NIVissimVehTypeClass::dictionary(int name, NIVissimVehTypeClass *o)
{
    DictType::iterator i=myDict.find(name);
    if(i==myDict.end()) {
        myDict[name] = o;
        return true;
    }
    return false;
}


NIVissimVehTypeClass *
NIVissimVehTypeClass::dictionary(int name)
{
    DictType::iterator i=myDict.find(name);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimVehTypeClass::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


