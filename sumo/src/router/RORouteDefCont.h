#ifndef RORouteDefCont_h
#define RORouteDefCont_h
//---------------------------------------------------------------------------//
//                        RORouteDefCont.h -
//  A container for route definitions
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
// Revision 1.5  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.4  2003/03/03 15:22:35  dkrajzew
// debugging
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <utils/router/IDSupplier.h>
#include "RORouteDef.h"
#include <utils/common/NamedObjectCont.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORouteDefCont
 * A container for route definitions
 */
class RORouteDefCont : public NamedObjectCont<RORouteDef*> {
private:
    typedef std::map<std::pair<ROEdge*, ROEdge*>, RORouteDef*> idMap;
    idMap _known;
    IDSupplier _idSupplier;
public:
    RORouteDefCont();
    ~RORouteDefCont();
    bool add(std::string id, RORouteDef *def);
//    std::string add(ROEdge *from, ROEdge *to);
    static void writeXML(std::ostream &os);
    bool known(const std::string &name) const;
    bool known(ROEdge *from, ROEdge *to) const;
    std::string getID(ROEdge *from, ROEdge *to) const;
    void clear();
private:
    /// we made the copy constructor invalid
    RORouteDefCont(const RORouteDefCont &src);

    /// we made the assignment operator invalid
    RORouteDefCont &operator=(const RORouteDefCont &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "RORouteDefCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

