/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
            contextDomain(contextDomainArg), range(rangeArg) {}

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
    };
}


#endif
