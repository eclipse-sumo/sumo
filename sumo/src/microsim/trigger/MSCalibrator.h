/****************************************************************************/
/// @file    MSCalibrator.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Tue, May 2005
/// @version $Id: MSCalibrator.h 2120 2012-10-09 14:09:08Z erdm_ja $
///
// Calibrates the flow on an edge by removing an inserting vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSCalibrator_h
#define MSCalibrator_h


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
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/Command.h>
#include <microsim/MSRouteHandler.h>
#include <microsim/output/MSMeanData_Net.h>
#include <microsim/trigger/MSTrigger.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSCalibrator
 * @brief Calibrates the flow on a segment to a specified one
 */
class MSCalibrator : public MSTrigger, public MSRouteHandler, public Command {
public:
    /** constructor */
    MSCalibrator(const std::string &id,
            MSEdge *edge, SUMOReal pos,
            const std::string &aXMLFilename,
            const std::string &outputFilename,
            const SUMOTime freq);

    /** destructor */
    virtual ~MSCalibrator();


    /** the implementation of the MSTrigger / Command interface.
        Calibrating takes place here. */
    SUMOTime execute(SUMOTime currentTime);

    /// @brief cleanup remaining data structures
    static void cleanup();

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
    virtual void myStartElement(int element,
                                const SUMOSAXAttributes &attrs);

    /** @brief Called on the closing of a tag;
     *
     * @param[in] element ID of the currently closed element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    virtual void myEndElement(int element);
    //@}

private:

    struct AspiredState {
        SUMOTime begin;
        SUMOTime end;
        SUMOReal q;
        SUMOReal v;
        SUMOVehicleParameter *vehicleParameter;
    };

    void writeXMLOutput();

    bool isCurrentStateActive(SUMOTime time);

    bool tryEmit(MSLane *lane, MSVehicle *vehicle);

    void init();

    inline int passed() const { 
        // calibrator measures at start of segment
        return myEdgeMeanData.nVehEntered + myEdgeMeanData.nVehDeparted - myEdgeMeanData.nVehVaporized;
    }

    /// @brief returns whether the segment is jammed although it should not be
    bool invalidJam() const;

    /// @brief returns the number of vehicles (of the current type) that still fit onto the segment
    int remainingVehicleCapacity() const;

    /// @brief returns the maximum number of vehicles that could enter from upstream until the calibrator is activated again
    inline int maximumInflow() const {
        return (int)std::ceil((SUMOReal)STEPS2TIME(myFrequency) * myEdge->getLanes().size());
    }

    /// @brief reset collected vehicle data
    void reset();

    /// @brief remove the last car on the calibrator edge
    bool removeLastCar();

private:
    /// @brief the edge on which this calibrator lies
    MSEdge *myEdge;
    /// @brief data collector for the calibrator
    std::vector<MSMeanData_Net::MSLaneMeanDataValues*> myLaneMeanData;
    /// @brief accumlated data for the whole edge
    MSMeanData_Net::MSLaneMeanDataValues myEdgeMeanData;
    /// @brief List of adaptation intervals
    std::vector<AspiredState> myIntervals;
    /// @brief Iterator pointing to the current interval
    std::vector<AspiredState>::const_iterator myCurrentStateInterval;

    /// @brief The device for xml statistics
    OutputDevice* myOutput;

    /// @brief The frequeny with which to check for calibration
    SUMOTime myFrequency;
    /// @brief The number of vehicles that were removed in the current interval
    unsigned int myRemoved;
    /// @brief The number of vehicles that were inserted in the current interval
    unsigned int myInserted;
    /// @brief The number of vehicles that were removed when clearin a jam
    unsigned int myClearedInJam;
    /// @brief The information whether the speed adaption has been reset
    bool mySpeedIsDefault;
    /// @brief The information whether speed was adapted in the current interval
    bool myDidSpeedAdaption;
    /// @brief The information whether init was called
    bool myDidInit;
    /// @brief The default (maximum) speed on the segment
    SUMOReal myDefaultSpeed;
    /// @brief The default (maximum) speed on the segment
    bool myHaveWarnedAboutClearingJam;

    /* @brief objects which need to live longer than the MSCalibrator
     * instance which created them */
    static std::vector<MSMoveReminder*> LeftoverReminders;
    static std::vector<SUMOVehicleParameter*> LeftoverVehicleParameters;

};

#endif

/****************************************************************************/
