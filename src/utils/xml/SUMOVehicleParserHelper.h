/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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

#include <random>
#include <string>
#include <utils/common/SUMOTime.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOSAXAttributes;
class SUMOVehicleParameter;
class SUMOVTypeParameter;


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
     * @param[in] isPerson   Whether a person is parsed
     * @return The parsed attribute structure if no error occured, 0 otherwise
     * @exception ProcessError If an attribute's value is invalid
     * @note: the caller is responsible for deleting the returned pointer
     */
    static SUMOVehicleParameter* parseVehicleAttributes(const SUMOSAXAttributes& attrs,
            const bool optionalID = false, const bool skipDepart = false, const bool isPerson = false);


    /** @brief Starts to parse a vehicle type
     *
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[in] file The name of the file being parsed (for resolving paths)
     * @param[in] defaultCFModel The XML tag of the default car following model (SUMO_TAG_NOTHING means no default)
     * @exception ProcessError If an attribute's value is invalid
     * @see SUMOVTypeParameter
     * @note: the caller is responsible for deleting the returned pointer
     */
    static SUMOVTypeParameter* beginVTypeParsing(const SUMOSAXAttributes& attrs, const std::string& file,
            const SumoXMLTag defaultCFModel);


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
                                   const SumoXMLTag element, const SUMOSAXAttributes& attrs,
                                   const bool fromVType = false);

    /// @brief Parses lane change model attributes
    static void parseLCParams(SUMOVTypeParameter& into, LaneChangeModel model, const SUMOSAXAttributes& attrs);

    /// @brief Parses junction model attributes
    static void parseJMParams(SUMOVTypeParameter& into, const SUMOSAXAttributes& attrs);

    /** @brief Parses the vehicle class
     *
     * When given, the vehicle class is parsed using getVehicleClassID.
     *  Exceptions occuring within this process are catched and reported.
     *
     * If no vehicle class is available in the attributes, the default class (SVC_IGNORING)
     *  is returned.
     *
     * @param[in] attrs The attributes to read the class from
     * @param[in] id The id of the parsed element, for error message generation
     * @return The parsed vehicle class
     * @see SUMOVehicleClass
     * @todo Recheck how errors are handled and what happens if they occure
     */
    static SUMOVehicleClass parseVehicleClass(const SUMOSAXAttributes& attrs, const std::string& id);


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

    /// @brief parse departPos or arrivalPos for a walk
    static double parseWalkPos(SumoXMLAttr attr, const std::string& id, double maxPos, const std::string& val, std::mt19937* rng = 0);


    /** @brief Checks and converts given value for the action step length from seconds
     *   to miliseconds assuring it being a positive multiple of the simulation step width
     *
     *   @param[in] given The value parsed from the configuration (seconds).
     *   @return The milisecond value rounded to the next positive multiple of the simulation step length.
     */
    static SUMOTime processActionStepLength(double given);


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
    typedef std::map<LaneChangeModel, std::set<SumoXMLAttr> > LCAttrMap;

    // returns allowed attrs for each known CF-model (init on first use)
    static const CFAttrMap& getAllowedCFModelAttrs();

    // brief allowed attrs for each known CF-model
    static CFAttrMap allowedCFModelAttrs;
    // brief allowed attrs for each known LC-model
    static LCAttrMap allowedLCModelAttrs;
    // brief allowed attrs for the junction model
    static std::set<SumoXMLAttr> allowedJMAttrs;


};


#endif

/****************************************************************************/

