/****************************************************************************/
/// @file    PCLoaderXML.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 02.11.2006
/// @version $Id$
///
// A reader for polygons and pois stored in XML-format
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
#ifndef PCLoaderXML_h
#define PCLoaderXML_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "PCPolyContainer.h"
#include "PCTypeMap.h"
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class PCLoaderXML
 * @brief A reader for polygons and pois stored in XML-format
 *
 * Reads pois stored as XML definition. The definitions must match
 *  the format POLYCONVERT generates.
 */
class PCLoaderXML : public SUMOSAXHandler {
public:
    /** @brief Loads pois/polygons assumed to be stored as XML
     *
     * If the option "xml" is set within the given options container,
     *  an instance of PCLoaderXML is built and used as a handler for the
     *  files given in this option.
     *
     * @param[in] oc The options container to get further options from
     * @param[in] toFill The poly/pois container to add loaded polys/pois to
     * @param[in] tm The type map to use for setting values of loaded polys/pois
     * @exception ProcessError if something fails
     */
    static void loadIfSet(OptionsCont& oc, PCPolyContainer& toFill,
                          PCTypeMap& tm);


protected:
    /** @brief Constructor
     * @param[in] toFill The poly/pois container to add loaded polys/pois to
     * @param[in] tm The type map to use for setting values of loaded polys/pois
     * @param[in] oc The options container to get further options from
     */
    PCLoaderXML(PCPolyContainer& toFill,
                PCTypeMap& tm, OptionsCont& oc);


    /// @brief Destructor
    ~PCLoaderXML();


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
    virtual void myStartElement(int element, const SUMOSAXAttributes& attrs);
    //@}


private:
    /// @brief The container to store the converted polygons/pois into
    PCPolyContainer& myCont;

    /// @brief The type map to use
    PCTypeMap& myTypeMap;

    /// @brief Settings to use
    OptionsCont& myOptions;


    /// @name Temporary storages used when parsing polygons
    /// @{

    /// @brief The id of the currently parsed polygon
    std::string myCurrentID;

    /// @brief The type of the currently parsed polygon
    std::string myCurrentType;

    /// @brief The color of the currently parsed polygon
    RGBColor myCurrentColor;

    /// @brief Whether the current polygon must not be prunned
    bool myCurrentIgnorePrunning;

    /// @brief The layer of the currently parsed polygon
    SUMOReal myCurrentLayer;
    /// @}


};


#endif

/****************************************************************************/

