/****************************************************************************/
/// @file    MSMessageEmitter.h
/// @author  Clemens Honomichl
/// @date    Tue, 26 Feb 2008
/// @version $Id$
///
// Builds detectors for microsim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSMessageEmitter_h
#define MSMessageEmitter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef _MESSAGES

#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSMessageEmitter
 * @brief Builds detectors for microsim
 *
 * The building methods may be overridden, to build guisim-instances of the triggers,
 *  for example.
 */
class MSMessageEmitter {
public:
    MSMessageEmitter(std::string& file, const std::string& base, std::string& whatemit,
                     bool reverse = false, bool tableOut = true, bool xy = false, SUMOReal step = 1);

    virtual ~MSMessageEmitter();

    void writeLaneChangeEvent(const std::string& id, SUMOReal& timeStep, MSLane *oldlane, SUMOReal myPos,
                              SUMOReal mySpeed, MSLane *newlane, SUMOReal x, SUMOReal y);

    void writeBreakEvent(const std::string& id, SUMOReal& timeStep, MSLane *lane, SUMOReal myPos,
                         SUMOReal speed, SUMOReal x, SUMOReal y);

    void writeHeartBeatEvent(const std::string &id, SUMOReal& timeStep, MSLane* lane, SUMOReal myPos,
                             SUMOReal speed, SUMOReal x, SUMOReal y);

    bool getWriteLCEvent();

    bool getWriteBEvent();

    bool getWriteHBEvent();

    bool getEventsEnabled(const std::string& enabled);

private:
    // methods
    std::string trimmed(const std::string& str, const char* sepSet = " \t\n\r");

    void setWriteEvents(std::string &events);

    void setFile(const std::string& file);

    void initXML();
    // variables
    OutputDevice &myDev;

    bool writeLCEvent;
    bool writeBEvent;
    bool writeHBEvent;
    bool reverseOrder;
    bool tableOutput;
    bool xyCoords;
    SUMOReal myStep;
};
#endif //_MESSAGES

#endif
