/****************************************************************************/
/// @file    NLDiscreteEventBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Sep, 2003
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/sax2/Attributes.hpp>
#include <string>
#include <map>
#include <utils/xml/GenericSAXHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLDiscreteEventBuilder
 * This class is responsible for building event-handling actions which
 *  the simulation shall execute.
 */
class NLDiscreteEventBuilder {
public:
    /// Known action types
    enum ActionType {
        /// "SaveTLSStates"
        EV_SAVETLSTATE,
        /// "SaveTLSSwitchTimes"
        EV_SAVETLSWITCHES,
        /// "SaveTLSSwitchStates"
        EV_SAVETLSWITCHSTATES
    };

    /// Constructor
    NLDiscreteEventBuilder(MSNet &net);

    /// Destructor
    ~NLDiscreteEventBuilder();

    /// Builds an action and saves it for further use
    void addAction(const SUMOSAXAttributes &attrs, const std::string &basePath);

private:
    /// Builds an action which saves the state of a certain tls into a file
    void buildSaveTLStateCommand(const SUMOSAXAttributes &attrs, const std::string &basePath);

    /// Builds an action which saves the switch times of links into a file
    void buildSaveTLSwitchesCommand(const SUMOSAXAttributes &attrs, const std::string &basePath);

    /// Builds an action which saves the switch times and states of tls into a file
    void buildSaveTLSwitchStatesCommand(const SUMOSAXAttributes &attrs, const std::string &basePath);

private:
    NLDiscreteEventBuilder& operator=(const NLDiscreteEventBuilder&); // just to avoid a compiler warning

protected:
    /// Definitions of a storage for build actions
    typedef std::map<std::string, ActionType> KnownActions;

    /// Build actions that shall be executed during the simulation
    KnownActions myActions;

    MSNet &myNet;

};


#endif

/****************************************************************************/

