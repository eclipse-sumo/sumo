/****************************************************************************/
/// @file    MFXCheckableButton.cpp
/// @author  unknown_author
/// @date    unknown_date
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
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MFXCheckableButton.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


FXDEFMAP(MFXCheckableButton) MFXCheckableButtonMap[]={
            FXMAPFUNC(SEL_PAINT,0,MFXCheckableButton::onPaint),
            FXMAPFUNC(SEL_UPDATE,0,MFXCheckableButton::onUpdate),
        };


// Object implementation
FXIMPLEMENT(MFXCheckableButton,FXButton,MFXCheckableButtonMap,ARRAYNUMBER(MFXCheckableButtonMap))

MFXCheckableButton::MFXCheckableButton(bool amChecked, FXComposite* p,
                                       const FXString& text, FXIcon* ic,
                                       FXObject* tgt,FXSelector sel,
                                       FXuint /*opts*/,
                                       FXint x,FXint y,FXint w,FXint h,
                                       FXint pl,FXint pr,FXint pt,FXint pb)
        : FXButton(p, text, ic, tgt, sel, BUTTON_DEFAULT, x, y, w,h, pl,pr,pt,pb),
        myAmChecked(amChecked), myAmInitialised(false)
{}


MFXCheckableButton::~MFXCheckableButton()
{}


bool
MFXCheckableButton::amChecked() const
{
    return myAmChecked;
}


void
MFXCheckableButton::setChecked(bool val)
{
    myAmChecked = val;
}


long
MFXCheckableButton::onPaint(FXObject*sender,FXSelector sel,void *data)
{
    if (!myAmInitialised) {
        buildColors();
    }
    setColors();
    return FXButton::onPaint(sender, sel, data);
}


long
MFXCheckableButton::onUpdate(FXObject *sender,FXSelector sel,void *data)
{
    if (!myAmInitialised) {
        buildColors();
    }
    setColors();
    long ret = FXButton::onUpdate(sender, sel, data);
    return ret;
}


void
MFXCheckableButton::buildColors()
{
    myBackColor = backColor;
    myDarkColor = makeShadowColor(myBackColor);
    myHiliteColor = hiliteColor;
    myShadowColor = shadowColor;
    myAmInitialised = true;
}


void
MFXCheckableButton::setColors()
{
    options &= (0xffffffff-(FRAME_SUNKEN|FRAME_SUNKEN|FRAME_THICK));
    if (myAmChecked) {
        backColor = myShadowColor;
        hiliteColor = myDarkColor;
        shadowColor = myHiliteColor;
        if (state==STATE_ENGAGED) {
            options |= FRAME_SUNKEN|FRAME_THICK;
        } else {
            options |= FRAME_SUNKEN;
        }
    } else {
        backColor = myBackColor;
        hiliteColor = myHiliteColor;
        shadowColor = myShadowColor;
        if (state==STATE_ENGAGED) {
            options |= FRAME_RAISED|FRAME_THICK;
        } else {
            options |= FRAME_RAISED;
        }
    }
}



/****************************************************************************/

