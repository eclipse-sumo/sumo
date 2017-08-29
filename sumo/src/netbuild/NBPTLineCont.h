/****************************************************************************/
/// @file    NBPTLineCont.h
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// Container for NBPTLine during netbuild
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

#ifndef SUMO_NBPTLINECONT_H
#define SUMO_NBPTLINECONT_H


#include <vector>
#include "NBPTLine.h"
#include "NBEdgeCont.h"

class NBPTLineCont {

public:
    /// @brief constructor
    NBPTLineCont();

    /// @brief destructor
    ~NBPTLineCont();

    /// @brief insert new line
    void insert(NBPTLine* pLine);

    /** @brief Returns the pointer to the begin of the stored pt lines
    * @return The iterator to the beginning of stored pt lines
    */
    std::vector<NBPTLine*>::const_iterator begin() const {
        return myPTLines.begin();
    }

    /** @brief Returns the pointer to the end of the stored pt lines
     * @return The iterator to the end of stored pt lines
     */
    std::vector<NBPTLine*>::const_iterator end() const {
        return myPTLines.end();
    }

    void process(NBEdgeCont& cont);
private:

    static const int FWD;
    static const int BWD;

    /// @brief Definition of the map of names to pt lines
    typedef std::vector<NBPTLine*> PTLinesCont;

    /// @brief The map of names to pt lines
    PTLinesCont myPTLines;

    long long int myIdCnt;
    void reviseStops(NBPTLine* myPTLine, NBEdgeCont& cont);
    void constructRoute(NBPTLine* myPTLine, NBEdgeCont& cont);
};


#endif //SUMO_NBPTLINECONT_H
