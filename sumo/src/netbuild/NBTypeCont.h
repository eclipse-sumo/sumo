/****************************************************************************/
/// @file    NBTypeCont.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A storage for the available types of edges
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
#ifndef NBTypeCont_h
#define NBTypeCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include "NBNode.h"
#include "NBType.h"
#include "NBJunctionTypesMatrix.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBTypeCont
 */
class NBTypeCont
{
public:
    NBTypeCont();

    ~NBTypeCont();

    /** sets the default values */
    void setDefaults(int defaultNoLanes,
                     SUMOReal defaultSpeed, int defaultPriority);

    /** adds a possible type into the list */
    bool insert(NBType *type);

    /** returns the number of lanes
        (the default value if type does not exist) */
    int getNoLanes(const std::string &type);

    /** returns the maximal velocity on a section/edge in m/s
        (the default value if the type does not exist) */
    SUMOReal getSpeed(const std::string &type);

    /// Returns the function streets of the named type have
    NBEdge::EdgeBasicFunction getFunction(const std::string &type);

    /** returns the priority of the section/edge with the given type
        (the default value if the type does not exist) */
    int getPriority(const std::string &type);

    /** returns the default number of lanes */
    int getDefaultNoLanes();

    /** returns the default speed */
    SUMOReal getDefaultSpeed();

    /** returns the default priority */
    int getDefaultPriority();

    /** returns the number of known types */
    size_t size();

    /** returns the type of the junction between two edges of the given types */
    NBNode::BasicNodeType getJunctionType(SUMOReal speed1, SUMOReal speed2) const;

    /** deletes all types */
    void clear();

    bool knows(const std::string &type) const;

private:
    /** a container of types, accessed by the string key */
    typedef std::map<std::string, NBType*> TypesCont;

private:
    /** the default number of lanes of a section/edge */
    int myDefaultNoLanes;

    /** the default maximal velocity on a section/edge in m/s */
    SUMOReal myDefaultSpeed;

    /** the default priority of a section/edge */
    int myDefaultPriority;

    /** the container of types */
    TypesCont myTypes;

    /** the matrix of roads to junction type mappings */
    NBJunctionTypesMatrix myJunctionTypes;


private:
    /** @brief invalid copy constructor */
    NBTypeCont(const NBTypeCont &s);

    /** @brief invalid assignment operator */
    NBTypeCont &operator=(const NBTypeCont &s);


};


#endif

/****************************************************************************/

