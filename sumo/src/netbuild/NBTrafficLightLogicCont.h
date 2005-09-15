#ifndef NBTrafficLightLogicCont_h
#define NBTrafficLightLogicCont_h
//---------------------------------------------------------------------------//
//                        NBTrafficLightLogicCont.h -
//  A container for traffic light logic vectors
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
// Revision 1.8  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/04/27 11:48:26  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.6  2003/12/04 13:03:58  dkrajzew
// possibility to pass the tl-type from the netgenerator added
//
// Revision 1.5  2003/10/06 07:46:12  dkrajzew
// further work on vissim import (unsignalised vs. signalised streams modality
//  cleared & lane2lane instead of edge2edge-prohibitions implemented
//
// Revision 1.4  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of
//  geometry information
//
// Revision 1.3  2003/06/05 11:43:36  dkrajzew
// class templates applied; documentation added
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <map>
#include <string>
#include "NBTrafficLightLogicVector.h"
#include "NBTrafficLightDefinition.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;
class NBEdgeCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NBTrafficLightLogicCont
 * This container class holds definitions of traffic light logics during
 * the loading of the network. After all information has been loaded, these
 * definitions are used to build the traffic light logics.
 * The build traffic light logics are saved within this container during their
 * building and written to the network file at the end.
 */
class NBTrafficLightLogicCont {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

