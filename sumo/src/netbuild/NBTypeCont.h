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
 * NBTypeCont stores properties of edge-types of edges. Additionally, a default 
 *  type is stored which is used if no type information is given.
 *
 * This structure also contains a structure for determining node types using edge
 *  speeds.
 */
class NBTypeCont {
public:
    /// @brief Constructor
    NBTypeCont() throw() {}


    /// @brief Destructor
    ~NBTypeCont() throw() {}


    /** @brief Sets the default values
     * @param[in] defaultNoLanes The default number of lanes an edge has
     * @param[in] defaultSpeed The default speed allowed on an edge
     * @param[in] defaultPriority The default priority of an edge
     */
    void setDefaults(int defaultNoLanes,
                     SUMOReal defaultSpeed, int defaultPriority) throw();


    /** @brief Adds a type into the list 
     * @param[in] id The id of the type
     * @param[in] noLanes The number of lanes an edge of this type has
     * @param[in] maxSpeed The speed allowed on an edge of this type
     * @param[in] prio The priority of an edge of this type
     * @param[in] vClasses The vehicle classes allowed on an edge of this type
     * @param[in] oneWayIsDefault Whether edges of this type are one-way per default
     * @return Whether the type could be added (no type with the same id existed)
     */
    bool insert(const std::string &id, int noLanes, SUMOReal maxSpeed, int prio,
                SUMOVehicleClass vClasses=SVC_UNKNOWN, bool oneWayIsDefault=false) throw();


    /** @brief Returns the number of known types
     * @return The number of known edge types (excluding the default)
     */
    unsigned int size() const throw() {
        return (unsigned int) myTypes.size();
    }


    /** @brief Returns the type of the junction between two edges of the given types
     * @return The node type for two edges havnig the given speeds
     */
    NBNode::BasicNodeType getJunctionType(SUMOReal speed1, SUMOReal speed2) const throw();


    /** @brief Returns whether the named type is in the container
     * @return Whether the named type is known
     */
    bool knows(const std::string &type) const throw();


    /** @brief Marks a type as to be discarded
     * @param[in] id The id of the type
     */
    bool markAsToDiscard(const std::string &id) throw();



    /// @name Type-dependant Retrieval methods
    /// @{

    /** @brief Returns the number of lanes for the given type
     * 
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the lane number for
     * @return The number of lanes an edge of this type has
     */
    int getNoLanes(const std::string &type) const throw();


    /** @brief Returns the maximal velocity for the given type [m/s]
     *
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the speed for
     * @return The allowed speed on edges of this type
     */
    SUMOReal getSpeed(const std::string &type) const throw();


    /** @brief Returns the priority for the given type
     *
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the priority for
     * @return The priority of edges of this type
     */
    int getPriority(const std::string &type) const throw();


    /** @brief Returns whether edges are one-way per default for the given type
     *
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the one-way information for
     * @return Whether edges of this type are one-way per default
     * @todo There is no default for one-way!?
     */
    bool getIsOneWay(const std::string &type) const throw();

    
    /** @brief Returns the information whether edges of this type shall be discarded.
     * 
     * Returns false if the type is not known.
     * @return Whether edges of this type shall be discarded.
     */
    bool getShallBeDiscarded(const std::string &type) const throw();


    /** @brief Returns allowed vehicle classes for the given type
     *
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the list of allowed vehicles classes for
     * @return List of vehicles class which may use edges of the given type
     */
    const std::vector<SUMOVehicleClass> &getAllowedClasses(const std::string &type) const throw();


    /** @brief Returns not allowed vehicle classes for the given type
     *
     * If the named type is not known, the default is returned
     * @param[in] type The name of the type to return the list of not allowed vehicles classes for
     * @return List of vehicles class which may not use edges of the given type
     */
    const std::vector<SUMOVehicleClass> &getDisallowedClasses(const std::string &type) const throw();
    /// @}



    /// @name Type-dependant Retrieval methods
    /// @{

    /** @brief Returns the default number of lanes of an edge
     * @return The number of lanes an edge has per default
     */
    int getDefaultNoLanes() throw() {
        return myDefaultType.noLanes;
    }


    /** @brief Returns the default speed of an edge
     * @return The maximum speed allowed on an edge per default
     */
    SUMOReal getDefaultSpeed() throw() {
        return myDefaultType.speed;
    }


    /** @brief Returns the default priority of an edge
     * @return The default priority of an edge 
     */
    int getDefaultPriority() throw() {
        return myDefaultType.priority;
    }
    /// @}


private:
    struct TypeDefinition {
        /// @brief Constructor
        TypeDefinition()
                : noLanes(1), speed((SUMOReal) 13.9), priority(-1), oneWay(true), discard(false) { }

        /// @brief Destructor
        TypeDefinition(int _noLanes, SUMOReal _speed, int _priority)
                : noLanes(_noLanes), speed(_speed), priority(_priority), oneWay(true), discard(false) { }

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
        /// @brief Whether edges of this type shall be discarded
        bool discard;

    };

    /// @brief The default type
    TypeDefinition myDefaultType;

    /// @brief A container of types, accessed by the string id
    typedef std::map<std::string, TypeDefinition> TypesCont;

    /// @brief The container of types
    TypesCont myTypes;

    /// @brief The matrix of roads to junction type mappings
    NBJunctionTypesMatrix myJunctionTypes;


private:
    /** @brief invalid copy constructor */
    NBTypeCont(const NBTypeCont &s);

    /** @brief invalid assignment operator */
    NBTypeCont &operator=(const NBTypeCont &s);


};


#endif

/****************************************************************************/

