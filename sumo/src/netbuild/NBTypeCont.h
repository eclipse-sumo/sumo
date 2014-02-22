/****************************************************************************/
/// @file    NBTypeCont.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A storage for available types of edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBTypeCont
 * @brief A storage for available types of edges
 *
 * NBTypeCont stores properties of edge-types of edges. Additionally, a default
 *  type is stored which is used if no type information is given.
 *
 * This structure also contains a structure for determining node types using edge
 *  speeds.
 */
class NBTypeCont {
public:
    /// @brief Constructor
    NBTypeCont() {}


    /// @brief Destructor
    ~NBTypeCont() {}


    /** @brief Sets the default values
     * @param[in] defaultNoLanes The default number of lanes an edge has
     * @param[in] defaultSpeed The default speed allowed on an edge
     * @param[in] defaultPriority The default priority of an edge
     */
    void setDefaults(int defaultNoLanes,
                     SUMOReal defaultSpeed, int defaultPriority);


    /** @brief Adds a type into the list. This is a simplified convenience form
     * of insert, if only one allowed vehicle class is necessary.
     * @param[in] id The id of the type
     * @param[in] noLanes The number of lanes an edge of this type has
     * @param[in] maxSpeed The speed allowed on an edge of this type
     * @param[in] prio The priority of an edge of this type
     * @param[in] width The width of lanes of edgesof this type
     * @param[in] vClasses The vehicle classes allowed on an edge of this type
     * @param[in] oneWayIsDefault Whether edges of this type are one-way per default
     * @return Whether the type could be added (no type with the same id existed)
     */
    bool insert(const std::string& id, int noLanes, SUMOReal maxSpeed, int prio,
                SUMOReal width, SUMOVehicleClass vClasses = SVC_UNKNOWN, bool oneWayIsDefault = false);

    /** @brief Adds a type into the list
     * @param[in] id The id of the type
     * @param[in] noLanes The number of lanes an edge of this type has
     * @param[in] maxSpeed The speed allowed on an edge of this type
     * @param[in] prio The priority of an edge of this type
     * @param[in] permissions The encoding of vehicle classes allowed on an edge of this type
     * @param[in] width The width of lanes of edgesof this type
     * @param[in] oneWayIsDefault Whether edges of this type are one-way per default
     * @return Whether the type could be added (no type with the same id existed)
     */
    bool insert(const std::string& id, int noLanes,
                SUMOReal maxSpeed, int prio,
                SVCPermissions permissions,
                SUMOReal width, bool oneWayIsDefault);

    /** @brief Returns the number of known types
     * @return The number of known edge types (excluding the default)
     */
    unsigned int size() const {
        return (unsigned int) myTypes.size();
    }


    /** @brief Returns whether the named type is in the container
     * @return Whether the named type is known
     */
    bool knows(const std::string& type) const;


    /** @brief Marks a type as to be discarded
     * @param[in] id The id of the type
     */
    bool markAsToDiscard(const std::string& id);



    /// @name Type-dependant Retrieval methods
    /// @{

    /** @brief Returns the number of lanes for the given type
     *
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the lane number for
     * @return The number of lanes an edge of this type has
     */
    int getNumLanes(const std::string& type) const;


    /** @brief Returns the maximal velocity for the given type [m/s]
     *
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the speed for
     * @return The allowed speed on edges of this type
     */
    SUMOReal getSpeed(const std::string& type) const;


    /** @brief Returns the priority for the given type
     *
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the priority for
     * @return The priority of edges of this type
     */
    int getPriority(const std::string& type) const;


    /** @brief Returns whether edges are one-way per default for the given type
     *
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the one-way information for
     * @return Whether edges of this type are one-way per default
     * @todo There is no default for one-way!?
     */
    bool getIsOneWay(const std::string& type) const;


    /** @brief Returns the information whether edges of this type shall be discarded.
     *
     * Returns false if the type is not known.
     * @return Whether edges of this type shall be discarded.
     */
    bool getShallBeDiscarded(const std::string& type) const;


    /** @brief Returns allowed vehicle classes for the given type
     *
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the list of allowed vehicles classes for
     * @return List of vehicles class which may use edges of the given type
     */
    SVCPermissions getPermissions(const std::string& type) const;


    /** @brief Returns the lane width for the given type [m/s]
     *
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the width for
     * @return The width of lanes of edges of this type
     */
    SUMOReal getWidth(const std::string& type) const;
    /// @}


private:
    struct TypeDefinition {
        /// @brief Constructor
        TypeDefinition() :
            noLanes(1), speed((SUMOReal) 13.9), priority(-1),
            permissions(SVCFreeForAll),
            oneWay(true), discard(false), width(NBEdge::UNSPECIFIED_WIDTH) { }

        /// @brief Constructor
        TypeDefinition(int _noLanes, SUMOReal _speed, int _priority,
                       SUMOReal _width, SVCPermissions _permissions, bool _oneWay) :
            noLanes(_noLanes), speed(_speed), priority(_priority),
            permissions(_permissions),
            oneWay(_oneWay), discard(false), width(_width) { }

        /// @brief The number of lanes of an edge
        int noLanes;
        /// @brief The maximal velocity on an edge in m/s
        SUMOReal speed;
        /// @brief The priority of an edge
        int priority;
        /// @brief List of vehicle types that are allowed on this edge
        SVCPermissions permissions;
        /// @brief Whether one-way traffic is mostly common for this type (mostly unused)
        bool oneWay;
        /// @brief Whether edges of this type shall be discarded
        bool discard;
        /// @brief The width of lanes of edges of this type [m]
        SUMOReal width;

    };


    /** @brief Retrieve the name or the default type
     *
     * If no name is given, the default type is returned
     * @param[in] name The name of the type to retrieve
     * @return The named type
     */
    const TypeDefinition& getType(const std::string& name) const;


private:
    /// @brief The default type
    TypeDefinition myDefaultType;

    /// @brief A container of types, accessed by the string id
    typedef std::map<std::string, TypeDefinition> TypesCont;

    /// @brief The container of types
    TypesCont myTypes;


private:
    /** @brief invalid copy constructor */
    NBTypeCont(const NBTypeCont& s);

    /** @brief invalid assignment operator */
    NBTypeCont& operator=(const NBTypeCont& s);


};


#endif

/****************************************************************************/

