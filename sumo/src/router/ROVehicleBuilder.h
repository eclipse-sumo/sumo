/****************************************************************************/
/// @file    ROVehicleBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Router vehicles building factory
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
#ifndef ROVehicleBuilder_h
#define ROVehicleBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/SUMOTime.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOVehicleParameter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class RORouteDef;
class ROVehicle;
class ROVehicleType;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROVehicleBuilder
 * @brief Router vehicles building factory
 *
 * @todo These methods should be rechecked as soon as the extensions by starting/ending parameter are added
 * @extension: starting/ending state definition
 */
class ROVehicleBuilder
{
public:
    /** @brief Constructor
     */
    ROVehicleBuilder();


    /** @brief Destructor
     */
    virtual ~ROVehicleBuilder();


    /** @brief Builds a vehicle
     *
     * @param[in] id The id of the vehicle to build
     * @param[in] route The definition of the route the vehicle to build shall use
     * @param[in] depart The depart time of the vehicle to build
     * @param[in] type The type of the vehicle to build
     * @param[in] color The color of the vehicle to build
     * @param[in] period The repetition period of the vehicle to build
     * @param[in] repNo The repetition number of the vehicle to build
     * @return The built vehicle
     * 
     * @todo Why is the depart time given as an int?
     * @todo Are there tests for loading vehicles with repetition information into routers?
     */
    virtual ROVehicle *buildVehicle(const SUMOVehicleParameter &pars, 
              RORouteDef *route, ROVehicleType *type);


};


#endif

/****************************************************************************/

