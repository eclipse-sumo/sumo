/****************************************************************************/
/// @file    NLDiscreteEventBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Sep, 2003
/// @version $Id$
///
// missing_desc
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
#ifndef NLDiscreteEventBuilder_h
#define NLDiscreteEventBuilder_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/sax2/Attributes.hpp>
#include <string>
#include <map>
#include <utils/xml/AttributesHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class Command;
class MSNet;


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLDiscreteEventBuilder
 * This class is responsible for building event-handling actions which
 *  the simulation shall execute.
 */
class NLDiscreteEventBuilder : public AttributesHandler
{
public:
    /// Known action types
    enum ActionType {
        EV_SAVETLSTATE,
        EV_SAVETLSWITCHES
    };

    /// Constructor
    NLDiscreteEventBuilder(MSNet &net);

    /// Destructor
    ~NLDiscreteEventBuilder();

    /// Builds an action and saves it for further use
    void addAction(const Attributes &attrs,
                   const std::string &basePath);

private:
    /// Builds an action which save the state of a certain tls into a file
    Command *buildSaveTLStateCommand(const Attributes &attrs,
                                     const std::string &basePath);

    /// Builds an action which save the state of a certain tls into a file
    Command *buildSaveTLSwitchesCommand(const Attributes &attrs,
                                        const std::string &basePath);

protected:
    /// Definitions of a storage for build actions
    typedef std::map<std::string, ActionType> KnownActions;

    /// Build actions that shall be executed during the simulation
    KnownActions myActions;

    MSNet &myNet;

};


#endif

/****************************************************************************/

