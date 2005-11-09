#ifndef MSEmitter_h
#define MSEmitter_h
//---------------------------------------------------------------------------//
//                        MSEmitter.h -
//  Class that realises the setting of a lane's maximum speed triggered by
//      values read from a file
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 21.07.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.1  2005/11/09 06:35:03  dkrajzew
// Emitters reworked
//
// Revision 1.4  2005/10/06 13:39:21  dksumo
// using of a configuration file rechecked
//
// Revision 1.3  2005/09/20 06:11:17  dksumo
// floats and doubles replaced by SUMOReal; warnings removed
//
// Revision 1.2  2005/09/09 12:51:25  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.1  2005/08/01 13:31:00  dksumo
// triggers reworked and new added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <utils/helpers/Command.h>
#include "MSTriggeredXMLReader.h"
#include "MSTrigger.h"
#include <utils/helpers/RandomDistributor.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;
class MSLane;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * MSEmitter
 * This trigger reads the next maximum velocity of the lane he is
 * responsible for from a file and sets it.
 * Lanes with variable speeds are so possible.
 */
class MSEmitter : public MSTrigger {
public:
    /** constructor for file-based emission */
    MSEmitter(const std::string &id, MSNet &net, MSLane* destLane,
        SUMOReal pos, const std::string &aXMLFilename);

    /** destructor */
    virtual ~MSEmitter();


public:
    class MSEmitterChild {
    public:
        MSEmitterChild(MSEmitter &parent)
            : myParent(parent), myTimeOffset(0) { }

        virtual ~MSEmitterChild() { }

        MSRoute *getRndRoute() const { return myRouteDist.get(); }

        MSVehicleType *getRndVType() const { return myVTypeDist.get(); }

        bool hasRoutes() const { return myRouteDist.getOverallProb()!=0; }

        bool hasVTypes() const { return myVTypeDist.getOverallProb()!=0; }

        SUMOReal computeOffset(SUMOReal flow) const {
            SUMOReal freq = (SUMOReal) (1. / (flow / 3600.));
            SUMOReal ret = freq;
            myTimeOffset += (freq - (SUMOTime) ret);
            if(myTimeOffset>1) {
                myTimeOffset -= 1;
                ret += 1;
            }
            if(ret==0) { // !!! check what to do in this case (more than one vehicle/s)
                ret = 1;
            }
            return ret;
        }

    protected:
        RandomDistributor<MSRoute*> myRouteDist;
        RandomDistributor<MSVehicleType*> myVTypeDist;
        MSEmitter &myParent;
        mutable SUMOReal myTimeOffset;
    };

public:
    bool childCheckEmit(MSEmitterChild *child);
    size_t getActiveChildIndex() const;
    void setActiveChild(MSEmitterChild *child);


protected:
    class MSEmitter_FileTriggeredChild
        : public MSTriggeredXMLReader, public MSEmitterChild, public Command {
    public:
        MSEmitter_FileTriggeredChild(MSNet &net,
            const std::string &aXMLFilename, MSEmitter &parent);

        ~MSEmitter_FileTriggeredChild();

        /** the implementation of the MSTriggeredReader-processNextEntryReaderTriggered method */
        bool processNextEntryReaderTriggered();

        SUMOTime execute();

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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

