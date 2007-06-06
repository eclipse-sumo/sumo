/****************************************************************************/
/// @file    MFXImageHelper.h
/// @author  Daniel Krajzewicz
/// @date    2005-05-04
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
#ifndef MFXImageHelper_h
#define MFXImageHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fx.h>

class MFXImageHelper
{
public:
    static FXImage * loadimage(FXApp *a, const std::string& file);
    static FXbool saveimage(FXApp *a, const std::string& file,
                            int width, int height, FXColor *data);


};


#endif

/****************************************************************************/

