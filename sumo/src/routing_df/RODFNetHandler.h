#ifndef RODFNetHandler_h
#define RODFNetHandler_h
//---------------------------------------------------------------------------//
//                        RODFNetHandler.h -
//  The handler for SUMO-Networks
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 27.03.2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
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
// Revision 1.1  2006/03/28 06:17:18  dkrajzew
// extending the dfrouter by distance/length factors
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

#include <router/RONetHandler.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class OptionsCont;
class ROEdge;
class ROAbstractEdgeBuilder;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RODFNetHandler
 */
class RODFNetHandler : public RONetHandler {
public:
    /// Constructor
    RODFNetHandler(OptionsCont &oc, RONet &net, ROAbstractEdgeBuilder &eb);

    /// Destrcutor
    virtual ~RODFNetHandler();

protected:
    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

