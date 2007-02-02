/****************************************************************************/
/// @file    MFXCheckableButton.h
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
/// @version $Id$
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
#ifndef MFXCheckableButton_h
#define MFXCheckableButton_h
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

#include <fx.h>

class MFXCheckableButton : public FXButton
{
    FXDECLARE(MFXCheckableButton)
public:
    MFXCheckableButton(bool amChecked, FXComposite* p,const FXString& text,
                       FXIcon* ic=NULL,FXObject* tgt=NULL,FXSelector sel=0,
                       FXuint opts=BUTTON_NORMAL,
                       FXint x=0,FXint y=0,FXint w=0,FXint h=0,
                       FXint pl=DEFAULT_PAD,FXint pr=DEFAULT_PAD,FXint pt=DEFAULT_PAD,FXint pb=DEFAULT_PAD);
    ~MFXCheckableButton();
    bool amChecked() const;
    void setChecked(bool val);
    long onPaint(FXObject*,FXSelector,void*);
    long onUpdate(FXObject*,FXSelector,void*);


private:
    void buildColors();
    void setColors();

private:
    bool myAmChecked;
    FXColor myBackColor, myDarkColor, myHiliteColor, myShadowColor;

    bool myAmInitialised;

protected:
    MFXCheckableButton()
    { }
};


#endif

/****************************************************************************/

