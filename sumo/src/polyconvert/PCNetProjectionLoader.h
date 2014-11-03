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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
     * @param[in] file The network file from which to parse the location element
     * @param[in] shift The shift of the decimal point when interpreting loaded coordinates
     */
    static void load(const std::string& file, int shift);


protected:
    /** @brief Constructor
     */
    PCNetProjectionLoader(int shift);


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
    /// @brief Information whether the parameter was read
    bool myFoundLocation;

    /// @brief shift of input coordinates (not given in the location element)
    int myShift;


};


#endif

/****************************************************************************/

