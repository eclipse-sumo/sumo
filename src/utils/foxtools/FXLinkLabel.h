/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    FXLinkLabel.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2006-03-08
/// @version $Id$
///
//
/****************************************************************************/

#ifndef FXLinkLabel_h
#define FXLinkLabel_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>

/** A label which points to a link.
* This class behaves very much like the FXLabel class but it
* points to some link which is started when clicked.
*/
class FXLinkLabel : public FXLabel {
protected:
    // FOX stuff
    FXDECLARE(FXLinkLabel)
    FXLinkLabel() {}
public:
    /// Constructor.
    FXLinkLabel(FXComposite* p, const FXString& text, FXIcon* ic = 0, FXuint opts = LABEL_NORMAL, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0, FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// Destructor.
    virtual ~FXLinkLabel();

    // FOX enum
    enum {
        ID_FIRST = FXLabel::ID_LAST,
        ID_TIMER,

        ID_LAST
    };

    // FOX messages
    long onLeftBtnPress(FXObject*, FXSelector, void*);
    long onTimer(FXObject*, FXSelector, void*);

    // open the given link in a browser or pdf viewer
    static FXint fxexecute(FXString link);
};




#endif
