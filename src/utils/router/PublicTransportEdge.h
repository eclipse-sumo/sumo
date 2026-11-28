/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    PublicTransportEdge.h
/// @author  Michael Behrisch
/// @date    Mon, 03 March 2014
///
// The PublicTransportEdge is a special intermodal edge connecting the stop edges with scheduled traffic
/****************************************************************************/
#pragma once
#include <config.h>

#include <algorithm>
#include <vector>
#include "IntermodalEdge.h"


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the public transport edge type connecting the stop edges
template<class E, class L, class N, class V>
class PublicTransportEdge : public IntermodalEdge<E, L, N, V> {
private:
    struct Schedule {
        Schedule(const std::string& _id, const SUMOTime _begin, const long long int _repetitionNumber, const SUMOTime _period, const SUMOTime _travelTime, const bool _isFlow)
            : ids({
            _id
        }), begin(_begin), repetitionNumber(_repetitionNumber), period(_period),
        end(_repetitionNumber <= 1 ? _begin : (_repetitionNumber - 1 > (SUMOTime_MAX - _begin) / _period ? SUMOTime_MAX : _begin + (_repetitionNumber - 1) * _period)),
        travelTime(_travelTime), isFlow(_isFlow) {}
        // the id of the vehicle or flow from which this schedule is generated
        std::vector<std::string> ids;
        SUMOTime begin;
        long long int repetitionNumber;
        // the repetition period for a flow or 1 for a single vehicle
        SUMOTime period;
        SUMOTime end;
        SUMOTime travelTime;
        bool isFlow;
    };

public:
    PublicTransportEdge(const std::string id, int numericalID, const IntermodalEdge<E, L, N, V>* entryStop, const E* endEdge, const std::string& line, const double length) :
        IntermodalEdge<E, L, N, V>(line + ":" + (id != "" ? id : endEdge->getID()), numericalID, endEdge, line, length), myEntryStop(entryStop) { }

    bool includeInRoute(bool /* allEdges */) const {
        return true;
    }

    bool prohibits(const IntermodalTrip<E, N, V>* const trip) const {
        return (trip->modeSet & SVC_BUS) == 0;
    }

    const IntermodalEdge<E, L, N, V>* getEntryStop() const {
        return myEntryStop;
    }

    bool hasSchedule(const SUMOTime begin) const {
        for (const Schedule& s : mySchedules) {
            if (s.begin >= begin) {
                return s.begin == begin;
            }
        }
        return false;
    }

    void addSchedule(const std::string id, const SUMOTime begin, const long long int repetitionNumber, const SUMOTime period, const SUMOTime travelTime) {
        // try to merge with existing vehicle schedule
        bool found = false;
        if (repetitionNumber < 0) {
            for (Schedule& s : mySchedules) {
                if (!s.isFlow && travelTime == s.travelTime) {
                    if (s.repetitionNumber == 1) {
                        if (begin > s.begin) {
                            s.period = begin - s.begin;
                            found = true;
                        }
                    } else if (begin == s.begin + s.repetitionNumber * s.period) {
                        found = true;
                    }
                    if (found) {
                        s.repetitionNumber += 1;
                        s.end = s.begin + (s.repetitionNumber - 1) * s.period;
                        s.ids.push_back(id);
                        break;
                    }
                }
            }
        }
        if (!found) {
            const Schedule newSched(id, begin, MAX2(repetitionNumber, 1ll), MAX2<SUMOTime>(period, 1), travelTime, repetitionNumber >= 0);
            auto it = std::upper_bound(mySchedules.begin(), mySchedules.end(), begin,
            [](const SUMOTime b, const Schedule& s) {
                return b < s.begin;
            });
            mySchedules.insert(it, newSched);
        }
    }

    double getTravelTime(const IntermodalTrip<E, N, V>* const /* trip */, double time) const {
        SUMOTime minArrival = SUMOTime_MAX;
        const SUMOTime step = TIME2STEPS(time);
        for (const Schedule& s : mySchedules) {
            if (s.begin > minArrival) {
                break;
            }
            if (step <= s.begin) {
                minArrival = MIN2(s.begin + s.travelTime, minArrival);
            } else if (step <= s.end) {
                const long long int running = (step - s.begin - 1) / s.period + 1;
                if (running < s.repetitionNumber) {
                    minArrival = MIN2(s.begin + running * s.period + s.travelTime, minArrival);
                }
            }
        }
        if (minArrival != SUMOTime_MAX) {
            return STEPS2TIME(minArrival - step);
        } else {
            // indicate failure
            return std::numeric_limits<double>::max();
        }
    }

    double getIntended(const double time, std::string& intended) const {
        /// @note: duplicates some code of getTravelTime()
        SUMOTime minArrival = SUMOTime_MAX;
        double bestDepartTime = std::numeric_limits<double>::max();
        const SUMOTime step = TIME2STEPS(time);
        for (const Schedule& s : mySchedules) {
            if (s.begin > minArrival) {
                break;
            }
            if (step <= s.end) {
                const long long int running = step <= s.begin ? 0 : (step - s.begin - 1) / s.period + 1;
                if (running < s.repetitionNumber) {
                    const SUMOTime nextDepart = s.begin + running * s.period;
                    if (nextDepart + s.travelTime < minArrival) {
                        minArrival = nextDepart + s.travelTime;
                        bestDepartTime = STEPS2TIME(nextDepart);
                        // see naming scheme in MSInsertionControl::determineCandidates()
                        if (s.ids.size() == 1 || running >= (long long int)s.ids.size()) {
                            intended = s.repetitionNumber == 1 ? s.ids[0] : s.ids[0] + "." + toString(running);
                        } else {
                            intended = s.ids[(size_t)running];
                        }
                    }
                }
            }
        }
        return bestDepartTime;
    }

private:
    std::vector<Schedule> mySchedules;
    const IntermodalEdge<E, L, N, V>* const myEntryStop;

};
