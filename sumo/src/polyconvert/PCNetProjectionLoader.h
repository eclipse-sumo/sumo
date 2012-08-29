/****************************************************************************/
/// @file    PCNetProjectionLoader.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 02.11.2006
/// @version $Id$
///
// A reader for a SUMO network's projection description
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef PCNetProjectionLoader_h
#define PCNetProjectionLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/Position.h>


// ===========================================================================
// class definitions
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class PCNetProjectionLoader
 * @brief A reader for a SUMO network's projection description
 */
class PCNetProjectionLoader : public SUMOSAXHandler {
public:
    /** @brief Loads network projection if wished
     *
     * If the option "net" is set within the given options container,
     *  an instance of PCNetProjectionLoader is built and used as a handler for the
     *  file given in this option.
     *
     * @param[in] oc The options container to get further options from
     * @param[out] netOffset The network offset to fill from values read from the network
     * @param[out] origNetBoundary The original network boundary to fill from values read from the network
     * @param[out] convNetBoundary The converted network boundary to fill from values read from the network
     * @param[out] projParameter The projection parameter to fill from values read from the network
     * @exception ProcessError if something fails
     */
    static void loadIfSet(OptionsCont& oc, Position& netOffset, Boundary& origNetBoundary,
                          Boundary& convNetBoundary, std::string& projParameter);


protected:
    /** @brief Constructor
     * @param[out] netOffset The network offset to fill from values read from the network
     * @param[out] origNetBoundary The original network boundary to fill from values read from the network
     * @param[out] convNetBoundary The converted network boundary to fill from values read from the network
     * @param[out] projParameter The projection parameter to fill from values read from the network
     */
    PCNetProjectionLoader(Position& netOffset,
                          Boundary& origNetBoundary, Boundary& convNetBoundary, std::string& projParameter);


    /// @brief Destructor
    ~PCNetProjectionLoader();


    /** @brief Returns whether all needed values were read
     * @return Whether all needed values were read
     */
    bool hasReadAll() const;


protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    virtual void myStartElement(int element,
                                const SUMOSAXAttributes& attrs);
    //@}


private:
    /// @brief The network offset to fill from values read from the network
    Position& myNetOffset;

    /// @brief The original network boundary to fill from values read from the network
    Boundary& myOrigNetBoundary;

    /// @brief The converted network boundary to fill from values read from the network
    Boundary& myConvNetBoundary;

    /// @brief The projection parameter to fill from values read from the network
    std::string& myProjParameter;

    /// @brief Information whether the parameter was read
    bool myFoundOffset, myFoundOrigNetBoundary, myFoundConvNetBoundary, myFoundProj;


};


#endif

/****************************************************************************/

