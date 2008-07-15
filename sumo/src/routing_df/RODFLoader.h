/****************************************************************************/
/// @file    RODFLoader.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Jun 2004
/// @version $Id$
///
// Loader used for dfrouter-classes
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
#ifndef RODFLoader_h
#define RODFLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <router/ROLoader.h>


// ===========================================================================
// class declarations
// ===========================================================================
class RODFNet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROLoader
 * @brief Loader used for dfrouter-classes
 *
 * @see ROLoader
 * @todo Recheck whether this is really usable; Maybe it is better to have a network
 *  to load assigned to the loader...
 */
class RODFLoader : public ROLoader
{
public:
    /** @brief Constructor
     *
     * @param[in] oc The options to use
     * @param[in] vb The vehicle builder to use
     * @param[in] emptyDestinationsAllowed Whether tripdefs may be given without destinations
     * @todo Recheck usage of emptyDestinationsAllowed
     */
    RODFLoader(OptionsCont &oc, ROVehicleBuilder &vb,
               bool emptyDestinationsAllowed) throw();


    /// @brief Destructor
    ~RODFLoader();


    /// Builds the RODFNet
    RODFNet *loadNet(ROAbstractEdgeBuilder &eb, bool amInHighwayMode);

};


#endif

/****************************************************************************/

