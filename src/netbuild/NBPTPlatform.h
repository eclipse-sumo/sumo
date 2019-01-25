/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBPTPlatform.h
/// @author  Gregor Laemmel
/// @date    Tue, 24 Aug 2017
/// @version $Id$
///
// The representation of a pt platform
/****************************************************************************/

#ifndef SUMO_NBPTPLATFORM_H
#define SUMO_NBPTPLATFORM_H


#include <utils/geom/Position.h>
class NBPTPlatform {
public:
    NBPTPlatform(Position position, double d);
public:
    const Position& getPos() const;
    void reshiftPosition(const double offsetX, const double offsetY);
    double getLength() const;
private:
    Position myPos;
    double myLength;
};


#endif //SUMO_NBPTPLATFORM_H
