/****************************************************************************/
/// @file    FXLCDLabel.h
/// @author  Mathew Robertson
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-03-19
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2004-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


#ifndef FXLCDLABEL_H
#define FXLCDLABEL_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif



#ifndef FXHORIZONTALFRAME_H
#include <FXHorizontalFrame.h>
using namespace FX;
#endif
namespace FXEX {

/// LCD Label styles
enum {
    LCDLABEL_NORMAL        = FRAME_SUNKEN | FRAME_THICK,
    LCDLABEL_LEADING_ZEROS = 0x01000000    /// leading zero's on label - useful for integers
};

/**
 * A widget which has the seven-segment display used as the drawing object for each letter
 * in the label.  Supports display of leading zeros, such as when you need to display a
 * number.  Also supports the 'JUSTIFY_RIGHT' option, for right alignment.
 */
class /* FXAPI // patch by Daniel Krajzewicz 24.02.2004 */
        FXLCDLabel : public FXHorizontalFrame {
    FXDECLARE(FXLCDLabel)

protected:
    FXLCDLabel() {}

    FXString label;      /// The currently shown label
    FXint   nfigures;    /// The number of shown figuresi, greater than zero

    // Draw a string
    virtual void drawString(const FXString& lbl);

public:
    enum {
        ID_SEVENSEGMENT = FXHorizontalFrame::ID_LAST,
        ID_LAST
    };

public:
    long onPaint(FXObject*, FXSelector, void*);
    long onCmdSetValue(FXObject*, FXSelector, void* ptr);
    long onCmdSetIntValue(FXObject*, FXSelector, void* ptr);
    long onCmdSetRealValue(FXObject*, FXSelector, void* ptr);
    long onCmdSetStringValue(FXObject*, FXSelector, void* ptr);
    long onCmdGetIntValue(FXObject*, FXSelector, void* ptr);
    long onCmdGetRealValue(FXObject*, FXSelector, void* ptr);
    long onCmdGetStringValue(FXObject*, FXSelector, void* ptr);
    long onRedirectEvent(FXObject*, FXSelector, void* ptr);
    long onQueryTip(FXObject*, FXSelector, void* ptr);
    long onQueryHelp(FXObject*, FXSelector, void* ptr);

public:
    /// make me a label
    FXLCDLabel(FXComposite* p, FXuint nfig = 1, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = LCDLABEL_NORMAL, FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD, FXint hs = DEFAULT_PAD);

    /// create resrouces
    virtual void create();

    /// detach resources
    virtual void detach();

    /// destroy resources
    virtual void destroy();

    /// manipulate text in LCD label
    void setText(FXString lbl);
    FXString getText() const {
        return label;
    }

    /// set/get forground color
    void setFgColor(FXColor clr);
    FXColor getFgColor() const;

    /// set/get background color
    void setBgColor(FXColor clr);
    FXColor getBgColor() const;

    /// set/get segment horizontal length - must be more than twice the segment width
    void setHorizontal(const FXint len);
    FXint getHorizontal() const;

    /// set/get segment vertical length - must be more than twice the segment width
    void setVertical(const FXint len);
    FXint getVertical() const;

    /// set/get segment width - must be less than half the segment length
    void setThickness(const FXint width);
    FXint getThickness() const;

    /// set/get groove width - must be less than segment width
    void setGroove(const FXint width);
    FXint getGroove() const;

    /// return minimum width
    virtual FXint getDefaultWidth();

    /// return minimum height
    virtual FXint getDefaultHeight();

    /// save resources
    virtual void save(FXStream& store) const;

    /// load resources
    virtual void load(FXStream& store);

    /// dtor
    virtual ~FXLCDLabel();
};

} // namespace FXEX
#endif // FXLCDLabel
