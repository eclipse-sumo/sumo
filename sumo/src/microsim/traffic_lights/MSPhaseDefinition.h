/****************************************************************************/
/// @file    MSPhaseDefinition.h
/// @author  Daniel Krajzewicz
/// @date    Jan 2004
/// @version $Id$
///
// The definition of a single phase of a tls logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSPhaseDefinition_h
#define MSPhaseDefinition_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <bitset>
#include <string>
#include <utils/common/SUMOTime.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSLink.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPhaseDefinition
 * @brief The definition of a single phase of a tls logic
 */
class MSPhaseDefinition {
public:
    /// @brief The duration of the phase
    SUMOTime duration;

    /// @brief The minimum duration of the phase
    SUMOTime minDuration;

    /// @brief The maximum duration of the phase
    SUMOTime maxDuration;

    /// @brief Stores the timestep of the last on-switched of the phase
    SUMOTime myLastSwitch;


public:
    /** @brief Constructor
     *
     * minDuration and maxDuration are set to duration.
     *
     * @param[in] durationArg The duration of the phase
     * @param[in] stateArg The state in the phase
     */
    MSPhaseDefinition(SUMOTime durationArg, const std::string &stateArg) throw()
            : duration(durationArg), minDuration(durationArg), maxDuration(durationArg),
            myLastSwitch(0), state(stateArg) {
        myLastSwitch = OptionsCont::getOptions().getInt("begin"); // SUMOTime-option
    }


    /** @brief Constructor
     *
     * @param[in] durationArg The duration of the phase
     * @param[in] minDurationArg The minimum duration of the phase
     * @param[in] maxDurationArg The maximum duration of the phase
     * @param[in] stateArg The state in the phase
     */
    MSPhaseDefinition(SUMOTime durationArg, SUMOTime minDurationArg, SUMOTime maxDurationArg,
                      const std::string &stateArg) throw()
            : duration(durationArg),
            myLastSwitch(0), state(stateArg) {
        myLastSwitch = OptionsCont::getOptions().getInt("begin"); // SUMOTime-option
        minDuration = minDurationArg<0 ? durationArg : minDurationArg;
        maxDuration = maxDurationArg<0 ? durationArg : maxDurationArg;
    }


    /// @brief Destructor
    virtual ~MSPhaseDefinition() throw() { }


    /** @brief Returns the state within this phase
     * @return The state in this phase
     */
    const std::string &getState() const throw() {
        return state;
    }


    /** @brief Returns whether this phase is a pure "green" phase
     *
     * "pure green" means in this case that at least one stream has green
     *  and no stream has yellow. Such phases are meant to be candidates
     *  for being stretched by actuated or agentbased traffic light logics.
     * @return Whether this phase is a "pure green" phase
     */
    bool isGreenPhase() const throw() {
        if (state.find_first_of("gG")==std::string::npos) {
            return false;
        }
        if (state.find_first_of("yY")!=std::string::npos) {
            return false;
        }
        return true;
    }


    /** @brief Returns the state of the tls signal at the given position
     * @param[in] pos The position of the signal to return the state for
     * @return The state of the signal at the given position
     */
    MSLink::LinkState getSignalState(unsigned int pos) const throw() {
        return (MSLink::LinkState) state[pos];
    }


    /** @brief Comparison operator
     *
     * Note that only the state must differ, not the duration!
     * @param[in] pd The phase definition to compare against
     * @return Whether the given phase definition differs
     */
    bool operator!=(const MSPhaseDefinition &pd) {
        return state!=pd.state;
    }


    /// @name Helper methods for converting between old and new representation
    /// @{

    /** @brief Helper method for converting old tls descriptions into new
     * @param[in] driveMask Information which vehicles (links) may drive
     * @param[in] brakeMask Information which vehicles (links) have to brake
     * @param[in] yellowMask Information which vehicles (links) have yellow
     * @return The new phase definition
     */
    static std::string old2new(const std::string &driveMask, const std::string &brakeMask, const std::string &yellowMask) throw() {
        std::string state;
        for (int i=(int) driveMask.length()-1; i>=0; --i) {
            if (driveMask[i]=='1') {
                state += 'g';
            } else {
                if (yellowMask[i]=='1') {
                    state += 'y';
                } else {
                    state += 'r';
                }
            }
        }
        //  brake needs then
        int j = 0;
        for (int i=(int) driveMask.length()-1; i>=0; --i, ++j) {
            if (brakeMask[i]=='0') {
                if (state[j]=='g') {
                    state[j] = 'G';
                }
                if (state[j]=='y') {
                    state[j] = 'Y';
                }
            }
        }
        return state;
    }


    /** @brief Helper method for extracting the old "driveMask" from new tls definitions
     * @param[in] state Definition of the phase
     * @return The driveMask
     */
    static std::string new2driveMask(const std::string &state) throw() {
        std::string mask;
        for (int i=(int) state.length()-1; i>=0; --i) {
            if (state[i]=='g'||state[i]=='G') {
                mask += '1';
            } else {
                mask += '0';
            }
        }
        return mask;
    }


    /** @brief Helper method for extracting the old "brakeMask" from new tls definitions
     * @param[in] state Definition of the phase
     * @return The brakeMask
     */
    static std::string new2brakeMask(const std::string &state) throw() {
        std::string mask;
        for (int i=(int) state.length()-1; i>=0; --i) {
            if (state[i]>='a'&&state[i]<='z') {
                mask += '0';
            } else {
                mask += '1';
            }
        }
        return mask;
    }


    /** @brief Helper method for extracting the old "yellowMask" from new tls definitions
     * @param[in] state Definition of the phase
     * @return The yellowMask
     */
    static std::string new2yellowMask(const std::string &state) throw() {
        std::string mask;
        for (int i=(int) state.length()-1; i>=0; --i) {
            if (state[i]=='y'||state[i]=='Y') {
                mask += '1';
            } else {
                mask += '0';
            }
        }
        return mask;
    }

    /// @}


private:
    /// @brief The phase definition
    std::string state;


};

#endif

/****************************************************************************/

