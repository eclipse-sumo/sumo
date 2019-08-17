/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Subscription.h
/// @author  Michael Behrisch
/// @date    2007/10/24
/// @version $Id$
///
// Subscription representation for libsumo and TraCI
/****************************************************************************/
#ifndef Subscription_h
#define Subscription_h


// ===========================================================================
// included modules
// ===========================================================================
#include <vector>
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// class definitions
// ===========================================================================
namespace libsumo {

/** @brief Filter types for context subscriptions
 */
enum SubscriptionFilterType {
    // No filters
    SUBS_FILTER_NONE = 0,
    // Filter by list of lanes relative to ego vehicle
    SUBS_FILTER_LANES = 1,
    // Exclude vehicles on opposite (and other) lanes from context subscription result
    SUBS_FILTER_NOOPPOSITE = 1 << 1,
    // Specify maximal downstream distance for vehicles in context subscription result
    SUBS_FILTER_DOWNSTREAM_DIST = 1 << 2,
    // Specify maximal upstream distance for vehicles in context subscription result
    SUBS_FILTER_UPSTREAM_DIST = 1 << 3,
    // Only return leader and follower on specified lanes in context subscription result
    SUBS_FILTER_LEAD_FOLLOW = 1 << 4,
    // Only return foes on upcoming junction in context subscription result
    SUBS_FILTER_TURN = 1 << 6,
    // Only return vehicles of the given vClass in context subscription result
    SUBS_FILTER_VCLASS = 1 << 7,
    // Only return vehicles of the given vType in context subscription result
    SUBS_FILTER_VTYPE = 1 << 8,
    // Filter category for measuring distances along the road network instead of using the usual rtree query
    SUBS_FILTER_NO_RTREE = SUBS_FILTER_DOWNSTREAM_DIST | SUBS_FILTER_UPSTREAM_DIST | SUBS_FILTER_LANES | SUBS_FILTER_TURN | SUBS_FILTER_LEAD_FOLLOW,
    // Filter category for maneuver filters
    SUBS_FILTER_MANEUVER = SUBS_FILTER_TURN | SUBS_FILTER_LEAD_FOLLOW,
};



/** @class Subscription
 * @brief Representation of a subscription
 */
class Subscription {
public:
    /** @brief Constructor
    * @param[in] commandIdArg The command id of the subscription
    * @param[in] idArg The id of the object that is subscribed
    * @param[in] variablesArg The subscribed variables
    * @param[in] beginTimeArg The begin time of the subscription
    * @param[in] endTimeArg The end time of the subscription
    * @param[in] contextDomainArg The domain ID of the context
    * @param[in] rangeArg The range of the context
    */
    Subscription(int commandIdArg, const std::string& idArg,
                 const std::vector<int>& variablesArg, const std::vector<std::vector<unsigned char> >& paramsArg,
                 SUMOTime beginTimeArg, SUMOTime endTimeArg, int contextDomainArg, double rangeArg)
        : commandId(commandIdArg), id(idArg), variables(variablesArg), parameters(paramsArg), beginTime(beginTimeArg), endTime(endTimeArg),
          contextDomain(contextDomainArg), range(rangeArg), activeFilters(SUBS_FILTER_NONE),
          filterLanes(), filterDownstreamDist(-1), filterUpstreamDist(-1), filterVTypes(), filterVClasses(0) {}

    /// @brief commandIdArg The command id of the subscription
    int commandId;
    /// @brief The id of the object that is subscribed
    std::string id;
    /// @brief The subscribed variables
    std::vector<int> variables;
    /// @brief The parameters for the subscribed variables
    std::vector<std::vector<unsigned char> > parameters;
    /// @brief The begin time of the subscription
    SUMOTime beginTime;
    /// @brief The end time of the subscription
    SUMOTime endTime;
    /// @brief The domain ID of the context
    int contextDomain;
    /// @brief The range of the context
    double range;

    /// @brief Active filters for the subscription (bitset, @see SubscriptionFilterType)
    int activeFilters;
    /// @brief lanes specified by the lanes filter
    std::vector<int> filterLanes;
    /// @brief Downstream distance specified by the downstream distance filter
    double filterDownstreamDist;
    /// @brief Upstream distance specified by the upstream distance filter
    double filterUpstreamDist;
    /// @brief vTypes specified by the vTypes filter
    std::set<std::string> filterVTypes;
    /// @brief vClasses specified by the vClasses filter, @see SVCPermissions
    int filterVClasses;
};

class VariableWrapper {
public:
    /// @brief Definition of a method to be called for serving an associated commandID
    typedef bool(*SubscriptionHandler)(const std::string& objID, const int variable, VariableWrapper* wrapper);
    VariableWrapper(SubscriptionHandler handler = nullptr) : handle(handler) {}
    SubscriptionHandler handle;
    virtual void setContext(const std::string& /* refID */) {}
    virtual void clear() {}
    virtual bool wrapDouble(const std::string& objID, const int variable, const double value) = 0;
    virtual bool wrapInt(const std::string& objID, const int variable, const int value) = 0;
    virtual bool wrapString(const std::string& objID, const int variable, const std::string& value) = 0;
    virtual bool wrapStringList(const std::string& objID, const int variable, const std::vector<std::string>& value) = 0;
    virtual bool wrapPosition(const std::string& objID, const int variable, const TraCIPosition& value) = 0;
    virtual bool wrapColor(const std::string& objID, const int variable, const TraCIColor& value) = 0;
    virtual bool wrapRoadPosition(const std::string& objID, const int variable, const TraCIRoadPosition& value) = 0;
};
}


#endif
