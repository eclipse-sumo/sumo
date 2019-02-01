/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBPTStop.h
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// The representation of a single pt stop
/****************************************************************************/
#ifndef SUMO_NBPTSTOP_H
#define SUMO_NBPTSTOP_H

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <utils/geom/Position.h>
#include "utils/common/SUMOVehicleClass.h"
#include "NBPTPlatform.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class NBEdgeCont;


// ===========================================================================
// class definitions
// ===========================================================================
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
    NBPTStop(std::string ptStopId, Position position, std::string edgeId, std::string origEdgeId, double length, std::string name, SVCPermissions svcPermissions);
    std::string getID() const;

    const std::string getEdgeId() const;
    const std::string getOrigEdgeId() const;
    const std::string getName() const;
    const Position& getPosition() const;
    SVCPermissions getPermissions() const;
    void write(OutputDevice& device);
    void reshiftPosition(const double offsetX, const double offsetY);

    const std::vector<NBPTPlatform>& getPlatformCands();
    bool getIsMultipleStopPositions() const;
    void setIsMultipleStopPositions(bool multipleStopPositions);
    double getLength() const;
    bool setEdgeId(std::string edgeId, NBEdgeCont& ec);
    void registerAdditionalEdge(std::string wayId, std::string edgeId);
    void addPlatformCand(NBPTPlatform platform);
    bool findLaneAndComputeBusStopExtent(NBEdgeCont& ec);

    void setMyPTStopId(std::string id);
    void addAccess(std::string laneID, double offset, double length);

    /// @brief remove all access definitions
    void clearAccess();

    /// @brief register line that services this stop (for displaying)
    void addLine(const std::string& line);

    void setBidiStop(NBPTStop* bidiStop) {
        myBidiStop = bidiStop;
    }

    NBPTStop* getBidiStop() const {
        return myBidiStop;
    }

private:
    void computeExtent(double center, double d);

private:
    std::string myPTStopId;
    Position myPosition;
    std::string myEdgeId;
    std::map<std::string, std::string> myAdditionalEdgeCandidates;
public:
    const std::map<std::string, std::string>& getMyAdditionalEdgeCandidates() const;
private:
    std::string myOrigEdgeId;
public:
    void setMyOrigEdgeId(const std::string& myOrigEdgeId);
private:
    double myPTStopLength;
public:
    void setMyPTStopLength(double myPTStopLength);
private:
    const std::string myName;
    std::string myLaneId;
    const SVCPermissions myPermissions;

    double myStartPos;
    double myEndPos;

    /// @brief laneId, lanePos, accessLength
    std::vector<std::tuple<std::string, double, double>> myAccesses;

    /// @brief list of public transport lines (for displaying)
    std::vector<std::string> myLines;

    NBPTStop* myBidiStop;


private:
    /// @brief Invalidated assignment operator.
    NBPTStop& operator=(const NBPTStop&);


    std::vector<NBPTPlatform> myPlatformCands;
    bool myIsMultipleStopPositions;
};

#endif //SUMO_NBPTSTOP_H
