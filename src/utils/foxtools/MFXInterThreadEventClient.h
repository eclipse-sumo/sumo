/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MFXInterThreadEventClient.h
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
/// @version $Id$
///
// missing_desc
/****************************************************************************/
#ifndef MFXInterThreadEventClient_h
#define MFXInterThreadEventClient_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>



class MFXInterThreadEventClient {
public:
    MFXInterThreadEventClient() {}
    virtual ~MFXInterThreadEventClient() { }
    virtual void eventOccurred() = 0;
};


#endif

/****************************************************************************/

