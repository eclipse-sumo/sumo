#ifndef NISUMOHandlerNodes_h
#define NISUMOHandlerNodes_h
//---------------------------------------------------------------------------//
//                        NISUMOHandlerNodes.h -
//  A handler for SUMO nodes
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
// Revision 1.2  2003/06/18 11:15:07  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
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
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <netbuild/NLLoadFilter.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NISUMOHandlerNodes : public SUMOSAXHandler {
private:
    LoadFilter _loading;
public:
    NISUMOHandlerNodes(LoadFilter what);
    ~NISUMOHandlerNodes();
protected:
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);
    void myCharacters(int element, const std::string &name,
        const std::string &chars);
    void myEndElement(int element, const std::string &name);
private:
    void addNode(const Attributes &attrs);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NISUMOHandlerNodes.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

