/****************************************************************************/
/// @file    NBPTLine.h
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// The representation of one direction of a single pt line
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
#ifndef SUMO_NBPTLINE_H
#define SUMO_NBPTLINE_H


#include <string>
#include <vector>
#include <map>

// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class NBPTStop;

class NBPTLine {

public:
    explicit NBPTLine(std::string name);

    void addPTStop(NBPTStop* pStop);
    std::string getName();
    std::vector<NBPTStop*> getStops();
    void write(OutputDevice& device);
    void setId(long long int id);
    void addWayNode(long long int way, long long int node);
    void setRef(std::string basic_string);
private:
    std::string myName;
    std::vector<NBPTStop*> myPTStops;
//public:
//    const std::vector<NBPTStop*>& getMyPTStops() const;
private:
    std::map<std::string, std::vector<long long int> > myWaysNodes;
    std::vector<std::string> myWays;
public:
    const std::vector<std::string>& getMyWays() const;
    std::vector<long long int>* getWaysNodes(std::string wayId);
private:

    std::string myCurrentWay;
    long long int myPTLineId;
    std::string myRef;
};


#endif //SUMO_NBPTLINE_H
