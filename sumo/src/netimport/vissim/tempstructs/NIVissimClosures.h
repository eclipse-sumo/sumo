#ifndef NIVissimClosures_h
#define NIVissimClosures_h
//---------------------------------------------------------------------------//
//                        NIVissimClosures.h -  ccc
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
// Revision 1.4  2003/06/05 11:46:56  dkrajzew
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
#include <utils/common/IntVector.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIVissimClosures {
public:
    NIVissimClosures(const std::string &id,
        int from_node, int to_node,
        IntVector &overEdges);
    ~NIVissimClosures();
    static bool dictionary(const std::string &id,
        int from_node, int to_node, IntVector &overEdges);
    static bool dictionary(const std::string &name, NIVissimClosures *o);
    static NIVissimClosures *dictionary(const std::string &name);
    static void clearDict();
private:
    typedef std::map<std::string, NIVissimClosures*> DictType;
    static DictType myDict;
    const std::string myID;
    int myFromNode, myToNode;
    IntVector myOverEdges;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimClosures.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

