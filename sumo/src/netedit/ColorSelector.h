/****************************************************************************/
/// @file    ColorSelector.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: $
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ColorSelector_h
#define ColorSelector_h

// ===========================================================================
// included modules
// ===========================================================================
#ifndef FXPACKER_H
#include "FXPacker.h"
#endif

#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// namespace definitions
// ===========================================================================
namespace FX
{

// ===========================================================================
// class declarations
// ===========================================================================
class FXColorWell;
//class FXButton;
class FXIcon;

/// Color selection widget
class  ColorSelector : public FXPacker
{
    FXDECLARE(ColorSelector)
protected:
    FXColorWell  *well;                 // Main well
//  FXXPMIcon    *eyedropicon;          // Icon for eye dropper
    FXfloat       rgba[4];              // Accurate RGBA color

protected:
    ColorSelector()
    {}
    void updateWell();
private:
    ColorSelector(const ColorSelector&);
    ColorSelector &operator=(const ColorSelector&);
public:
    long onCmdColor(FXObject*,FXSelector,void*);
    long onChgColor(FXObject*,FXSelector,void*);
    long onCmdSetValue(FXObject*,FXSelector,void*);
    long onCmdColorPick(FXObject*,FXSelector,void*);
    long onCmdWell(FXObject*,FXSelector,void*);
    long onChgWell(FXObject*,FXSelector,void*);
public:
    enum
    {
        ID_COLOR,
        ID_COLORPICK,
        ID_WELL_CHANGED,
        ID_LAST
    };
public:

    /// Construct a new ColorSelector
    ColorSelector(FXComposite *p,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0);

    /// Create the ColorSelector
    virtual void create();

    /// Set the selected color
    void setRGBA(FXColor clr);

    /// Get the selected color
    FXColor getRGBA() const;

    /// Destructor
    virtual ~ColorSelector();
};

}


#endif

/****************************************************************************/

