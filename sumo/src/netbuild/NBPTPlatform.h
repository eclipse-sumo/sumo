/****************************************************************************/
/// @file    NBPTPlatform.h
/// @author  Gregor Laemmel
/// @date    Tue, 24 Aug 2017
/// @version $Id: NBPTPlatform.h 25685 2017-08-22 11:37:56Z laemmel $
///
// The representation of a pt platform
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

#ifndef SUMO_NBPTPLATFORM_H
#define SUMO_NBPTPLATFORM_H


#include <utils/geom/Position.h>
class NBPTPlatform {

public:
    NBPTPlatform(Position position, double d);
private:
    Position myPos;
public:
    Position* getMyPos();
    double getMyLength() const;
private:
    double myLength;
};


#endif //SUMO_NBPTPLATFORM_H
