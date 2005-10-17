#ifndef MSTriggeredEmitter_h
#define MSTriggeredEmitter_h
//---------------------------------------------------------------------------//
//                        MSTriggeredEmitter.h -
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
// Revision 1.4  2005/10/17 08:58:24  dkrajzew
// trigger rework#1
//
// Revision 1.3  2005/10/07 11:37:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.2  2005/09/22 13:45:52  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.1  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
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
 * MSTriggeredEmitter
 * This trigger reads the next maximum velocity of the lane he is
 * responsible for from a file and sets it.
 * Lanes with variable speeds are so possible.
 */
class MSTriggeredEmitter : public MSTrigger {
public:
    /** constructor for file-based emission */
    MSTriggeredEmitter(const std::string &id,
        MSNet &net, MSLane* destLane, SUMOReal pos,
        const std::string &aXMLFilename);

    /** destructor */
    virtual ~MSTriggeredEmitter();


public:
    class MSTriggeredEmitterChild {
    public:
        MSTriggeredEmitterChild(MSTriggeredEmitter &parent)
            : myParent(parent) { }
        MSTriggeredEmitterChild(MSTriggeredEmitter &parent,
            MSTriggeredEmitterChild &s)
            : myParent(parent), myRouteDist(s.myRouteDist),
            myCurrentVTypeDist(s.myCurrentVTypeDist) { }
        virtual ~MSTriggeredEmitterChild() { }

        MSRoute *getRndRoute() const { return myRouteDist.get(); }
        MSVehicleType *getRndVType() const { return myCurrentVTypeDist.get(); }
        bool hasRoutes() const { return myRouteDist.getOverallProb()!=0; }
        bool hasVTypes() const { return myCurrentVTypeDist.getOverallProb()!=0; }

    protected:
        RandomDistributor<MSRoute*> myRouteDist;
        RandomDistributor<MSVehicleType*> myCurrentVTypeDist;
        MSTriggeredEmitter &myParent;
    };

public:
    /** the implementation of the MSTriggeredReader-processNext method */
    bool childCheckEmit(MSTriggeredEmitterChild *child);
    size_t getActiveChildIndex() const;
    void setActiveChild(MSTriggeredEmitterChild *child);


protected:
    class MSTriggeredEmitter_FileTriggeredChild
        : public MSTriggeredXMLReader, public MSTriggeredEmitterChild/*, public Command*/ {
    public:
        MSTriggeredEmitter_FileTriggeredChild(MSNet &net,
            const std::string &aXMLFilename, MSTriggeredEmitter &parent);
        ~MSTriggeredEmitter_FileTriggeredChild();
        /** the implementation of the MSTriggeredReader-processNext method */
        bool processNext();
        //SUMOTime execute();

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

        bool nextRead();

    protected:
        bool myHaveNext;
    };

public:
    /*
    void setUserMode(bool val);
    void setUserFlow(SUMOReal factor);
    int userEmit();
//    bool wantsMe(UserCommand *us);
    SUMOReal getFrequency() const;
    bool inUserMode() const;
    SUMOReal getUserFlow() const;
    */
public:
    void schedule(MSTriggeredEmitterChild *child, MSVehicle *v, SUMOReal speed);


protected:
    MSNet &myNet;
    /** the lane the trigger is responsible for */
    MSLane *myDestLane;


    /// The information whether the read speed shall be overridden
    bool myAmOverriding;
    bool myIsNewEmitFound;
    SUMOReal myPos;
    bool myUserMode;
    SUMOReal myUserFlow;
    SUMOTime myLastUserEmit;
    //int myNoUserEvents;
//    std::vector<UserCommand*> mySentCommands;
    MSTriggeredEmitterChild *myFileBasedEmitter;
    //std::vector<MSTriggeredEmitterChild*> myChildren;
    std::map<MSTriggeredEmitterChild*, std::pair<MSVehicle*, SUMOReal> > myToEmit;
    MSTriggeredEmitterChild *myActiveChild;


};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

