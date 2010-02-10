/****************************************************************************/
/// @file    SUMOVehicleParserHelper.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 07.04.2008
/// @version $Id$
///
// Helper methods for parsing vehicle attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
     */
    static SUMOVehicleParameter *parseFlowAttributes(const SUMOSAXAttributes &attrs) throw(ProcessError);


    /** @brief Parses a vehicle's attributes
     *
     * Parses all attributes stored in "SUMOVehicleParameter".
     *
     * @see SUMOVehicleParameter
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[in] skipID Whether parsing the id shall be skipped
     * @param[in] skipDepart Whether parsing the departure time shall be skipped
     * @return The parsed attribute structure if no error occured, 0 otherwise
     * @exception ProcessError If an attribute's value is invalid
     */
    static SUMOVehicleParameter *parseVehicleAttributes(const SUMOSAXAttributes &attrs,
            bool skipID=false, bool skipDepart=false) throw(ProcessError);


    /** @brief Starts to parse a vehicle type
     *
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @exception ProcessError If an attribute's value is invalid
     * @see SUMOVTypeParameter
     */
    static SUMOVTypeParameter *beginVTypeParsing(const SUMOSAXAttributes &attrs) throw(ProcessError);


    /** @brief Parses an element embedded in vtype definition
     *
     * @param[in, filled] into The structure to fill with parsed values
     * @param[in] element The id of the currently parsed XML-element
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[in] fromVType Whether the attributes are a part of the vtype-definition
     * @exception ProcessError If an attribute's value is invalid
     * @see SUMOVTypeParameter
     */
    static void parseVTypeEmbedded(SUMOVTypeParameter &into,
                                   int element, const SUMOSAXAttributes &attrs,
                                   bool fromVType=false) throw(ProcessError);


    /** @brief Parses the Krauss definition embedded in vtype definition
     *
     * @param[in, filled] into The structure to fill with parsed values
     * @param[in] attr The SAX-attributes to get model parameter from
     * @param[in] fromVType Whether the attributes are a part of the vtype-definition
     * @exception ProcessError If an attribute's value is invalid
     * @see SUMOVTypeParameter
     */
    static void parseVTypeEmbedded_Krauss(SUMOVTypeParameter &into,
                                          const SUMOSAXAttributes &attrs,
                                          bool fromVType=false) throw(ProcessError);


    /** @brief Parses the Krauss definition embedded in vtype definition
     *
     * @param[in, filled] into The structure to fill with parsed values
     * @param[in] attr The SAX-attributes to get model parameter from
     * @exception ProcessError If an attribute's value is invalid
     * @see SUMOVTypeParameter
     */
    static void parseVTypeEmbedded_IDM(SUMOVTypeParameter &into,
                                       const SUMOSAXAttributes &attrs) throw(ProcessError);


    /** @brief Closes parsing of the vehicle type
     * @return The resulting vehicle type parameter
     * @see SUMOVTypeParameter
     */
    static void closeVTypeParsing(SUMOVTypeParameter &vtype) throw() { }


    /** @brief Parses the vehicle class
     *
     * When given, the vehicle class is parsed using getVehicleClassID.
     *  Exceptions occuring within this process are catched and reported.
     *
     * If no vehicle class is available in the attributes, the default class (SVC_UNKNOWN)
     *  is returned.
     *
     * @param[in] attrs The attributes to read the class from
     * @param[in] type The data type (element) that is processed, for error message generation
     * @param[in] id The id of the parsed element, for error message generation
     * @return The parsed vehicle class
     * @see SUMOVehicleClass
     * @todo Recheck how errors are handled and what happens if they occure
     */
    static SUMOVehicleClass parseVehicleClass(const SUMOSAXAttributes &attrs, const std::string &type,
            const std::string &id) throw();


    /** @brief Parses the vehicle emission class
     *
     * When given, the vehicle emission class is parsed using getVehicleEmissionTypeID.
     *  Exceptions occuring within this process are catched and reported.
     *
     * If no vehicle class is available in the attributes, the default class (SVE_UNKNOWN)
     *  is returned.
     *
     * @param[in] attrs The attributes to read the class from
     * @param[in] type The data type (element) that is processed, for error message generation
     * @param[in] id The id of the parsed element, for error message generation
     * @return The parsed vehicle emission class
     * @see SUMOEmissionClass
     * @todo Recheck how errors are handled and what happens if they occure
     */
    static SUMOEmissionClass parseEmissionClass(const SUMOSAXAttributes &attrs, const std::string &type,
            const std::string &id) throw();


    /** @brief Parses the vehicle class
     *
     * When given, the vehicle class is parsed using getVehicleShapeID.
     *  Exceptions occuring within this process are catched and reported.
     *
     * If no vehicle class is available in the attributes, the default class (SVS_UNKNOWN)
     *  is returned.
     *
     * @param[in] attrs The attributes to read the class from
     * @param[in] type The data type (element) that is processed, for error message generation
     * @param[in] id The id of the parsed element, for error message generation
     * @return The parsed vehicle shape
     * @see SUMOVehicleShape
     * @todo Recheck how errors are handled and what happens if they occure
     */
    static SUMOVehicleShape parseGuiShape(const SUMOSAXAttributes &attrs, const std::string &type,
                                          const std::string &id) throw();


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
    static void parseCommonAttributes(const SUMOSAXAttributes &attrs,
            SUMOVehicleParameter *ret, std::string element) throw(ProcessError);


};


#endif

/****************************************************************************/

