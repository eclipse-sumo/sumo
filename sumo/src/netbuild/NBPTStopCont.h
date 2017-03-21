/****************************************************************************/
/// @file    NBPtStopCont.h
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id: NBPTStopCont.h 23150 2017-02-27 12:08:30Z behrisch $
///
// Container for pt stops during the netbuilding process
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

#ifndef SUMO_NBPTSTOPCONT_H
#define SUMO_NBPTSTOPCONT_H

#include <string>
#include <map>
#include "NBPTStop.h"
class NBPTStopCont {

public:
    /** @brief Inserts a node into the map
    * @param[in] stop The pt stop to insert
    * @return Whether the pt stop could be added
    */
    bool insert(NBPTStop* ptStop);


    /// @brief Returns the number of pt stops stored in this container
    int size() const {
        return (int) myPTStops.size();
    }

private:
    /// @brief Definition of the map of names to pt stops
    typedef std::map<std::string, NBPTStop*> PTStopsCont;

    /// @brief The map of names to pt stops
    PTStopsCont myPTStops;

};

#endif //SUMO_NBPTSTOPCONT_H
