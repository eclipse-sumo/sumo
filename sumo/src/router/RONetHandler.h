#ifndef RONetHandler_h
#define RONetHandler_h
//---------------------------------------------------------------------------//
//                        RONetHandler.h -
//  The handler for SUMO-Networks
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
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
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
#include <utils/sumoxml/SUMOSAXHandler.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class OptionsCont;
class ROEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RONetHandler
 * SAX2-Handler for SUMO-network loading
 */
class RONetHandler : public SUMOSAXHandler {
public:
    /// Constructor
    RONetHandler(OptionsCont &oc, RONet &net);

    /// Destrcutor
    ~RONetHandler();

protected:
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);

private:
    /// Parses an edge
    void parseEdge(const Attributes &attrs);

    /// Parses a lane
    void parseLane(const Attributes &attrs);

    /// Parses a junction
    void parseJunction(const Attributes &attrs);

    /// Parses edge connections
    void parseConnEdge(const Attributes &attrs);

    /// Preallocates edges
    void preallocateEdges(const std::string &chars);

private:
    /// the options (program settings)
    OptionsCont &_options;

    /// the net to store the information into
    RONet &_net;

    /// the name of the edge/node that is currently processed
    std::string _currentName;

    /// the currently build edge
    ROEdge *_currentEdge;

private:
    /// invalidated copy constructor
    RONetHandler(const RONetHandler &src);

    /// invalidated assignment operator
    RONetHandler &operator=(const RONetHandler &src);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "RONetHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

