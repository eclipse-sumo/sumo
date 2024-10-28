/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    SUMOVehicleParserHelper.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 07.04.2008
///
// Helper methods for parsing vehicle attributes
/****************************************************************************/
#pragma once
#include <config.h>

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
     * @param[in] tag SumoXMLTag (used in netedit)
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[in] hardFail enable or disable hard fails if a parameter is invalid
     * @param[in] needID check if flow needs an Id (used by Calibrator flows)
     * @param[in] allowInternalRoutes whether references to internal routes are allowed in this context
     * @return The parsed attribute structure if no error occurred, 0 otherwise
     * @exception ProcessError If an attribute's value is invalid
     * @note: the caller is responsible for deleting the returned pointer
     */
    static SUMOVehicleParameter* parseFlowAttributes(SumoXMLTag tag, const SUMOSAXAttributes& attrs, const bool hardFail, const bool needID, const SUMOTime beginDefault, const SUMOTime endDefault, const bool allowInternalRoutes = false);

    /** @brief Parses a vehicle's attributes
     *
     * Parses all attributes stored in "SUMOVehicleParameter".
     *
     * @see SUMOVehicleParameter
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[in] hardFail enable or disable hard fails if a parameter is invalid
     * @param[in] optionalID Whether the id shall be skipped (Used only in Calibrator Flows)
     * @param[in] skipDepart Whether parsing the departure time shall be skipped
     * @param[in] allowInternalRoutes whether references to internal routes are allowed in this context
     * @return The parsed attribute structure if no error occurred, 0 otherwise
     * @exception ProcessError If an attribute's value is invalid
     * @note: the caller is responsible for deleting the returned pointer
     */
    static SUMOVehicleParameter* parseVehicleAttributes(int element, const SUMOSAXAttributes& attrs, const bool hardFail, const bool optionalID = false, const bool skipDepart = false, const bool allowInternalRoutes = false);

    /** @brief Starts to parse a vehicle type
     *
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[in] hardFail enable or disable hard fails if a parameter is invalid
     * @param[in] file The name of the file being parsed (for resolving paths)
     * @param[in] hardFail enable or disable hard fails if a parameter is invalid
     * @exception ProcessError If an attribute's value is invalid and hardFail is enabled
     * @see SUMOVTypeParameter
     * @note: the caller is responsible for deleting the returned pointer
     */
    static SUMOVTypeParameter* beginVTypeParsing(const SUMOSAXAttributes& attrs, const bool hardFail, const std::string& file);

    /** @brief Parse string containing AngleTimes triplets (angle, entry time, exit time)
     *
     * @param[in] vtype - the vtype element constructed in the parser
     * @param[in] string - containing , separated  AngleTimes triplets "angle entry time exit time"
     * @param[in] hardFail enable or disable hard fails if a parameter is invalid
     * @exception ProcessError If an attribute's value is invalid
     *
     * @note  if the map parameter set is an empty string then the vtype map will not be changed
     */
    static bool parseAngleTimesMap(SUMOVTypeParameter* vtype, const std::string);

    /** @brief Parses Car Following Mode params
     *
     * @param[in, filled] into The structure to fill with parsed values
     * @param[in] element The id of the currently parsed XML-element
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[in] hardFail enable or disable hard fails if a parameter is invalid
     * @param[in] nestedCFM Whether the attributes are nested
     * @exception ProcessError If an attribute's value is invalid
     * @see SUMOVTypeParameter
     */
    static bool parseCFMParams(SUMOVTypeParameter* into, const SumoXMLTag element, const SUMOSAXAttributes& attrs, const bool nestedCFM);

    /// @brief Parses lane change model attributes
    static bool parseLCParams(SUMOVTypeParameter* into, LaneChangeModel model, const SUMOSAXAttributes& attrs);

    /// @brief Parses junction model attributes
    static bool parseJMParams(SUMOVTypeParameter* into, const SUMOSAXAttributes& attrs);

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
    static double parseWalkPos(SumoXMLAttr attr, const bool hardFail, const std::string& id, double maxPos, const std::string& val, SumoRNG* rng = 0);

    /** @brief Checks and converts given value for the action step length from seconds
     *   to miliseconds assuring it being a positive multiple of the simulation step width
     *
     *   @param[in] given The value parsed from the configuration (seconds).
     *   @return The milisecond value rounded to the next positive multiple of the simulation step length.
     */
    static SUMOTime processActionStepLength(double given);

    /** @brief Checks whether the route ID uses the syntax of internal routes.
     *
     *   @param[in] id The route ID to check.
     *   @return The given route ID follows the syntax of internal routes.
     */
    static bool isInternalRouteID(const std::string& id);

    static int parseCarWalkTransfer(const OptionsCont& oc, const bool hasTaxi = false);
private:
    /**@brief parse ID
     * @return empty string if ID obtained from attrs isn't valid
     */
    static std::string parseID(const SUMOSAXAttributes& attrs, const SumoXMLTag element);

    /** @brief Parses attributes common to vehicles and flows
     *
     * Parses all attributes stored in "SUMOVehicleParameter".
     *
     * @see SUMOVehicleParameter
     * @param[in] attr The SAX-attributes to get vehicle parameter from
     * @param[out] ret The parameter to parse into
     * @param[in] element The name of the element (vehicle or flow)
     * @param[in] allowInternalRoutes Whether internal routes are valid in this context
     * @exception ProcessError If an attribute's value is invalid
     */
    static void parseCommonAttributes(const SUMOSAXAttributes& attrs, SUMOVehicleParameter* ret, SumoXMLTag tag, const bool allowInternalRoutes = false);

    /// @brief handle error loading SUMOVehicleParameter
    static SUMOVehicleParameter* handleVehicleError(const bool hardFail, SUMOVehicleParameter* vehicleParameter, const std::string message = "");

    /// @brief handle error loading SUMOVTypeParameter
    static SUMOVTypeParameter* handleVehicleTypeError(const bool hardFail, SUMOVTypeParameter* vehicleTypeParameter, const std::string message = "");

    /// @brief Car-Following attributes map
    typedef std::map<SumoXMLTag, std::set<SumoXMLAttr> > CFAttrMap;

    /// @brief Lane-Change-Model attributes map
    typedef std::map<LaneChangeModel, std::set<SumoXMLAttr> > LCAttrMap;

    /// @brief returns allowed attrs for each known CF-model (init on first use)
    static const CFAttrMap& getAllowedCFModelAttrs();

    /// @brief allowed attrs for each known CF-model
    static CFAttrMap allowedCFModelAttrs;

    /// @brief allowed attrs for each known LC-model
    static LCAttrMap allowedLCModelAttrs;

};
