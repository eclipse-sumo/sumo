/****************************************************************************/
/// @file    MSPersonControl.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// 	»missingDescription«
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
#ifndef MSPersonControl_h
#define MSPersonControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <microsim/MSNet.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSPerson;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 * @class MSPersonControl
 * The class is used to handle persons who are not using a transportation
 *  system but are walking or waiting. Both is processed by waiting
 *  for the arrival time / the time the waiting is over.
 */
class MSPersonControl {
public:

    typedef std::vector<MSPerson*> PersonVector;

    /// constructor
    MSPersonControl();

    /// destructor
    ~MSPersonControl();

    /// adds a single person to the list of walking persons
    void add(SUMOTime now, MSPerson *person);

    ///
    bool hasWaitingPersons(SUMOTime time) const;

    /// returns the list of persons which waiting or walking period is over
    const PersonVector &getWaitingPersons(SUMOTime time) const;

    /**
     * The class holds the list of persons which all arrive/stop waiting at
     *  the specified time
     */
    class SameDepartureTimeCont {
    public:
        /// constructor
        SameDepartureTimeCont(SUMOTime time);

        /// destructor
        ~SameDepartureTimeCont();

        /// adds a person to the list of walking/waiting persons
        void add(MSPerson *person);

        /// extends the list of walking/waiting persons by the given list of persons
        void add(const std::vector<MSPerson*> &cont);

        /// returns the arrival time
        SUMOTime getTime() const;

        /// returns the container of persons walking/waiting
        const std::vector<MSPerson*> &getPersons() const;

    private:
        /// the time of the arrival at the destination / the end of the waiting
        SUMOTime m_uiArrivalTime;
        /// the list of persons walking/waiting
        std::vector<MSPerson*> m_pPersons;
    };

private:
    /**
     * The class that performs the checking whether the current item in a search
     * is smaller or equal to a time
     */
    class my_greater : public std::unary_function<SameDepartureTimeCont, bool> {
    public:
        my_greater(SUMOTime value) : m_value(value) {}

        bool operator()(SameDepartureTimeCont arg) const {
            return m_value > arg.getTime();
        }

    private:
        SUMOTime m_value;

    };

private:
    /**
     * The class that performs the checking whether the current item in a search
     * is equal to a time
     */
    class equal : public std::unary_function<SameDepartureTimeCont, bool> {
    public:
        equal(SUMOTime value) : m_value(value) {}
        bool operator()(SameDepartureTimeCont arg) const {
            return m_value == arg.getTime();
        }
    private:
        SUMOTime m_value;
    };

public:
    typedef std::vector<SameDepartureTimeCont> WaitingPersons;

private:
    /// the list of waiting persons
    std::vector<SameDepartureTimeCont> myWaiting;

    /// the list of walking persons
    std::vector<SameDepartureTimeCont> myWalking;

private:
    /// adds the list of persons walking until the specified time
    void push(SUMOTime time, const std::vector<MSPerson*> &list);

    /// adds the timedlist of persons walking until the specified time
    void push(const SameDepartureTimeCont &cont);

};


#endif

/****************************************************************************/

