#ifndef NIVissimVehTypeClass_h
#define NIVissimVehTypeClass_h
//---------------------------------------------------------------------------//
//                        NIVissimVehTypeClass.h -  ccc
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
// $Log$
// Revision 1.4  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <string>
#include <map>
#include <utils/gfx/RGBColor.h>
#include <utils/common/IntVector.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIVissimVehTypeClass {
public:
    NIVissimVehTypeClass(int id, const std::string &name,
        const RGBColor &color, IntVector &types);
    ~NIVissimVehTypeClass();
    static bool dictionary(int id, const std::string &name,
        const RGBColor &color, IntVector &types);
    static bool dictionary(int id, NIVissimVehTypeClass *o);
    static NIVissimVehTypeClass *dictionary(int name);
    static void clearDict();
private:
    int myID;
    std::string myName;
    RGBColor myColor;
    IntVector myTypes;
private:
    typedef std::map<int, NIVissimVehTypeClass*> DictType;
    static DictType myDict;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimVehTypeClass.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

