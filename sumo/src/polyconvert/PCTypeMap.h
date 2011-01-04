/****************************************************************************/
/// @file    PCTypeMap.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// A storage for type mappings
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef PCTypeMap_h
#define PCTypeMap_h


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


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class PCTypeMap
 * @brief A storage for type mappings
 *
 * This class holds the mappings between names of read polygon/poi types and the
 *  values (color, new type name etc.) that shall be assigned to them.
 */
class PCTypeMap {
public:
    /// @brief Constructor
    PCTypeMap() throw();


    /// @brief Destructor
    ~PCTypeMap() throw();


    /**
     * @struct TypeDef
     * @brief A single definition of values that shall be used for a given type
     */
    struct TypeDef {
        /// @brief The new type id to use
        std::string id;
        /// @brief The color to use
        std::string color;
        /// @brief The prefix to use
        std::string prefix;
        /// @brief The layer to use
        int layer;
        /// @brief Information whether polygons of this type shall be discarded
        bool discard;
        /// @brief Information whether polygons of this type can be filled
        bool allowFill;

    };


    /** @brief Adds a type definition
     *
     * @param[in] id The original id of the type
     * @param[in] newid The new id (name) of the type
     * @param[in] color The color to set for imported objects of this type
     * @param[in] prefix The prefix to prepend to the read names of this type's objects
     * @param[in] layer The layer number to set for this type's objects
     * @param[in] discard Whether objects of this type shall be discarded
     * @param[in] allowFill Whether objects of this type may be filled
     * @return Whether the type could been added (was not known before)
     */
    bool add(const std::string &id, const std::string &newid, const std::string &color,
             const std::string &prefix, int layer, bool discard, bool allowFill) throw();


    /** @brief Returns a type definition
     *
     * This type definition MUST have been added otherwise the further process
     *  is undefined.
     * @param[in] id The id of the type to get the definitions of
     * @return Definition of the named type
     */
    const TypeDef &get(const std::string &id) throw();


    /** @brief Returns the information whether the named type is known
     * @param[in] id The id of the type
     * @return Whether a definition of the named type was added before
     */
    bool has(const std::string &id) throw();


protected:
    /// @brief A map of type names to type definitions
    std::map<std::string, TypeDef> myTypes;

};


#endif

/****************************************************************************/

