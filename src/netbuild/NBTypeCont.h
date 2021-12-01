/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    NBTypeCont.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Tue, 20 Nov 2001
///
// A storage for available edgeTypes of edges
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <map>
#include <utils/common/SUMOVehicleClass.h>
#include <netbuild/NBEdge.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBTypeCont
 * @brief A storage for available edgeTypes of edges
 *
 * NBTypeCont stores properties of edge-types of edges. Additionally, a default
 *  edgeType is stored which is used if no edgeType information is given.
 *
 * This structure also contains a structure for determining node edgeTypes using edge
 *  speeds.
 */
class NBTypeCont {

public:
    /// @name struct declaration
    struct EdgeTypeDefinition;

    /// @brief laneType definition
    struct LaneTypeDefinition {

        /// @brief default Constructor
        LaneTypeDefinition();

        /// @brief constructor (using parameters of EdgeTypeDefinition)
        LaneTypeDefinition(const EdgeTypeDefinition* edgeTypeDefinition);

        /// @brief parameter constructor
        LaneTypeDefinition(const double speed, const double width, SVCPermissions permissions, const std::set<SumoXMLAttr> &attrs);

        /// @brief copy constructor
        LaneTypeDefinition(const LaneTypeDefinition* laneTypeDefinition);

        /// @brief The maximal velocity on a lane in m/s
        double speed;

        /// @brief List of vehicle edgeTypes that are allowed on this lane
        SVCPermissions permissions;

        /// @brief lane width [m]
        double width;

        /// @brief The vehicle class specific speed restrictions
        std::map<SUMOVehicleClass, double> restrictions;

        /// @brief The attributes which have been set
        std::set<SumoXMLAttr> attrs;
    };

    /// @brief edgeType definition
    struct EdgeTypeDefinition {

        /// @brief Constructor
        EdgeTypeDefinition();

        /// @brief copy constructor
        EdgeTypeDefinition(const EdgeTypeDefinition* edgeType);

        /// @brief parameter Constructor
        EdgeTypeDefinition(int numLanes, double speed, int priority,
                           double width, SVCPermissions permissions, LaneSpreadFunction spreadType,
                           bool oneWay, double sideWalkWidth, double bikeLaneWidth,
                           double widthResolution, double maxWidth, double minWidth);

        /// @brief whether any lane attributes deviate from the edge attributes
        bool needsLaneType() const;

        /// @brief The maximal velocity on an edge in m/s
        double speed;

        /// @brief The priority of an edge
        int priority;

        /// @brief List of vehicle edgeTypes that are allowed on this edge
        SVCPermissions permissions;

        /// @brief lane spread type
        LaneSpreadFunction spreadType;

        /// @brief Whether one-way traffic is mostly common for this edgeType (mostly unused)
        bool oneWay;

        /// @brief Whether edges of this edgeType shall be discarded
        bool discard;

        /// @brief The width of lanes of edges of this edgeType [m]
        double width;

        /// @brief The resolution for interpreting custom (noisy) lane widths of this edgeType [m]
        double widthResolution;

        /// @brief The maximum width for lanes of this edgeType [m]
        double maxWidth;

        /// @brief The minimum width for lanes of this edgeType [m]
        double minWidth;

        /* @brief The width of the sidewalk that should be added as an additional lane
        * a value of NBEdge::UNSPECIFIED_WIDTH indicates that no sidewalk should be added */
        double sidewalkWidth;

        /* @brief The width of the bike lane that should be added as an additional lane
        * a value of NBEdge::UNSPECIFIED_WIDTH indicates that no bike lane should be added */
        double bikeLaneWidth;

        /// @brief The vehicle class specific speed restrictions
        std::map<SUMOVehicleClass, double> restrictions;

        /// @brief The attributes which have been set
        std::set<SumoXMLAttr> attrs;

        /// @brief vector with LaneTypeDefinitions
        std::vector<LaneTypeDefinition> laneTypeDefinitions;
    };

    /// @brief A container of edgeTypes, accessed by the string id
    typedef std::map<std::string, EdgeTypeDefinition*> TypesCont;

    /// @brief Constructor
    NBTypeCont();

    /// @brief Destructor
    ~NBTypeCont();

    /// @brief clear types
    void clearTypes();

    /**@brief Sets the default values
     * @param[in] defaultNumLanes The default number of lanes an edge has
     * @param[in] defaultLaneWidth The default width of lanes
     * @param[in] defaultSpeed The default speed allowed on an edge
     * @param[in] defaultPriority The default priority of an edge
     * @param[in] defaultPermissions The default permissions of an edge
     * @param[in] defaultSpreadType The default lane spread type of an edge
     */
    void setEdgeTypeDefaults(int defaultNumLanes, double defaultLaneWidth,
                             double defaultSpeed, int defaultPriority,
                             SVCPermissions defaultPermissions,
                             LaneSpreadFunction defaultSpreadType);

    /**@brief Adds a edgeType into the list
     * @param[in] id The id of the edgeType
     * @param[in] numLanes The number of lanes an edge of this edgeType has
     * @param[in] maxSpeed The speed allowed on an edge of this edgeType
     * @param[in] prio The priority of an edge of this edgeType
     * @param[in] permissions The encoding of vehicle classes allowed on an edge of this edgeType
     * @param[in] spreadType Defines how to compute the lane geometry from the edge geometry (right, center or roadCenter)
     * @param[in] width The width of lanes of edgesof this edgeType
     * @param[in] oneWayIsDefault Whether edges of this edgeType are one-way per default
     * @return Whether the edgeType could be added (no edgeType with the same id existed)
     */
    void insertEdgeType(const std::string& id, int numLanes,
                        double maxSpeed, int prio,
                        SVCPermissions permissions,
                        LaneSpreadFunction spreadType,
                        double width,
                        bool oneWayIsDefault,
                        double sidewalkWidth,
                        double bikeLaneWidth,
                        double widthResolution,
                        double maxWidth,
                        double minWidth);

    /**@brief Adds a edgeType into the list
     * @param[in] id The id of the edgeType
     * @param[in] edgeType pointer to EdgeTypeDefinition (for copying parameters)
     */
    void insertEdgeType(const std::string& id, const EdgeTypeDefinition* edgeType);

    /**@brief Adds a laneType into the list
     * @param[in] maxSpeed The speed allowed on an lane of this edgeType
     * @param[in] prio The priority of an lane of this edgeType
     * @param[in] permissions The encoding of vehicle classes allowed on an lane of this edgeType
     * @param[in] width The width of lane of this edgeType
     * @param[in] attrs enabled attributes
     */
    void insertLaneType(const std::string& edgeTypeID,
                        int index,
                        double maxSpeed,
                        SVCPermissions permissions,
                        double width,
                        const std::set<SumoXMLAttr>& attrs);

    /**@brief Returns the number of known edgeTypes
     * @return The number of known edge edgeTypes (excluding the default)
     */
    int size() const;

    /**@brief Remove a edgeType from the list
     * @param[in] id The id of the edgeType
     * @return Whether the edgeType could be added (no edgeType with the same id existed)
     */
    void removeEdgeType(const std::string& id);

    /**@brief change edge type ID
    * @param[in] oldID The old id of the edgeType
    * @param[in] newID The new id of the edgeType
    */
    void updateEdgeTypeID(const std::string& oldId, const std::string& newId);

    /// @brief return begin iterator
    TypesCont::const_iterator begin() const;

    /// @brief return end iterator
    TypesCont::const_iterator end() const;

    /**@brief Returns whether the named edgeType is in the container
     * @return Whether the named edgeType is known
     */
    bool knows(const std::string& edgeType) const;

    /**@brief Retrieve the name or the default edgeType
     *
     * If no name is given, the default edgeType is returned
     * @param[in] name The name of the edgeType to retrieve
     * @return The named edgeType
     */
    const EdgeTypeDefinition* getEdgeType(const std::string& name) const;


    /**@brief Marks a edgeType as to be discarded
     * @param[in] id The id of the edgeType
     */
    bool markEdgeTypeAsToDiscard(const std::string& id);

    /**@brief Marks an attribute of a edgeType as set
     * @param[in] id The id of the edgeType
     * @param[in] attr The id of the attribute
     */
    bool markEdgeTypeAsSet(const std::string& id, const SumoXMLAttr attr);

    /**@brief Adds a restriction to a edgeType
     * @param[in] id The id of the edgeType
     * @param[in] svc The vehicle class the restriction refers to
     * @param[in] speed The restricted speed
     */
    bool addEdgeTypeRestriction(const std::string& id, const SUMOVehicleClass svc, const double speed);

    /**@brief Copy restrictions to a edgeType
     * @param[in] fromId The id of the source edgeType
     * @param[in] toId The id of the destination edgeType
     */
    bool copyEdgeTypeRestrictionsAndAttrs(const std::string& fromId, const std::string& toId);

    /**@brief Marks an attribute of last laneType as set
    * @param[in] id The id of the edgeType parent
    * @param[in] attr The id of the attribute
    */
    bool markLaneTypeAsSet(const std::string& id, int index, const SumoXMLAttr attr);

    /**@brief Adds a restriction to last laneType
    * @param[in] id The id of the edgeType parent
    * @param[in] svc The vehicle class the restriction refers to
    * @param[in] speed The restricted speed
    */
    bool addLaneTypeRestriction(const std::string& id, const SUMOVehicleClass svc, const double speed);

    /// @brief writes all EdgeTypes (and their lanes) as XML
    void writeEdgeTypes(OutputDevice& into) const;

    /// @name Type-dependant Retrieval methods
    /// @{

    /**@brief Returns the number of lanes for the given edgeType
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the lane number for
     * @return The number of lanes an edge of this edgeType has
     */
    int getEdgeTypeNumLanes(const std::string& edgeType) const;

    /**@brief Returns the maximal velocity for the given edgeType [m/s]
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the speed for
     * @return The allowed speed on edges of this edgeType
     */
    double getEdgeTypeSpeed(const std::string& edgeType) const;

    /**@brief Returns the priority for the given edgeType
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the priority for
     * @return The priority of edges of this edgeType
     */
    int getEdgeTypePriority(const std::string& edgeType) const;

    /**@brief Returns whether edges are one-way per default for the given edgeType
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the one-way information for
     * @return Whether edges of this edgeType are one-way per default
     * @todo There is no default for one-way!?
     */
    bool getEdgeTypeIsOneWay(const std::string& edgeType) const;

    /**@brief Returns the information whether edges of this edgeType shall be discarded.
     *
     * Returns false if the edgeType is not known.
     * @param[in] edgeType The id of the edgeType
     * @return Whether edges of this edgeType shall be discarded.
     */
    bool getEdgeTypeShallBeDiscarded(const std::string& edgeType) const;

    /**@brief Returns the resolution for interpreting edge/lane widths of the given
     * edgeType
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the width resolution for
     * @return The width resolution on edges of this edgeType
     */
    double getEdgeTypeWidthResolution(const std::string& edgeType) const;

    /**@brief Returns the maximum edge/lane widths of the given
     * edgeType
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the maximum width for
     * @return The maximum width on edges of this edgeType
     */
    double getEdgeTypeMaxWidth(const std::string& edgeType) const;

    /**@brief Returns the minimum edge/lane widths of the given
     * edgeType
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the maximum width for
     * @return The minimum width on edges of this edgeType
     */
    double getEdgeTypeMinWidth(const std::string& edgeType) const;

    /**@brief Returns whether an attribute of a edgeType was set
     * @param[in] edgeType The id of the edgeType
     * @param[in] attr The id of the attribute
     * @return Whether the attribute was set
     */
    bool wasSetEdgeTypeAttribute(const std::string& edgeType, const SumoXMLAttr attr) const;

    /**@brief Returns allowed vehicle classes for the given edgeType
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the list of allowed vehicles classes for
     * @return List of vehicles class which may use edges of the given edgeType
     */
    SVCPermissions getEdgeTypePermissions(const std::string& edgeType) const;

    /**@brief Returns spreadType for the given edgeType
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the spread type
     * @return spread type which may use edges of the given edgeType
     */
    LaneSpreadFunction getEdgeTypeSpreadType(const std::string& edgeType) const;

    /**@brief Returns the lane width for the given edgeType [m]
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the width for
     * @return The width of lanes of edges of this edgeType
     */
    double getEdgeTypeWidth(const std::string& edgeType) const;

    /**@brief Returns the lane width for a sidewalk to be added [m]
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the width for
     * @return The width of lanes of edges of this edgeType
     */
    double getEdgeTypeSidewalkWidth(const std::string& edgeType) const;

    /**@brief Returns the lane width for a bike lane to be added [m]
     *
     * If the named edgeType is not known, the default is returned
     * @param[in] edgeType The name of the edgeType to return the width for
     * @return The width of lanes of edges of this edgeType
     */
    double getEdgeTypeBikeLaneWidth(const std::string& edgeType) const;

    /// @}

protected:
    /// @brief The default edgeType
    EdgeTypeDefinition* myDefaultType;

    /// @brief The container of edgeTypes
    TypesCont myEdgeTypes;

private:
    /// @brief invalid copy constructor
    NBTypeCont(const NBTypeCont& s) = delete;

    /// @brief invalid assignment operator
    NBTypeCont& operator=(const NBTypeCont& s) = delete;
};
