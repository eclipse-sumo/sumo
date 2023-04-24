/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    SUMOVTypeParameter.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    10.09.2009
///
// Structure representing possible vehicle parameter
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <map>
#include <utils/common/Parameterised.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/distribution/Distribution_Parameterized.h>

// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class OptionsCont;


// ===========================================================================
// value definitions
// ===========================================================================
const long long int VTYPEPARS_LENGTH_SET = 1;
const long long int VTYPEPARS_MINGAP_SET = 1 << 1;
const long long int VTYPEPARS_MAXSPEED_SET = 1 << 2;
const long long int VTYPEPARS_PROBABILITY_SET = 1 << 3;
const long long int VTYPEPARS_SPEEDFACTOR_SET = 1 << 4;
const long long int VTYPEPARS_EMISSIONCLASS_SET = 1 << 5;
const long long int VTYPEPARS_COLOR_SET = 1 << 6;
const long long int VTYPEPARS_VEHICLECLASS_SET = 1 << 7;
const long long int VTYPEPARS_WIDTH_SET = 1 << 8;
const long long int VTYPEPARS_HEIGHT_SET = 1 << 9;
const long long int VTYPEPARS_SHAPE_SET = 1 << 10;
const long long int VTYPEPARS_OSGFILE_SET = 1 << 11;
const long long int VTYPEPARS_IMGFILE_SET = 1 << 12;
const long long int VTYPEPARS_IMPATIENCE_SET = 1 << 13;
const long long int VTYPEPARS_LANE_CHANGE_MODEL_SET = 1 << 14;
const long long int VTYPEPARS_PERSON_CAPACITY = 1 << 15;
const long long int VTYPEPARS_BOARDING_DURATION = 1 << 16;
const long long int VTYPEPARS_CONTAINER_CAPACITY = 1 << 17;
const long long int VTYPEPARS_LOADING_DURATION = 1 << 18;
const long long int VTYPEPARS_CAR_FOLLOW_MODEL = 1 << 19;
const long long int VTYPEPARS_MAXSPEED_LAT_SET = 1 << 20;
const long long int VTYPEPARS_LATALIGNMENT_SET = 1 << 21;
const long long int VTYPEPARS_MINGAP_LAT_SET = 1 << 22;
const long long int VTYPEPARS_ACTIONSTEPLENGTH_SET = 1 << 23;
const long long int VTYPEPARS_DESIRED_MAXSPEED_SET = 1 << 24;
const long long int VTYPEPARS_CARRIAGE_LENGTH_SET = 1 << 25;
const long long int VTYPEPARS_LOCOMOTIVE_LENGTH_SET = 1 << 26;
const long long int VTYPEPARS_CARRIAGE_GAP_SET = 1 << 27;
const long long int VTYPEPARS_MANEUVER_ANGLE_TIMES_SET = 1 << 28;
const long long int VTYPEPARS_FRONT_SEAT_POS_SET = 1 << 29;
const long long int VTYPEPARS_SCALE_SET = 1 << 30;
const long long int VTYPEPARS_MASS_SET = (long long int)1 << 31;
const long long int VTYPEPARS_TTT_SET = (long long int)1 << 32;
const long long int VTYPEPARS_TTT_BIDI_SET = (long long int)1 << 33;
const long long int VTYPEPARS_SEATING_WIDTH_SET = (long long int)1 << 34;
const long long int VTYPEPARS_SPEEDFACTOR_PREMATURE_SET = (long long int)1 << 35;


const int VTYPEPARS_DEFAULT_EMERGENCYDECEL_DEFAULT = -1;
const int VTYPEPARS_DEFAULT_EMERGENCYDECEL_DECEL = -2;


// ===========================================================================
// enum definitions
// ===========================================================================
/**
 * @enum LatAlignmentDefinition
 * @brief Possible ways to choose the lateral alignment, i.e., how vehicles align themselves within their lane
 */
enum class LatAlignmentDefinition {
    /// @brief No information given; use default
    DEFAULT,
    /// @brief The alignment as offset is given
    GIVEN,
    /// @brief drive on the right side
    RIGHT,
    /// @brief drive in the middle
    CENTER,
    /// @brief maintain the current alignment
    ARBITRARY,
    /// @brief align with the closest sublane border
    NICE,
    /// @brief align with the rightmost sublane that allows keeping the current speed
    COMPACT,
    /// @brief drive on the left side
    LEFT
};


// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @class SUMOVTypeParameter
 * @brief Structure representing possible vehicle parameter
 */
class SUMOVTypeParameter : public Parameterised {

public:
    /// @brief struct for default values that depend of VClass
    struct VClassDefaultValues {
        /// @brief parameter constructor
        VClassDefaultValues(SUMOVehicleClass vClass);

        /// @brief The physical vehicle length
        double length;

        /// @brief This class' free space in front of the vehicle itself
        double minGap;

        /// @brief The vehicle type's minimum lateral gap [m]
        double minGapLat;

        /// @brief The vehicle type's maximum speed [m/s] (technical limit, not subject to speed deviation)
        double maxSpeed;

        /// @brief The vehicle type's desired maximum speed [m/s]
        double desiredMaxSpeed;

        /// @brief This class' width
        double width;

        /// @brief This class' height
        double height;

        /// @brief This class' shape
        SUMOVehicleShape shape;

        /// @brief The emission class of this vehicle
        SUMOEmissionClass emissionClass;

        /// @brief This class' mass
        double mass;

        /// @brief The factor by which the maximum speed may deviate from the allowed max speed on the street
        Distribution_Parameterized speedFactor;

        /// @brief The person capacity of the vehicle
        int personCapacity;

        /// @brief The container capacity of the vehicle
        int containerCapacity;

        /// @brief 3D model file for this class
        std::string osgFile;

        /// @brief the length of train carriages
        double carriageLength;

        /// @brief the length of train locomotive
        double locomotiveLength;

        /// @brief the lateral alignment procedure
        LatAlignmentDefinition latAlignmentProcedure;

    private:
        /// @brief default constructor
        VClassDefaultValues();
    };

    /** @brief Constructor
     *
     * Initialises the structure with default values
     */
    SUMOVTypeParameter(const std::string& vtid, const SUMOVehicleClass vc = SVC_IGNORING);

    /// @brief virtual destructor
    virtual ~SUMOVTypeParameter() {};

    /** @brief Returns whether the given parameter was set
     * @param[in] what The parameter which one asks for
     * @return Whether the given parameter was set
     */
    bool wasSet(long long int what) const {
        return (parametersSet & what) != 0;
    }

    /** @brief Writes the vtype
     *
     * @param[in, out] dev The device to write into
     * @exception IOError not yet implemented
     */
    void write(OutputDevice& dev) const;

    /** @brief Returns the named value from the map, or the default if it is not contained there
     * @param[in] attr The corresponding xml attribute
     * @param[in] defaultValue The value to return if the given map does not contain the named variable
     * @return The named value from the map or the default if it does not exist there
     */
    double getCFParam(const SumoXMLAttr attr, const double defaultValue) const;

    /** @brief Returns the named value from the map, or the default if it is not contained there
    * @param[in] attr The corresponding xml attribute
    * @param[in] defaultValue The value to return if the given map does not contain the named variable
    * @return The named value from the map or the default if it does not exist there
    */
    std::string getCFParamString(const SumoXMLAttr attr, const std::string defaultValue) const;

    /** @brief Returns the named value from the map, or the default if it is not contained there
     * @param[in] attr The corresponding xml attribute
     * @param[in] defaultValue The value to return if the given map does not contain the named variable
     * @return The named value from the map or the default if it does not exist there
     */
    double getLCParam(const SumoXMLAttr attr, const double defaultValue) const;

    /** @brief Returns the named value from the map, or the default if it is not contained there
     * @param[in] attr The corresponding xml attribute
     * @param[in] defaultValue The value to return if the given map does not contain the named variable
     * @return The named value from the map or the default if it does not exist there
     */
    std::string getLCParamString(const SumoXMLAttr attr, const std::string& defaultValue) const;

    /// @brief sub-model parameters
    typedef std::map<SumoXMLAttr, std::string> SubParams;

    /// @brief Returns the LC parameter
    const SubParams& getLCParams() const;

    /** @brief Returns the named value from the map, or the default if it is not contained there
     * @param[in] attr The corresponding xml attribute
     * @param[in] defaultValue The value to return if the given map does not contain the named variable
     * @return The named value from the map or the default if it does not exist there
     */
    double getJMParam(const SumoXMLAttr attr, const double defaultValue) const;

    /** @brief Returns the named value from the map, or the default if it is not contained there
     * @param[in] attr The corresponding xml attribute
     * @param[in] defaultValue The value to return if the given map does not contain the named variable
     * @return The named value from the map or the default if it does not exist there
     */
    std::string getJMParamString(const SumoXMLAttr attr, const std::string defaultValue) const;

    void cacheParamRestrictions(const std::vector<std::string>& restrictionKeys);

    /// @brief init Rail Visualization Parameters
    void initRailVisualizationParameters();

    /// @brief The vehicle type's id
    std::string id;

    /// @brief The physical vehicle length
    double length;

    /// @brief This class' free space in front of the vehicle itself
    double minGap;

    /// @brief The vehicle type's (technical) maximum speed [m/s]
    double maxSpeed;

    /// @brief The vehicle type's desired maximum speed [m/s]
    double desiredMaxSpeed;

    /// @brief The vehicle type's default actionStepLength [ms], i.e. the interval between two control actions.
    ///        The default value of 0ms. induces the value to be traced from MSGlobals::gActionStepLength
    SUMOTime actionStepLength;

    /// @brief The probability when being added to a distribution without an explicit probability
    double defaultProbability;

    /// @brief The factor by which the maximum speed may deviate from the allowed max speed on the street
    Distribution_Parameterized speedFactor;

    /// @brief The emission class of this vehicle
    SUMOEmissionClass emissionClass;

    /// @brief The mass
    double mass;

    /// @brief The color
    RGBColor color;

    /// @brief The vehicle's class
    SUMOVehicleClass vehicleClass;

    /// @brief The vehicle's impatience (willingness to obstruct others)
    double impatience;

    /// @brief The person capacity of the vehicle
    int personCapacity;

    /// @brief The container capacity of the vehicle
    int containerCapacity;

    /// @brief The time a person needs to board the vehicle
    SUMOTime boardingDuration;

    /// @brief The time a container needs to get loaded on the vehicle
    SUMOTime loadingDuration;

    /// @brief individual scaling factor (-1 for undefined)
    double scale;

    /// @name Values for drawing this class' vehicles
    /// @{

    /// @brief This class' width
    double width;

    /// @brief This class' height
    double height;

    /// @brief This class' shape
    SUMOVehicleShape shape;

    /// @brief 3D model file for this class
    std::string osgFile;

    /// @brief Image file for this class
    std::string imgFile;
    /// @}


    /// @brief The enum-representation of the car-following model to use
    SumoXMLTag cfModel;

    /// @brief Car-following parameter
    SubParams cfParameter;

    /// @brief Lane-changing parameter
    SubParams lcParameter;

    /// @brief Junction-model parameter
    SubParams jmParameter;

    /// @brief The lane-change model to use
    LaneChangeModel lcModel;

    /// @brief The vehicle type's maximum lateral speed [m/s]
    double maxSpeedLat;

    /// @brief (optional) The vehicle's desired lateral alignment as offset in m from center line
    double latAlignmentOffset;

    /// @brief Information on how the vehicle shall choose the lateral alignment
    LatAlignmentDefinition latAlignmentProcedure;

    /// @brief The vehicle type's minimum lateral gap [m]
    double minGapLat;

    /// @brief the length of train carriages and locomotive
    double carriageLength;
    double locomotiveLength;
    double carriageGap;

    /// @brief the custom time-to-teleport for this type
    SUMOTime timeToTeleport;

    /// @brief the custom time-to-teleport.bidi for this type
    SUMOTime timeToTeleportBidi;

    /// @brief the possible speed reduction when a train is ahead of schedule
    double speedFactorPremature;

    /// @brief the offset of the first person seat from the front of the vehicle
    double frontSeatPos;

    /// @brief width to be used when comping seats
    double seatingWidth;

    /// @brief Information for the router which parameter were set
    long long int parametersSet;

    /// @brief Information whether this type was already saved (needed by routers)
    mutable bool saved;

    /// @brief Information whether this is a type-stub, being only referenced but not defined (needed by routers)
    bool onlyReferenced;

    /// @brief cached value of parameters which may restrict access to certain edges
    std::vector<double> paramRestrictions;

    /// @brief satisfy vType / router template requirements
    inline double getLength() const {
        return length;
    }

    /** @brief Returns the default acceleration for the given vehicle class
     * This needs to be a function because the actual value is stored in the car following model
     * @param[in] vc the vehicle class
     * @return the acceleration in m/s^2
     */
    static double getDefaultAccel(const SUMOVehicleClass vc = SVC_IGNORING);

    /** @brief Returns the default deceleration for the given vehicle class
     * This needs to be a function because the actual value is stored in the car following model
     * @param[in] vc the vehicle class
     * @return the deceleration in m/s^2
     */
    static double getDefaultDecel(const SUMOVehicleClass vc = SVC_IGNORING);

    /** @brief Returns the default emergency deceleration for the given vehicle class
     * This needs to be a function because the actual value is stored in the car following model
     * @param[in] vc the vehicle class
     * @param[in] decel the deceleration of the vehicle type
     * @return the emergency deceleration in m/s^2
     */
    static double getDefaultEmergencyDecel(const SUMOVehicleClass vc, double decel, double defaultOption);

    /** @brief Returns the default driver's imperfection (sigma or epsilon in Krauss' model) for the given vehicle class
     * This needs to be a function because the actual value is stored in the car following model
     * @param[in] vc the vehicle class
     * @return the imperfection as a value between 0 and 1
     */
    static double getDefaultImperfection(const SUMOVehicleClass vc = SVC_IGNORING);

    /// @brief return the default parameters, this is a function due to the http://www.parashift.com/c++-faq/static-init-order.html
    static const SUMOVTypeParameter& getDefault();

    /** @brief Parses and validates a given latAlignment value
     * @param[in] val The latAlignment value to parse
     * @param[out] lao The parsed lateral alignment offset, if given
     * @param[out] lad The parsed latAlignment definition
     * @return Whether the given value is a valid latAlignment definition
     */
    static bool parseLatAlignment(const std::string& val, double& lao, LatAlignmentDefinition& lad);

    static inline bool isValidLatAlignment(const std::string& val) {
        double lao;
        LatAlignmentDefinition lad;
        return SUMOVTypeParameter::parseLatAlignment(val, lao, lad);
    }

    /// @brief return all valid strings for latAlignment
    // XXX: does not include valid float strings
    static inline std::vector<std::string> getLatAlignmentStrings() {
        std::vector<std::string> result;
        result.push_back("right");
        result.push_back("center");
        result.push_back("arbitrary");
        result.push_back("nice");
        result.push_back("compact");
        result.push_back("left");
        return result;
    }

    /// @brief Map of manoeuver angles versus the times (entry, exit) to execute the manoeuver
    std::map<int, std::pair<SUMOTime, SUMOTime>>  myManoeuverAngleTimes;

    /** @brief Initialise the default mapping between manoeuver angle and times dependant on vehicle class
     *  @param[in] vclass The vehicle class
     *  @note  These default values were 'informed' by a paper by Purnawan, and Yousif:
     *  @note    usir.salford.ac.uk/id/eprint/9729/3/Paper_Kassel_%28Seminar%29.pdf (no reverse park values in paper)
     *  @note    truck values were simply doubled - all are modifiable in the vehicle type definition and there is no limit to the no of triplets
     *    TODO:
     *        optionality for 90 degree bay entry (forwards or reverse) not implemented - probably should be a driver propensity
     *        the defaults assume reverse entry - a reverse manoeuvre has to happen and there will be a small difference in timings depending whether its reverse in or out
     */
    void setManoeuverAngleTimes(const SUMOVehicleClass vclass);

    /** @brief Returns the time that will be needed for the vehicle type to execute the (entry) manoeuvre (and be blocking the lane)
     * @param[in] angle The angle, in degrees through which the vehicle needs to manoeuver (0-180 degrees)
     * @return The SUMOTime value
     */
    SUMOTime getEntryManoeuvreTime(const int angle) const;

    /** @brief Returns the time that will be needed for the vehicle type to execute the (exit) manoeuvre (and be blocking the lane)
      * @param[in] angle The angle, in degrees through which the vehicle needs to manoeuver (0-180 degrees)
      * @return The SUMOTime value
      */
    SUMOTime getExitManoeuvreTime(const int angle) const;

    /** @brief Returns myManoeuverAngleTimes as a string for xml output
     *  @return A string of , separated triplets (angle entry-time exit-time)
     */
    std::string getManoeuverAngleTimesS() const;

    /// @brief return time-to-teleport (either custom or default)
    SUMOTime getTimeToTeleport(SUMOTime defaultValue) const;

    /// @brief return time-to-teleport.bidi (either custom or default)
    SUMOTime getTimeToTeleportBidi(SUMOTime defaultValue) const;
};
