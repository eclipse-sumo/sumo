/****************************************************************************/
/// @file    MSEmitter.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 21.07.2005
/// @version $Id$
///
// A vehicle emitting device
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
#ifndef MSEmitter_h
#define MSEmitter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/common/Command.h>
#include "MSTriggeredXMLReader.h"
#include "MSTrigger.h"
#include <utils/common/RandomDistributor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSEmitter
 * @brief A vehicle emitting device
 *
 * Instances of this class are places at a lane where vehicles are inserted into
 *  the network. This mechanism can be used in parallel to the standard vehicle
 *  emission on roads. An emitter is meant to be used to simulate the entry of a
 *  flow into an area at places where induction loops are located at in reality.
 */
class MSEmitter : public MSTrigger
{
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the emitter
     * @param[in] net The net the emitter belongs to
     * @param[in] destLane The lane the emitter is placed on
     * @param[in] pos Position of the emitter on the given lane
     * @param[in] file Name of the file to read the emission definitions from
     */
    MSEmitter(const std::string &id, MSNet &net, MSLane* destLane,
              SUMOReal pos, const std::string &file) throw();

    /** destructor */
    virtual ~MSEmitter() throw();


public:
    /**
     * @class MSEmitterChild
     * @brief Holder of information about vehicle routes and types
     *
     * Classes which realise different schemes to emit vehicles (file-based,
     *  based on user entries on the gui, etc.) should subclass this class
     */
    class MSEmitterChild
    {
    public:
        /// Constructor
        MSEmitterChild(MSEmitter &parent, MSVehicleControl &vc)
                : myParent(parent), myVehicleControl(vc), myTimeOffset(0) { }

        /// Destructor
        virtual ~MSEmitterChild() { }

        /// Returns a random route
        MSRoute *getRndRoute() const {
            return myRouteDist.get();
        }

        /// Returns a list of all available routes
        const std::vector<MSRoute*> &getAllRoutes() const {
            return myRouteDist.getVals();
        }

        /// Returns a random type
        MSVehicleType *getRndVType() const {
            return myVTypeDist.get();
        }

        /// Returns the information whether any route is stored
        bool hasRoutes() const {
            return myRouteDist.getOverallProb()!=0;
        }

        /// Returns the information whether any vehicle type is stored
        bool hasVTypes() const {
            return myVTypeDist.getOverallProb()!=0;
        }

        /// Returns the time offset till the next vehicle emission for a given flow
        SUMOReal computeOffset(SUMOReal flow) const {
            SUMOReal freq = (SUMOReal)(1. / (flow / 3600.));
            SUMOReal ret = freq;
            myTimeOffset += (freq - (SUMOTime) ret);
            if (myTimeOffset>1) {
                myTimeOffset -= 1;
                ret += 1;
            }
            if (ret==0) { // !!! check what to do in this case (more than one vehicle/s)
                ret = 1;
            }
            return ret;
        }

        /// Returns the routes probability
        RandomDistributor<MSRoute*> &getRouteDist() {
            return myRouteDist;
        }

    protected:
        /// The emitter this instance is child of
        MSEmitter &myParent;

        /// The vehicle control to use for vehicle building
        MSVehicleControl &myVehicleControl;

        /// The used route probability distribution
        RandomDistributor<MSRoute*> myRouteDist;

        /// The used vehicle type probability distribution
        RandomDistributor<MSVehicleType*> myVTypeDist;

        /// The time offset till next emission
        mutable SUMOReal myTimeOffset;

    };

public:
    /** Called by a MSEmitterChild (given as parameter) this method checks whether
     * this child is the currently active one and whether a vehicle from this source
     * can be emitted.
     * Returns true if the child can continue with the next vehicle, false otherwise */
    bool childCheckEmit(MSEmitterChild *child);

    /// Returns the index of the currently active child
    size_t getActiveChildIndex() const;

    /// Sets the given child as the currently used one
    void setActiveChild(MSEmitterChild *child);


protected:
    /**
     * @class MSEmitter_FileTriggeredChild
     * @brief An EmitterChild which uses information from a parsed file
     */
    class MSEmitter_FileTriggeredChild
                : public MSTriggeredXMLReader, public MSEmitterChild, public Command
    {
    public:
        /// Constructor
        MSEmitter_FileTriggeredChild(MSNet &net,
                                     const std::string &aXMLFilename, MSEmitter &parent, MSVehicleControl &vc) throw();

        /// Destructor
        ~MSEmitter_FileTriggeredChild() throw();

        /** the implementation of the MSTriggeredReader-processNextEntryReaderTriggered method */
        bool processNextEntryReaderTriggered();


        /// @name Derived from Command
        /// @{

        /** @brief Prcesses the last loaded vehicle
         *
         * @param[in] currentTime The current simulation time
         * @return The time after which the command shall be executed again
         * @exception ProcessError 
         * @todo Recheck, describe!!!
         */
        SUMOTime execute(SUMOTime currentTime) throw(ProcessError);
        /// @}


        /// Returns the loaded flow
        SUMOReal getLoadedFlow() const;

    protected:
        /// @name inherited from GenericSAXHandler
        //@{

        /** @brief Called on the opening of a tag;
         *
         * @param[in] element ID of the currently opened element
         * @param[in] attrs Attributes within the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myStartElement
         */
        virtual void myStartElement(SumoXMLTag element,
                                    const SUMOSAXAttributes &attrs) throw(ProcessError);
        //@}

        /// Informs the child that the end of the processed file has been reached
        void inputEndReached();

        /// Returns the information whether a next valid entry has been reached
        // !!! seee
        bool nextRead();

        /// Builds a vehicle and schedules it for further processing
        void buildAndScheduleFlowVehicle();

    protected:
        /// Information whether a further vehicle to emit exists
        bool myHaveNext;

        /// The last loaded flow
        SUMOReal myFlow;

        /// Information whether the flow is used
        bool myHaveInitialisedFlow;

        /// A running vehicle id
        int myRunningID;

        /// Information about the simulation time the processing starts at; previous vehicles are discarded
        SUMOTime myBeginTime;

    };

public:
    /// Schedules a vehicle to emit
    void schedule(MSEmitterChild *child, MSVehicle *v, SUMOReal speed);

protected:
    /// The network the emitter is located in
    MSNet &myNet;

    /** the lane the emitter is placed on */
    MSLane *myDestLane;

    /// The position of the emitter at the lane
    SUMOReal myPos;

    /// The file-based child
    MSEmitterChild *myFileBasedEmitter;

    /// A map children->vehicles to emit
    std::map<MSEmitterChild*, std::pair<MSVehicle*, SUMOReal> > myToEmit;

    /// The currently active child
    MSEmitterChild *myActiveChild;

};


#endif

/****************************************************************************/

