/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Junction.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <microsim/MSNet.h>
#include <microsim/MSJunctionControl.h>
#include "Helper.h"
#include "Junction.h"


// ===========================================================================
// member definitions
// ===========================================================================
namespace libsumo {
std::vector<std::string>
Junction::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getJunctionControl().insertIDs(ids);
    return ids;
}

int
Junction::getIDCount() {
    return (int)getIDList().size();
}

TraCIPosition
Junction::getPosition(const std::string& junctionID) {
    return Helper::makeTraCIPosition(getJunction(junctionID)->getPosition());
}

TraCIPositionVector
Junction::getShape(const std::string& junctionID) {
    return Helper::makeTraCIPositionVector(getJunction(junctionID)->getShape());
}

MSJunction*
Junction::getJunction(const std::string& id) {
    MSJunction* j = MSNet::getInstance()->getJunctionControl().get(id);
    if (j == 0) {
        throw TraCIException("Junction '" + id + "' is not known");
    }
    return j;
}

NamedRTree*
Junction::getTree() {
    NamedRTree* t = new NamedRTree();
    for (const auto& i : MSNet::getInstance()->getJunctionControl()) {
        Boundary b = i.second->getShape().getBoxBoundary();
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        t->Insert(cmin, cmax, i.second);
    }
    return t;
}

}


/****************************************************************************/
