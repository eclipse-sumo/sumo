/****************************************************************************/
/// @file    GUIJunctionControlBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2003
/// @version $Id$
///
// Builder of guisim-junctions and tls
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
#ifndef GUIJunctionControlBuilder_h
#define GUIJunctionControlBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <netload/NLJunctionControlBuilder.h>
#include <utils/geom/Position2DVector.h>


// ===========================================================================
// class declarations
// ===========================================================================
class Position2DVector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIJunctionControlBuilder
 * @brief Builder of guisim-junctions and tls
 *
 * @see NLJunctionControlBuilder
 */
class GUIJunctionControlBuilder
            : public NLJunctionControlBuilder {
public:
    /** @brief Constructor
     * @param[in] net The network to fill
     * @param[in] oc The options to use
     * @todo Why are options not const?
     */
    GUIJunctionControlBuilder(MSNet &net, OptionsCont &oc) throw();


    /** @brief Destructor
     */
    ~GUIJunctionControlBuilder() throw();


protected:
    /// @name Factory methods, overwrite NLJunctionControlBuilder methods
    /// @{

    /** @brief Builds a junction that does not use a logic
     *
     * Builds a GUINoLogicJunction
     *
     * @return The built junction
     */
    virtual MSJunction *buildNoLogicJunction() throw();


    /** @brief Builds a junction with a logic
     *
     * Builds a GUIRightOfWayJunction. Throws an exception if the logic was not built
     *  (see getJunctionLogicSecure).
     *
     * @return The built junction
     * @exception InvalidArgument If the logic of the junction was not built before
     */
    virtual MSJunction *buildLogicJunction() throw(InvalidArgument);
    /// @}


private:
    /** @brief invalid copy operator */
    GUIJunctionControlBuilder(const GUIJunctionControlBuilder &s);

    /** @brief invalid assignment operator */
    GUIJunctionControlBuilder &operator=(const GUIJunctionControlBuilder &s);


};


#endif

/****************************************************************************/

