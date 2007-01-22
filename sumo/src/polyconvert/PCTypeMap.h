#ifndef PCTypeMap_h
#define PCTypeMap_h
/***************************************************************************
                          PCElmar.h
    A storage for type mappings
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Mon, 05 Dec 2005
    copyright            : (C) 2005 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.2  2007/01/08 14:43:58  dkrajzew
// code beautifying; prliminary import for Visum points added
//
// Revision 1.1  2006/08/01 07:52:47  dkrajzew
// polyconvert added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <map>


/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * @class PCTypeMap
 * @brief A storage for type mappings
 *
 * This class holds the mappings between names of read polygon types and the
 *  values (color, new type name etc.) that shall be assigned to them.
 */
class PCTypeMap {
public:
    /// Constructor
    PCTypeMap();

    /// Destructor
    ~PCTypeMap();

    /**
     * @struct TypeDef
     * @brief a single definition of values resulting from a type
     */
    struct TypeDef {
        /// The new type id to use
        std::string id;
        /// The color to use
        std::string color;
        /// The prefix to use
        std::string prefix;
        /// The layer to use
        int layer;
        /// Information whether polygons of this type shall be discarded
        bool discard;
        /// Information whether polygons of this type can be filled
        bool allowFill;

    };

    /// Adds a type definition
    bool add(const std::string &id, const std::string &newid, const std::string &color,
        const std::string &prefix, int layer, bool discard, bool allowFill);

    /** @brief Returns a type definition
     *
     * This type definition MUST have been added otherwise the further process
     *  is undefined.
     */
    const TypeDef &get(const std::string &id);

    /// Returns the information whether the named type was addded
    bool has(const std::string &id);


protected:
    /// A map of type names to type definitions
    std::map<std::string, TypeDef> myTypes;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
