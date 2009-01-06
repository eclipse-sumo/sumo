/****************************************************************************/
/// @file    MFXMenuHeader.h
/// @author  Daniel Krajzewicz
/// @date    2004-07-02
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MFXMenuHeader_h
#define MFXMenuHeader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>

class MFXMenuHeader : public FXMenuCommand
{
public:
    MFXMenuHeader(FXComposite* p, FXFont *fnt, const FXString& text,FXIcon* ic=NULL,
                  FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=0);
    ~MFXMenuHeader();
};


#endif

/****************************************************************************/

