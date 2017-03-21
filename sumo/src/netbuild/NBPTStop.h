/****************************************************************************/
/// @file    NBPtStop.h
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id: NBPtStopCont.h 23150 2017-02-27 12:08:30Z behrisch $
///
// The representation of a single pt stop
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMO_NBPTSTOP_H
#define SUMO_NBPTSTOP_H

#include <string>
#include <utils/geom/Position.h>


// ===========================================================================
// class definitions
// ===========================================================================

class OutputDevice;
/**
* @class NBPTStop
* @brief The representation of a single pt stop
*/
class NBPTStop {

public:
    /**@brief Constructor
    * @param[in] id The id of the pt stop
    * @param[in] position The position of the pt stop
    * @param[in] edgeId The edge id of the pt stop
    * @param[in] length The length of the pt stop
    */
    NBPTStop(std::string ptStopId, Position position, std::string edgeId, double length);
    std::string getID() const;

    const std::string getEdgeId();
    const Position& getPosition();
    void computExtent(double center, double d);
    void setLaneID(const std::string& laneId);
    void write(OutputDevice& device);
    void reshiftPostion(const double offsetX, const double offsetY);
private:
    const std::string myPTStopId;
    Position myPosition;
    const std::string myEdgeId;
    const double myPTStopLength;
    std::string myLaneId;
    double myFrom;
    double myTo;
    bool friendlyPos;

private:
    /// @brief Invalidated assignment operator.
    NBPTStop& operator=(const NBPTStop&);


};

#endif //SUMO_NBPTSTOP_H
