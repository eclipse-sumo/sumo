/****************************************************************************/
/// @file    RODFNetHandler.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 27.03.2006
/// @version $Id$
///
// The handler for SUMO-Networks
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RODFNetHandler_h
#define RODFNetHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <router/RONetHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class OptionsCont;
class ROEdge;
class ROAbstractEdgeBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RODFNetHandler
 */
class RODFNetHandler : public RONetHandler
{
public:
    /// Constructor
    RODFNetHandler(OptionsCont &oc, RONet &net, ROAbstractEdgeBuilder &eb);

    /// Destrcutor
    virtual ~RODFNetHandler() throw();

protected:
    /** the user-implemented handler method for characters */
    void myCharacters(SumoXMLTag element, const std::string &name,
                      const std::string &chars) throw();

};


#endif

/****************************************************************************/

