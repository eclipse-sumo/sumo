/****************************************************************************/
/// @file    MSTLLogicControl.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A class that holds all traffic light logics used
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSTLLogicControl_h
#define MSTLLogicControl_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include "MSTrafficLightLogic.h"
#include "MSSimpleTrafficLightLogic.h"
#include <utils/helpers/Command.h>


// ===========================================================================
// type definitions
// ===========================================================================
/** @brief Definition of a phase description
    Within the first bitsets, all links having gree are marked, within the second
    all links having yellow */
typedef std::pair<std::bitset<64>, std::bitset<64> > SimplePhaseDef;

/// Definition of a complete phase information, including the time
typedef std::pair<SUMOTime, SimplePhaseDef> CompletePhaseDef;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTLLogicControl
 * @brief This class holds all traffic light logics (programs) and their
 *  variants during the simulation.
 *
 * When a WAUT is forced to switch, for each TLS, a switching procedure
 *  derived from WAUTSwitchProcedure is initialised and is asked repeatedly
 *  whether a switch could be done until it returns true.
 */
class MSTLLogicControl
{
public:
    /**
     * @struct TLSLogicVariants
     * @brief Storage for all programs of a single tls
     *
     * The currently used program is additionally stored.
     */
    struct TLSLogicVariants
    {
        /// The currently used program
        MSTrafficLightLogic *defaultTL;
        /// A map of subkeys to programs
        std::map<std::string, MSTrafficLightLogic*> ltVariants;
    };

    /// Constructor
    MSTLLogicControl();

    /// Destructor
    ~MSTLLogicControl();

    /** This method must be called after the network (including the initial tls
     * definitions) was loaded.
     */
    void markNetLoadingClosed();

    /// For all traffic lights, the requests are masked away if they have red light (not yellow)
    void maskRedLinks();

    /// For all traffic lights, the requests are masked away if they have yellow light
    void maskYellowLinks();

    /** @brief Returns a vector which contains all logics
     *
     * All logics are included, active (current) and non-current
     */
    std::vector<MSTrafficLightLogic*> getAllLogics() const;

    /// Returns the variants of a named tls
    const TLSLogicVariants &get(const std::string &id) const;

    /// Returns a single program (variant) defined by the tls id and the program subid
    MSTrafficLightLogic *get(const std::string &id, const std::string &subid) const; // !!! reference, const?

    /// Returns the active program of a named tls
    MSTrafficLightLogic *getActive(const std::string &id) const; // !!! reference, const?

    /** @brief Adds a tls program to the container
     *
     * The parameter newDefault defines whether this program will be used as the new
     *  default program of this tls.
     */
    bool add(const std::string &id, const std::string &subID,
                 MSTrafficLightLogic *logic, bool newDefault=true);

    /// Returns the information whether the named tls is stored
    bool knows(const std::string &id) const;

    /// Returns the information whether the given tls program is the currently active for his tls
    bool isActive(const MSTrafficLightLogic *tl) const;

    /** @brief Switches the named (id) tls to the named (subid) program
     *
     * The program with the used subid must be previously added.
     * If the tls itself or the program to switch to is not known, false is returned.
     */
    bool switchTo(const std::string &id, const std::string &subid);

    /** @brief Adds a WAUT to the control
     *
     * If a WAUT with the same ID has already been defined, false is returned
     */
    bool addWAUT(SUMOTime refTime, const std::string &id, const std::string &startProg);

    /** @brief Adds a timestep at which the named WAUT shall switch to another program
     *
     * If a WAUT with the given id does not exist, false is returned.
     */
    bool addWAUTSwitch(const std::string &wautid, SUMOTime when, const std::string &to);

    /** @brief Assigns the named junction to be controlled by the named WAUT
     *
     * If a WAUT with the given id does not exist, or a junction with the given id,
     *  false is returned.
     */
    bool addWAUTJunction(const std::string &wautid, const std::string &junc,
                         const std::string &proc, bool synchron);

    /** @brief Checks whether any WAUT is trying to switch a tls into another program
     *
     * Called from MSNet::simulationStep
     */
    void check2Switch(SUMOTime step);


    /** @brief return the complete phase definition for a named traffic lights logic
    */
    CompletePhaseDef getPhaseDef(const std::string &tlid) const;


protected:
    /**
     * @class SwitchInitCommand
     * @brief This event-class is used to initialise a WAUT switch at a certain time.
     *
     * This command is reused. The index of the WAUT-switch is incremented at each
     *  call to the control.
     */
class SwitchInitCommand : public Command
    {
    public:
        // / Constructor
        SwitchInitCommand(MSTLLogicControl &p, const std::string &wautid)
                : myParent(p), myWAUTID(wautid), myIndex(0)
        { }

        /// Destructor
        ~SwitchInitCommand()
        { }

        /** @brief Executes the command what forces the logic control to
         * initialise a switch process
         *
         * The control will ask for the index and increment it.
         */
        SUMOTime execute(SUMOTime)
        {
            return myParent.initWautSwitch(*this);
        }

        /// Returns the WAUT-id
        const std::string &getWAUTID() const
        {
            return myWAUTID;
        }

        /// Returns a reference to the index
        int &getIndex()
        {
            return myIndex;
        }

    protected:
        /// The control to call
        MSTLLogicControl &myParent;

        /// The id of the WAUT that shall switch
        std::string myWAUTID;

        /// The current index within the WAUT switch table
        int myIndex;

    };

public:
    /** @brief Initialises switching a WAUT
     *
     * This method is called from a previously built SwitchInitCommand
     */
    SUMOTime initWautSwitch(SwitchInitCommand &cmd);


protected:
    /**
     * @struct WAUTSwitch
     * @brief Storage for a WAUTs switch point
     */
    struct WAUTSwitch
    {
        /// The time the WAUT shall switch the TLS
        SUMOTime when;
        /// The program name the WAUT shall switch the TLS to
        std::string to;
    };

    /**
     * @struct WAUTJunction
     * @brief Storage for a junction assigned to a WAUT
     */
    struct WAUTJunction
    {
        /// The junction name
        std::string junction;
        /// The procedure to switch the junction with
        std::string procedure;
        /// Information whether this junction shall be switched synchron
        bool synchron;
    };

    /**
     * @struct WAUT
     * @brief A WAUT definition
     */
    struct WAUT
    {
        /// The id of the WAUT
        std::string id;
        /// The name of the start program
        std::string startProg;
        /// The reference time (offset to the switch times)
        SUMOTime refTime;
        /// The list of switches to be done by the WAUT
        std::vector<WAUTSwitch> switches;
        /// The list of switches assigned to the WAUT
        std::vector<WAUTJunction> junctions;
    };

    /**
     * @class WAUTSwitchProcedure
     * @brief This is the abstract base class for switching from one tls program to another.
     */
    class WAUTSwitchProcedure
    {
    public:
        /// Constructor
        WAUTSwitchProcedure(MSTLLogicControl &control, WAUT &waut,
                            MSTrafficLightLogic *from, MSTrafficLightLogic *to,
                            bool synchron)
                : myFrom(from), myTo(to), mySwitchSynchron(synchron), myWAUT(waut), myControl(control)
        { }

        /// Destructor
        virtual ~WAUTSwitchProcedure()
        { }

        /** @brief Determines whether a switch is possible.
         *
         * If a switch shall be done, this method should return true.
         */
        virtual bool trySwitch(SUMOTime step) = 0;

        // checks, if the position of a signal programm is at the GSP ("GünstigerUmschaltPunkt")
        bool isPosAtGSP(SUMOTime step, MSSimpleTrafficLightLogic *testLogic);

        /// Returns the difference between an given position and the start of the phase
        size_t getDiffToStartOfPhase(MSSimpleTrafficLightLogic *givenLogic, size_t pos);

        // switches the given Logic directly to the given position
        void switchToPos(SUMOTime simStep, MSSimpleTrafficLightLogic *givenLogic, size_t toPos);

    protected:
        /// The current program of the tls to switch
        MSTrafficLightLogic *myFrom;

        /// The program to switch the tls to
        MSTrafficLightLogic *myTo;

        /// Information whether to switch synchron (?)
        bool mySwitchSynchron;

        /// The WAUT responsible for switching
        WAUT &myWAUT;

        /// The control the logic belongs to
        MSTLLogicControl &myControl;

        /// Returns the GSP-value which should be within the tls program definition
        SUMOReal getGSPValue(MSTrafficLightLogic *from) const;

    };


    /**
     * @class WAUTSwitchProcedure_JustSwitch
     * @brief This class simply switches to the next program
     */
class WAUTSwitchProcedure_JustSwitch : public WAUTSwitchProcedure
    {
    public:
        /// Constructor
        WAUTSwitchProcedure_JustSwitch(MSTLLogicControl &control, WAUT &waut,
                                       MSTrafficLightLogic *from, MSTrafficLightLogic *to,
                                       bool synchron);

        /// Destructor
        ~WAUTSwitchProcedure_JustSwitch();

        /** @brief Determines whether a switch is possible.
         *
         * This implementation alsways returns true
         */
        bool trySwitch(SUMOTime step);

    };


    /**
     * @class WAUTSwitchProcedure_GSP
     * @brief This class switches using the GSP algorithm.
     */
class WAUTSwitchProcedure_GSP : public WAUTSwitchProcedure
    {
    public:
        /// Constructor
        WAUTSwitchProcedure_GSP(MSTLLogicControl &control, WAUT &waut,
                                MSTrafficLightLogic *from, MSTrafficLightLogic *to,
                                bool synchron);

        /// Destructor
        ~WAUTSwitchProcedure_GSP();

        /** @brief Determines whether a switch is possible.
         */
        bool trySwitch(SUMOTime step);

        void adaptLogic(SUMOTime step);

    protected:

    };


    /**
     * @class WAUTSwitchProcedure_Stretch
     * @brief This class switches using the GSP algorithm.
     */
class WAUTSwitchProcedure_Stretch : public WAUTSwitchProcedure
    {
    public:
        /// Constructor
        WAUTSwitchProcedure_Stretch(MSTLLogicControl &control, WAUT &waut,
                                    MSTrafficLightLogic *from, MSTrafficLightLogic *to,
                                    bool synchron);

        /// Destructor
        ~WAUTSwitchProcedure_Stretch();

        /** @brief Determines whether a switch is possible.
         */
        bool trySwitch(SUMOTime step);

        void adaptLogic(SUMOTime step, SUMOReal position);

        // cuts (stauchen) the Logic to synchronize,
        // returns false if cutting was imposible
        void cutLogic(SUMOTime step, size_t pos, size_t deltaToCut);

        // stretchs the logic to synchronize
        void stretchLogic(SUMOTime step, size_t startPos, size_t deltaToStretch);

    protected:
        /** @struct StretchBereichDef
         * @brief A definition of a stretch - Bereich
         */
        struct StretchBereichDef
        {
            /// The begin of a Bereich (?)
            SUMOReal begin;
            /// The end of a Bereich (?)
            SUMOReal end;
            /// The factor of a Bereich (?)
            SUMOReal fac;

        };

        /// Returns the number of given Stretch-Bereiche(?) for the given program
        int getStretchBereicheNo(MSTrafficLightLogic *from) const;

        /** @brief Returns the numbered Stretch-Bereich for the given program
         *
         * The first bereich has normally the number "1", not "0"!
         */
        StretchBereichDef getStretchBereichDef(MSTrafficLightLogic *from, int index) const;

    };


    /**
     * @struct WAUTSwitchProcess
     * @brief An initialised switch process
     */
    struct WAUTSwitchProcess
    {
        /// The id of the junction to switch
        std::string junction;
        /// The current program of the tls
        MSTrafficLightLogic *from;
        /// The program to switch the tls to
        MSTrafficLightLogic *to;
        /// The used procedure
        WAUTSwitchProcedure *proc;
    };

    /// A map of ids to corresponding WAUTs
    std::map<std::string, WAUT*> myWAUTs;

    /// A list of currently running switching procedures
    std::vector<WAUTSwitchProcess> myCurrentlySwitched;

    /// A map from ids to the corresponding variants
    std::map<std::string, TLSLogicVariants> myLogics;

    /// A list of active logics
    std::vector<MSTrafficLightLogic*> myActiveLogics;

    /// Information whether the net was completely loaded
    bool myNetWasLoaded;

};


#endif

/****************************************************************************/

