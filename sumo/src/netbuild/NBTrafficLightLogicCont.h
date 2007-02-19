/****************************************************************************/
/// @file    NBTrafficLightLogicCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for traffic light logic vectors
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
#ifndef NBTrafficLightLogicCont_h
#define NBTrafficLightLogicCont_h
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

#include <map>
#include <string>
#include "NBTrafficLightLogicVector.h"
#include "NBTrafficLightDefinition.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class NBEdgeCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBTrafficLightLogicCont
 * This container class holds definitions of traffic light logics during
 * the loading of the network. After all information has been loaded, these
 * definitions are used to build the traffic light logics.
 * The build traffic light logics are saved within this container during their
 * building and written to the network file at the end.
 */
class NBTrafficLightLogicCont
{
public:
    NBTrafficLightLogicCont();
    ~NBTrafficLightLogicCont();

    /// inserts a named logic definition into the container
    bool insert(const std::string &id,
                NBTrafficLightDefinition *logics);

    /// computes the traffic light logics using the definitions and stores the results
    bool computeLogics(NBEdgeCont &ec, OptionsCont &oc);

    /// saves all known logics
    void writeXML(std::ostream &into);

    /// destroys all stored logics
    void clear();

    void remapRemoved(NBEdge *removed,
                      const EdgeVector &incoming, const EdgeVector &outgoing);

    void replaceRemoved(NBEdge *removed, int removedLane,
                        NBEdge *by, int byLane);

    NBTrafficLightDefinition *getDefinition(const std::string &id);

    bool setTLControllingInformation(const NBEdgeCont &ec);


private:
    /// inserts a named logic into the container
    bool insert(const std::string &id,
                NBTrafficLightLogicVector *logics);


private:
    /// Definition of the container type for tl-ids to previously computed logics
    typedef std::map<std::string, NBTrafficLightLogicVector*> ComputedContType;

    /// The container for previously computed tl-logics
    ComputedContType _computed;

    /// Definition of the container type for tl-ids to their definitions
    typedef std::map<std::string, NBTrafficLightDefinition*> DefinitionContType;

    /// The container for tl-ids to their definitions
    DefinitionContType _definitions;

};


#endif

/****************************************************************************/

