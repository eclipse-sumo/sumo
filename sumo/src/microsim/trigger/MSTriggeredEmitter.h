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
#include <config.h>
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
class MSTriggeredEmitter : public MSTriggeredXMLReader,
                           public MSTrigger {
public:
    /** constructor */
    MSTriggeredEmitter(const std::string &id,
        MSNet &net, MSLane* destLane, SUMOReal pos,
        const std::string &aXMLFilename);

    /** destructor */
    virtual ~MSTriggeredEmitter();

    /** the implementation of the MSTriggeredReader-processNext method */
    bool processNext();


protected:
    /**
     * Event type to trigger the execution of the derived strctures
     */
    class UserCommand : public Command
    {
    public:
        /// Constructor
        UserCommand(MSTriggeredEmitter &parent);

        /// virtual destructor
        virtual ~UserCommand( void );

        /** Execute the command and return an offset for recurring commands
            or 0 for single-execution commands. */
        virtual SUMOTime execute();

    private:
        /// The parent reader
        MSTriggeredEmitter &_parent;
    };

public:
    void setUserMode(bool val);
    void setUserFlow(SUMOReal factor);
    int userEmit();
    bool wantsMe(UserCommand *us);
    SUMOReal getFrequency() const;
    bool inUserMode() const;
    SUMOReal getUserFlow() const;

/*
    void setOverriding(bool val);
/*
    void setOverridingValue(SUMOReal val);
*/
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

private:   // private members
    /**

    */
    /*
    class RouteDistribution
    {
    public:
        /// Default constructor.
        RouteDistribution( void );

        /// Destructor.
        ~RouteDistribution( void );
        void addElement( MSRoute* aRoute,
                         SUMOReal aFrequency );
		MSRoute* getRndRoute( void );
        unsigned getSize( void ) const;
    protected:
    private:
        /// Copy constructor.
        RouteDistribution( const RouteDistribution& );

        /// Assignment operator.
        RouteDistribution& operator=( RouteDistribution& );
    protected:
    private:
        /
         /
        struct RouteDistElement
        {
            /// Pointer to route. Shouldn't be 0.
            MSRoute* myRoute;

            /** Route-pointers frequency.
                Frequency with which the vehicles will get the
                corresponding route-pointer. Is not bound to a maximum
                value, but should be > 0.
            /
            SUMOReal myFrequency;

            SUMOReal myFrequencySum;
        };

        typedef std::vector< RouteDistElement > RouteDistCont;
        RouteDistCont myDistribution;

        /**
         /
        struct FrequencyGreater :
            public std::binary_function< RouteDistElement,
                                         SUMOReal, bool >
        {
            bool operator() ( RouteDistElement elem,
                              SUMOReal cmpFreq ) const;
        };

    }; // end class RouteDistribution
    */

    RandomDistributor<MSRoute*> myRouteDist;
    MSVehicle* myVehicle;

protected:
    /** the lane the trigger is responsible for */
    MSLane *myDestLane;

    bool myHaveNext;

    /// The information whether the read speed shall be overridden
    bool myAmOverriding;
    bool myIsNewEmitFound;
    SUMOReal myEmitSpeed;
    SUMOReal myPos;
    bool myUserMode;
    SUMOReal myUserFlow;
    SUMOTime myLastUserEmit;
    //int myNoUserEvents;
    RandomDistributor<MSVehicleType*> myCurrentVTypeDist;
    SUMOReal myFlow;
    std::vector<UserCommand*> mySentCommands;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

