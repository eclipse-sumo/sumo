/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MFXMenuHeader.cpp
/// @author  Daniel Krajzewicz
/// @date    2004-07-02
/// @version $Id$
///
// Header for menu commands
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MFXMenuHeader.h"

MFXMenuHeader::MFXMenuHeader(FXComposite* p, FXFont* fnt, const FXString& text, FXIcon* ic, FXObject* tgt, FXSelector sel, FXuint opts) :
    FXMenuCommand(p, text, ic, tgt, sel, opts) {
    setFont(fnt);
}


MFXMenuHeader::~MFXMenuHeader() {}



/****************************************************************************/

