/****************************************************************************/
/// @file    METriggeredCalibrator.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: METriggeredCalibrator.h 111 2007-06-14 13:00:14Z behr_mi $
///
// Calibrates the flow on a segment to a specified one
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
#ifndef METriggeredCalibrator_h
#define METriggeredCalibrator_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_MESOSIM

#include <string>
#include <vector>
#include <microsim/MSMoveReminder.h>
#include <microsim/trigger/MSTriggeredXMLReader.h>
#include <microsim/trigger/MSTrigger.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/helpers/RandomDistributor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;
class MSRoute;
class MESegment;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class METriggeredCalibrator
 * @brief Calibrates the flow on a segment to a specified one
 */
class METriggeredCalibrator :
            public MSTrigger,
            public SUMOSAXHandler
{
public:
    /** constructor */
    METriggeredCalibrator(const std::string &id,
                          MSNet &net, MESegment *edge,
                          const std::string &routesFile,
                          const std::string &aXMLFilename);

    /** destructor */
    virtual ~METriggeredCalibrator() throw();


    SUMOTime execute(SUMOTime currentTime);

    struct ShouldBe
    {
        SUMOTime begin;
        SUMOTime end;
        SUMOReal q;
        SUMOReal v;
        mutable bool beenDone;
    };

    struct VehTypeDist
    {
        SUMOTime begin;
        SUMOTime end;
        RandomDistributor<MSVehicleType*> typeDist;
    };

    struct RouteDist
    {
        SUMOTime begin;
        SUMOTime end;
        RandomDistributor<MSRoute*> routeDist;
    };

    bool hasCurrentShouldBe(SUMOTime time) const;

    const ShouldBe &getCurrentShouldBe(SUMOTime time) const;

protected:
    /** the implementation of the SAX-handler interface for reading
        element begins */
    virtual void myStartElement(SumoXMLTag element,
                                const Attributes &attrs) throw(ProcessError);

    /** the implementation of the SAX-handler interface for reading
        characters */
    void myCharacters(SumoXMLTag element,
                      const std::string &chars) throw(ProcessError);

    /** the implementation of the SAX-handler interface for reading
        element ends */
    void myEndElement(SumoXMLTag element) throw(ProcessError);

    bool nextRead();

    MSRoute *getRandomRoute(const ShouldBe &sb);

    MSVehicleType *getRandomVehicleType(const ShouldBe &sb);

protected:

    /// The used SAX-parser
    SAX2XMLReader* myParser;
    MESegment *mySegment;

    /// the current route
    MSEdgeVector    *m_pActiveRoute;

    /// the id of the current route
    std::string     m_ActiveId;

    // during run
    size_t myRunningID;
    std::vector<ShouldBe> myIntervals;
    std::vector<VehTypeDist> myVehicleTypes;
    std::vector<RouteDist> myRoutes;
    SUMOReal myAggregatedPassedQ;
    SUMOReal myWishedQ;
    SUMOReal myWishedV;
    SUMOReal myWishedDensity;
    SUMOReal myDefaultSpeed;

    // during parsing
    RandomDistributor<MSRoute*> myCurrentRouteDist;
    RandomDistributor<MSVehicleType*> myCurrentVTypeDist;
    SUMOTime myCurrentIntervalBegin, myCurrentIntervalEnd;

    std::vector<MSVehicle*> myCarPool;

    RandomDistributor<MSRoute*> myGlobalRoutes;

    int blaDelay;

};

#endif // HAVE_MESOSIM

#endif

/****************************************************************************/
