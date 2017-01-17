/****************************************************************************/
/// @file    FXLinkLabel.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2006-03-08
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2006-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef FXLinkLabel_h
#define FXLinkLabel_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
