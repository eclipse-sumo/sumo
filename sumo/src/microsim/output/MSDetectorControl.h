#ifndef MSDetectorControl_h
#define MSDetectorControl_h

/**
 * @file   MSDetectorControl.h
 * @author Daniel Krajzewicz
 * @date   Tue Jul 29 10:41:01 2003
 * @version $Id$
 * @brief  Declaration of class MSDetectorControl
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

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
#include <utils/helpers/NamedObjectCont.h>
#include <microsim/output/e2_detectors/MSE2Collector.h>
#include <microsim/output/e2_detectors/MS_E2_ZS_CollectorOverLanes.h>
//#include <microsim/output/e3_detectors/MSE3Collector.h>
#include <microsim/MSLaneState.h>
#include <microsim/output/MSInductLoop.h>
#include "meandata/MSMeanData_Net_Cont.h"
#include "MSDetector2File.h"



//class MSE2Collector;
//class MS_E2_ZS_CollectorOverLanes;
class MSE3Collector;
//class MSLaneState;
//class MSInductLoop;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSDetectorControl {
public:
    typedef NamedObjectCont< MSLaneState*> LaneStateDict;
    typedef NamedObjectCont< MSInductLoop*> LoopDict;
    typedef NamedObjectCont< MSE2Collector*> E2Dict;
    typedef NamedObjectCont< MSE3Collector*> E3Dict;
    typedef NamedObjectCont< MS_E2_ZS_CollectorOverLanes* > E2ZSOLDict;

    typedef std::vector< MSLaneState*> LaneStateVect;
    typedef std::vector< MSInductLoop*> LoopVect;
    typedef std::vector< MSE2Collector*> E2Vect;
    typedef std::vector< MSE3Collector*> E3Vect;
    typedef std::vector< MS_E2_ZS_CollectorOverLanes* > E2ZSOLVect;

public:
    MSDetectorControl();

    ~MSDetectorControl();

    MSInductLoop *findInductLoop(const std::string &id);

    void add(MSInductLoop *il, OutputDevice *device,
        int splInterval);

    void add(MSE2Collector *e2, OutputDevice *device,
        int splInterval);

    void add(MS_E2_ZS_CollectorOverLanes *e2ol, OutputDevice *device,
        int splInterval);

    /// adds an e2 detector coupled to an extern output impulse giver
    void add(MSE2Collector *e2);

    /// adds an e2ol detector coupled to an extern output impulse giver
    void add(MS_E2_ZS_CollectorOverLanes *e2ol);

    void add(MSE3Collector *e3, OutputDevice *device,
        int splInterval);

    size_t getMeanDataSize() const;
    void addMeanData(MSMeanData_Net *newMeanData);

    MSDetector2File &getDet2File();

    void resetInterval(MSDetectorFileOutput *il, SUMOTime interval);

    LaneStateVect getLaneStateVector() const;
    LoopVect getLoopVector() const;
    E2Vect getE2Vector() const;
    E3Vect getE3Vector() const;
    E2ZSOLVect getE2OLVector() const;


protected:
    LaneStateDict myLaneStates;
    LoopDict myLoops;
    E2Dict myE2Detectors;
    E3Dict myE3Detectors;
    E2ZSOLDict myE2OverLanesDetectors;
    MSMeanData_Net_Cont myMeanData;
    MSDetector2File myDetector2File;


};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
