/****************************************************************************/
/// @file    FXRealSpinDial.h
/// @author  Lyle Johnson
/// @author  Bill Baxter
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-03-19
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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


#ifndef FXREALSPINDIAL_H
#define FXREALSPINDIAL_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef FXPACKER_H
#include "FXPacker.h"
#endif

namespace FX {


// Spinner Options
enum {
    SPINDIAL_CYCLIC    =  SPIN_CYCLIC,   // Cyclic spinner
    SPINDIAL_NOTEXT    =  SPIN_NOTEXT,   // No text visible
    SPINDIAL_NOMAX     =  SPIN_NOMAX,    // Spin all the way up to infinity
    SPINDIAL_NOMIN     =  SPIN_NOMIN,    // Spin all the way down to -infinity
    SPINDIAL_LOG       =  0x00200000,    // Logarithmic rather than linear
    SPINDIAL_NODIAL    =  0x00400000,    // No dial visible
    SPINDIAL_NOBUTTONS =  0x00800000,    // No spinbuttons visible
    SPINDIAL_NORMAL    =  SPINDIAL_NOBUTTONS// Normal, non-cyclic, no buttons
};

enum {
    SPINDIAL_INC_NORMAL = 0,
    SPINDIAL_INC_FINE = -1,
    SPINDIAL_INC_COARSE = 1
};

class FXRealSpinDialText;
class FXDial;


/// Spinner control
class /*FXAPI*/ FXRealSpinDial : public FXPacker {
    FXDECLARE(FXRealSpinDial)
protected:
    FXRealSpinDialText* textField; // Text field
    FXArrowButton* upButton;          // The up button
    FXArrowButton* downButton;        // The down button
    FXDial*        dial;              // The up/down dial
    FXdouble       range[2];          // Reported data range
    FXdouble       incr[3];           // Increments (fine,normal,coarse)
    FXdouble       pos;               // Current position
    FXint          dialpos;           // Current position of dial
    FXint          keystate;          // Current key modifiers
protected:
    FXRealSpinDial();
private:
    FXRealSpinDial(const FXRealSpinDial&);
    FXRealSpinDial& operator=(const FXRealSpinDial&);
public:
    long onUpdDial(FXObject*, FXSelector, void*);
    long onChgDial(FXObject*, FXSelector, void*);
    long onCmdDial(FXObject*, FXSelector, void*);
    long onUpdIncrement(FXObject*, FXSelector, void*);
    long onCmdIncrement(FXObject*, FXSelector, void*);
    long onUpdDecrement(FXObject*, FXSelector, void*);
    long onCmdDecrement(FXObject*, FXSelector, void*);
    long onMouseWheel(FXObject*, FXSelector, void*);
    long onUpdEntry(FXObject*, FXSelector, void*);
    long onCmdEntry(FXObject*, FXSelector, void*);
    long onChgEntry(FXObject*, FXSelector, void*);
    long onKeyPress(FXObject*, FXSelector, void*);
    long onKeyRelease(FXObject*, FXSelector, void*);
    long onCmdSetValue(FXObject*, FXSelector, void*);
    long onCmdSetIntValue(FXObject*, FXSelector, void*);
    long onCmdGetIntValue(FXObject*, FXSelector, void*);
    long onCmdSetIntRange(FXObject*, FXSelector, void*);
    long onCmdGetIntRange(FXObject*, FXSelector, void*);
    long onCmdSetRealValue(FXObject*, FXSelector, void*);
    long onCmdGetRealValue(FXObject*, FXSelector, void*);
    long onCmdSetRealRange(FXObject*, FXSelector, void*);
    long onCmdGetRealRange(FXObject*, FXSelector, void*);
    long onMotion(FXObject*, FXSelector, void*);
    //long onDefault(FXObject*,FXSelector,void*);
public:
    enum {
        ID_DIAL = FXPacker::ID_LAST,
        ID_INCREMENT,
        ID_DECREMENT,
        ID_ENTRY,
        ID_LAST
    };
public:

    /// Construct a spinner
    FXRealSpinDial(FXComposite* p, FXint cols, FXObject* tgt = NULL,
                   FXSelector sel = 0, FXuint opts = SPINDIAL_NORMAL,
                   FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0,
                   FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD
                  );

    /// Create server-side resources
    virtual void create();

    /// Perform layout
    virtual void layout();

    /// Disable spinner
    virtual void disable();

    /// Enable spinner
    virtual void enable();

    /// Return default width
    virtual FXint getDefaultWidth();

    /// Return default height
    virtual FXint getDefaultHeight();

    /// Increment spinner
    void increment(FXint incMode = SPINDIAL_INC_NORMAL);

    /// Decrement spinner
    void decrement(FXint incMode = SPINDIAL_INC_NORMAL);

    /// Return TRUE if in cyclic mode
    FXbool isCyclic() const;

    /// Set to cyclic mode, i.e. wrap around at maximum/minimum
    void setCyclic(FXbool cyclic);

    /// Return TRUE if text is visible
    FXbool isTextVisible() const;

    /// Set text visible flag
    void setTextVisible(FXbool shown);

    /// Return TRUE if dial is visible
    FXbool isDialVisible() const;

    /// Set dial visible flag
    void setDialVisible(FXbool shown);

    /// Change current value
    virtual void setValue(FXdouble value);

    /// Return current value
    FXdouble getValue() const {
        return pos;
    }

    /// Change the spinner's range
    void setRange(FXdouble lo, FXdouble hi);

    /// Get the spinner's current range
    void getRange(FXdouble& lo, FXdouble& hi) const {
        lo = range[0];
        hi = range[1];
    }

    /// Change spinner increment
    void setIncrement(FXdouble increment);
    /// Change all spinner increment
    void setIncrements(FXdouble fine, FXdouble norm, FXdouble coarse);
    /// Change spinner fine adjustment increment (when CTRL key held down)
    void setFineIncrement(FXdouble increment);
    /// Change spinner coarse adjustment increment (when SHIFT key held down)
    void setCoarseIncrement(FXdouble increment);

    /// Return spinner increment
    FXdouble getIncrement() const {
        return incr[1];
    }
    /// Return spinner increment
    FXdouble getFineIncrement() const {
        return incr[0];
    }
    /// Return spinner increment
    FXdouble getCoarseIncrement() const {
        return incr[2];
    }

    /// Set the text font
    void setFont(FXFont* fnt);

    /// Get the text font
    FXFont* getFont() const;

    /// Set the status line help text for this spinner
    void setHelpText(const FXString& text);

    /// Get the status line help text for this spinner
    FXString getHelpText() const;

    /// Set the tool tip message for this spinner
    void setTipText(const FXString& text);

    /// Get the tool tip message for this spinner
    FXString getTipText() const;

    /// Change spinner style
    void setSpinnerStyle(FXuint style);

    /// Return current spinner style
    FXuint getSpinnerStyle() const;

    /// Allow editing of the text field
    void setEditable(FXbool edit = TRUE);

    /// Return TRUE if text field is editable
    FXbool isEditable() const;

    /// Change color of the dial
    void setDialColor(FXColor clr);

    /// Return color of the dial
    FXColor getDialColor() const;

    /// Change color of the up arrow
    void setUpArrowColor(FXColor clr);

    /// Return color of the up arrow
    FXColor getUpArrowColor() const;

    /// Change color of the down arrow
    void setDownArrowColor(FXColor clr);

    /// Return color of the the down arrow
    FXColor getDownArrowColor() const;

    /// Change text color
    void setTextColor(FXColor clr);

    /// Return text color
    FXColor getTextColor() const;

    /// Change selected background color
    void setSelBackColor(FXColor clr);

    /// Return selected background color
    FXColor getSelBackColor() const;

    /// Change selected text color
    void setSelTextColor(FXColor clr);

    /// Return selected text color
    FXColor getSelTextColor() const;

    /// Changes the cursor color
    void setCursorColor(FXColor clr);

    /// Return the cursor color
    FXColor getCursorColor() const;

    /// Save spinner to a stream
    virtual void save(FXStream& store) const;

    /// Load spinner from a stream
    virtual void load(FXStream& store);

    /// Set the precision and exponent display for numbers
    /// Calling this overrides the number format string.
    void setNumberFormat(FXint prec, FXbool bExp = FALSE);

    /// Return the digits of precision used to display numbers
    FXint getNumberFormatPrecision() const;

    /// Return whether the exponent is used in number display
    FXbool getNumberFormatExponent() const;

    /// Set a format string for number display, using printf
    /// syntax.  Format must contain no more than one %fmt pattern.
    /// Calling this overrides the simple precision/exponent number format.
    void setFormatString(const FXchar* fmt);

    /// Return the format string for number display.
    FXString getNumberFormatString() const;

    /// Mark the text entry as selected
    void selectAll();

    const FXDial& getDial() const;

    /// Destructor
    virtual ~FXRealSpinDial();
};

}


#endif //  FXREALSPINDIAL_H
