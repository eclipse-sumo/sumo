/****************************************************************************/
/// @file    MSEmitter.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 21.07.2005
/// @version $Id$
///
// Class that realises the setting of a lane's maximum speed triggered by
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

#include <string>
#include <vector>
#include <utils/helpers/Command.h>
#include "MSTriggeredXMLReader.h"
#include "MSTrigger.h"
#include <utils/helpers/RandomDistributor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * MSEmitter
 * This trigger reads the next maximum velocity of the lane he is
 * responsible for from a file and sets it.
 * Lanes with variable speeds are so possible.
 */
class MSEmitter : public MSTrigger
{
public:
    /** constructor for file-based emission */
    MSEmitter(const std::string &id, MSNet &net, MSLane* destLane,
              SUMOReal pos, const std::string &aXMLFilename);

    /** destructor */
    virtual ~MSEmitter();


public:
    class MSEmitterChild
    {
    public:
        MSEmitterChild(MSEmitter &parent, MSVehicleControl &vc)
                : myParent(parent), myVehicleControl(vc), myTimeOffset(0)
        { }

        virtual ~MSEmitterChild()
        { }

        MSRoute *getRndRoute() const
        {
            return myRouteDist.get();
        }

        const std::vector<MSRoute*> &getAllRoutes() const
        {
            return myRouteDist.getVals();
        }

        MSVehicleType *getRndVType() const
        {
            return myVTypeDist.get();
        }

        bool hasRoutes() const
        {
            return myRouteDist.getOverallProb()!=0;
        }

        bool hasVTypes() const
        {
            return myVTypeDist.getOverallProb()!=0;
        }

        SUMOReal computeOffset(SUMOReal flow) const
        {
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

        RandomDistributor<MSRoute*> &getRouteDist()
        {
            return myRouteDist;
        }

    protected:
        MSEmitter &myParent;
        MSVehicleControl &myVehicleControl;
        RandomDistributor<MSRoute*> myRouteDist;
        RandomDistributor<MSVehicleType*> myVTypeDist;
        mutable SUMOReal myTimeOffset;
    };

public:
    bool childCheckEmit(MSEmitterChild *child);
    size_t getActiveChildIndex() const;
    void setActiveChild(MSEmitterChild *child);


protected:
    class MSEmitter_FileTriggeredChild
                : public MSTriggeredXMLReader, public MSEmitterChild, public Command
    {
    public:
        MSEmitter_FileTriggeredChild(MSNet &net,
                                     const std::string &aXMLFilename, MSEmitter &parent, MSVehicleControl &vc);

        ~MSEmitter_FileTriggeredChild();

        /** the implementation of the MSTriggeredReader-processNextEntryReaderTriggered method */
        bool processNextEntryReaderTriggered();

        SUMOTime execute(SUMOTime currentTime);

        SUMOReal getLoadedFlow() const;

    protected:
        /** the implementation of the SAX-handler interface for reading
            element begins */
        virtual void myStartElement(int element, const std::string &name,
                                    const Attributes &attrs);

        /** the implementation of the SAX-handler interface for reading
            characters */
        void myCharacters(int element, const std::string &name,
                          const std::string &chars);

        /** the implementation of the SAX-handler interface for reading
            element ends */
        void myEndElement(int element, const std::string &name);

        void inputEndReached();

        bool nextRead();

        void buildAndScheduleFlowVehicle();

    protected:
        bool myHaveNext;
        SUMOReal myFlow;
        bool myHaveInitialisedFlow;
        int myRunningID;
        SUMOTime myBeginTime;
    };

public:
    void schedule(MSEmitterChild *child, MSVehicle *v, SUMOReal speed);


protected:
    MSNet &myNet;
    /** the lane the trigger is responsible for */
    MSLane *myDestLane;
    SUMOReal myPos;
    MSEmitterChild *myFileBasedEmitter;
    std::map<MSEmitterChild*, std::pair<MSVehicle*, SUMOReal> > myToEmit;
    MSEmitterChild *myActiveChild;

};


#endif

/****************************************************************************/

