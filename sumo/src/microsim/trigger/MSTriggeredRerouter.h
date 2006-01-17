#ifndef MSTriggeredRerouter_h
#define MSTriggeredRerouter_h
//---------------------------------------------------------------------------//
//                        MSTriggeredRerouter.h -
//  Allows the triggered rerouting of vehicles within the simulation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 25 July 2005
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
// Revision 1.4  2006/01/17 14:10:56  dkrajzew
// debugging
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
#include <microsim/MSMoveReminder.h>
#include "MSTriggeredXMLReader.h"
#include "MSTrigger.h"
#include <utils/sumoxml/SUMOSAXHandler.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;
class MSLane;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSTriggeredRerouter
 */
class MSTriggeredRerouter : public MSTrigger, public SUMOSAXHandler {
public:
    /** constructor */
    MSTriggeredRerouter(const std::string &id,
        MSNet &net, const std::vector<MSEdge*> &edges,
        SUMOReal prob, const std::string &aXMLFilename);

    /** destructor */
    virtual ~MSTriggeredRerouter();

    class Setter : public MSMoveReminder {
    public:
        Setter(MSTriggeredRerouter *parent, MSLane *lane,
            const std::string &id);

        ~Setter();

        bool isStillActive( MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos,
            SUMOReal newSpeed );

        void dismissByLaneChange( MSVehicle& veh );

        bool isActivatedByEmitOrLaneChange( MSVehicle& veh );

    private:
        MSTriggeredRerouter *myParent;
    };

    struct RerouteInterval {
        SUMOTime begin;
        SUMOTime end;
        std::vector<MSEdge*> closed;
        std::vector<MSEdge*> dests;
        std::vector<std::pair<SUMOReal, MSEdge*> > prob;
    };

    void reroute(MSVehicle &veh, const MSEdge *src);

    bool hasCurrentReroute(SUMOTime time, MSVehicle &veh) const;

    const RerouteInterval &getCurrentReroute(SUMOTime time, MSVehicle &veh) const;

    bool hasCurrentReroute(SUMOTime time) const;

    const RerouteInterval &getCurrentReroute(SUMOTime time) const;

    void setUserMode(bool val);
    void setUserUsageProbability(SUMOReal prob);
    bool inUserMode() const;
    SUMOReal getProbability() const;
    SUMOReal getUserProbability() const;

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
    std::vector<Setter*> mySetter;
    std::vector<RerouteInterval> myIntervals;
    SUMOTime myCurrentIntervalBegin, myCurrentIntervalEnd;
    std::vector<MSEdge*> myCurrentClosed;
    std::vector<MSEdge*> myCurrentDests;
    std::vector<std::pair<SUMOReal, MSEdge*> > myCurrentProb;
    SUMOReal myProbability, myUserProbability;
    bool myAmInUserMode;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

