/****************************************************************************/
/// @file    NBTypeCont.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A storage for available types of edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include "NBJunctionTypesMatrix.h"
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBTypeCont
 * @brief A storage for available types of edges
 *
 * NBTypeCont stores properties of types of edges. If one of needed edge
 *  attributes is missing, but a type name is given, this attribute's
 *  value will be filled from what was defined for the type.
 *
 * Additionally, a default type is stored which is used if no type information
 *  is given.
 */
class NBTypeCont {
public:
    /// @brief Constructor
    NBTypeCont() throw() {}


    /// @brief Destructor
    ~NBTypeCont() throw() {}

    /** sets the default values */
    void setDefaults(int defaultNoLanes,
                     SUMOReal defaultSpeed, int defaultPriority);

    /** adds a possible type into the list */
    bool insert(const std::string &id, int noLanes, SUMOReal maxSpeed, int prio,
                SUMOVehicleClass vClasses=SVC_UNKNOWN, bool oneWayIsDefault=false);

    /** returns the number of lanes
        (the default value if type does not exist) */
    int getNoLanes(const std::string &type);

    /** returns the maximal velocity on a section/edge in m/s
        (the default value if the type does not exist) */
    SUMOReal getSpeed(const std::string &type);

    /** returns the priority of the section/edge with the given type
        (the default value if the type does not exist) */
    int getPriority(const std::string &type);

    bool getIsOneWay(const std::string &type);
    const std::vector<SUMOVehicleClass> &getAllowedClasses(const std::string &type);
    const std::vector<SUMOVehicleClass> &getDisallowedClasses(const std::string &type);

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


    bool knows(const std::string &type) const;

private:
    struct TypeDefinition {
        TypeDefinition()
                : noLanes(1), speed((SUMOReal) 13.9), priority(-1), oneWay(true) { }

        TypeDefinition(int _noLanes, SUMOReal _speed, int _priority)
                : noLanes(_noLanes), speed(_speed), priority(_priority), oneWay(true) { }


        /// @brief The number of lanes of an edge
        int noLanes;
        /// @brief The maximal velocity on an edge in m/s
        SUMOReal speed;
        /// @brief The priority of an edge
        int priority;
        /// @brief List of vehicle types that are allowed on this edge
        std::vector<SUMOVehicleClass> allowed;
        /// @brief List of vehicle types that are not allowed on this edge
        std::vector<SUMOVehicleClass> notAllowed;
        /// @brief Whether one-way traffic is mostly common for this type (mostly unused)
        bool oneWay;

    };

    /// @brief The default type
    TypeDefinition myDefaultType;

    /// @brief A container of types, accessed by the string id
    typedef std::map<std::string, TypeDefinition> TypesCont;

    /// @brief The container of types
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

