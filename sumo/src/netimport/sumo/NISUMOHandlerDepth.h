#ifndef NISUMOHandlerDepth_h
#define NISUMOHandlerDepth_h
//---------------------------------------------------------------------------//
//                        NISUMOHandlerDepth.h -
//  A handler for deeper SUMO-import (connections and logics)
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
// Revision 1.1  2003/02/07 11:13:27  dkrajzew
// names changed
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <netbuild/NLLoadFilter.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NISUMOHandlerDepth : public SUMOSAXHandler {
private:
    LoadFilter _loading;
public:
    NISUMOHandlerDepth(LoadFilter what, bool warn, bool verbose);
    ~NISUMOHandlerDepth();
protected:
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);
    void myCharacters(int element, const std::string &name,
        const std::string &chars);
    void myEndElement(int element, const std::string &name);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NISUMOHandlerDepth.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

