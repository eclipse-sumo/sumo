/****************************************************************************/
/// @file    SUMOVehicleParserHelper.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 07.04.2008
/// @version $Id$
///
// Helper methods for parsing vehicle attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMOVehicleParserHelper_h
#define SUMOVehicleParserHelper_h


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
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/SUMOVehicleParameter.h>
#include <utils/common/SUMOVTypeParameter.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOVehicleParserHelper
 * @brief Helper methods for parsing vehicle attributes
 *
 * This class supports helper methods for parsing a vehicle's attributes.
 */
class SUMOVehicleParserHelper {
public:
    /** @brief Parses a flow's attributes
     *
     * Parses all attributes stored in "SUMOVehicleParameter".
     *
     * @see SUMOVehicleParameter
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @return The parsed attribute structure if no error occured, 0 otherwise
     * @exception ProcessError If an attribute's value is invalid
     * @note: the caller is responsible for deleting the returned pointer
     */
    static SUMOVehicleParameter* parseFlowAttributes(const SUMOSAXAttributes& attrs, const SUMOTime beginDefault, const SUMOTime endDefault);


    /** @brief Parses a vehicle's attributes
     *
     * Parses all attributes stored in "SUMOVehicleParameter".
     *
     * @see SUMOVehicleParameter
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[in] optionalID Whether the id shall be skipped
     * @param[in] skipDepart Whether parsing the departure time shall be skipped
     * @return The parsed attribute structure if no error occured, 0 otherwise
     * @exception ProcessError If an attribute's value is invalid
     * @note: the caller is responsible for deleting the returned pointer
     */
    static SUMOVehicleParameter* parseVehicleAttributes(const SUMOSAXAttributes& attrs,
            bool optionalID = false, bool skipDepart = false);


    /** @brief Starts to parse a vehicle type
     *
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[in] file The name of the file being parsed (for resolving paths)
     * @exception ProcessError If an attribute's value is invalid
     * @see SUMOVTypeParameter
     * @note: the caller is responsible for deleting the returned pointer
     */
    static SUMOVTypeParameter* beginVTypeParsing(const SUMOSAXAttributes& attrs, const std::string& file);


    /** @brief Parses an element embedded in vtype definition
     *
     * @param[in, filled] into The structure to fill with parsed values
     * @param[in] element The id of the currently parsed XML-element
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[in] fromVType Whether the attributes are a part of the vtype-definition
     * @exception ProcessError If an attribute's value is invalid
     * @see SUMOVTypeParameter
     */
    static void parseVTypeEmbedded(SUMOVTypeParameter& into,
                                   int element, const SUMOSAXAttributes& attrs,
                                   bool fromVType = false);


    /** @brief Closes parsing of the vehicle type
     * @return The resulting vehicle type parameter
     * @see SUMOVTypeParameter
     */
    static void closeVTypeParsing(SUMOVTypeParameter& vtype) {
        UNUSED_PARAMETER(vtype);
    }


    /** @brief Parses the vehicle class
     *
     * When given, the vehicle class is parsed using getVehicleClassID.
     *  Exceptions occuring within this process are catched and reported.
     *
     * If no vehicle class is available in the attributes, the default class (SVC_UNKNOWN)
     *  is returned.
     *
     * @param[in] attrs The attributes to read the class from
     * @param[in] id The id of the parsed element, for error message generation
     * @return The parsed vehicle class
     * @see SUMOVehicleClass
     * @todo Recheck how errors are handled and what happens if they occure
     */
    static SUMOVehicleClass parseVehicleClass(const SUMOSAXAttributes& attrs, const std::string& id);


    /** @brief Parses the vehicle emission class
     *
     * When given, the vehicle emission class is parsed using getVehicleEmissionTypeID.
     *  Exceptions occuring within this process are catched and reported.
     *
     * If no vehicle class is available in the attributes, the default class (SVE_UNKNOWN)
     *  is returned.
     *
     * @param[in] attrs The attributes to read the class from
     * @param[in] id The id of the parsed element, for error message generation
     * @return The parsed vehicle emission class
     * @see SUMOEmissionClass
     * @todo Recheck how errors are handled and what happens if they occure
     */
    static SUMOEmissionClass parseEmissionClass(const SUMOSAXAttributes& attrs, const std::string& id);


    /** @brief Parses the vehicle class
     *
     * When given, the vehicle class is parsed using getVehicleShapeID.
     *  Exceptions occuring within this process are catched and reported.
     *
     * If no vehicle class is available in the attributes, the default class (SVS_UNKNOWN)
     *  is returned.
     *
     * @param[in] attrs The attributes to read the class from
     * @param[in] id The id of the parsed element, for error message generation
     * @return The parsed vehicle shape
     * @see SUMOVehicleShape
     * @todo Recheck how errors are handled and what happens if they occure
     */
    static SUMOVehicleShape parseGuiShape(const SUMOSAXAttributes& attrs, const std::string& id);

private:
    /** @brief Parses attributes common to vehicles and flows
     *
     * Parses all attributes stored in "SUMOVehicleParameter".
     *
     * @see SUMOVehicleParameter
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[out] ret The parameter to parse into
     * @param[in] element The name of the element (vehicle or flow)
     * @exception ProcessError If an attribute's value is invalid
     */
    static void parseCommonAttributes(const SUMOSAXAttributes& attrs,
                                      SUMOVehicleParameter* ret, std::string element);


    typedef std::map<SumoXMLTag, std::set<SumoXMLAttr> > CFAttrMap;

    // returns allowed attrs for each known CF-model (init on first use)
    static const CFAttrMap& getAllowedCFModelAttrs();

    // brief allowed attrs for each known CF-model
    static CFAttrMap allowedCFModelAttrs;


};


#endif

/****************************************************************************/

