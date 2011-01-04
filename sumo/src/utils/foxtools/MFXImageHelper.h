/****************************************************************************/
/// @file    MFXImageHelper.h
/// @author  Daniel Krajzewicz
/// @date    2005-05-04
/// @version $Id$
///
// static helpers for loading and saving images in various formats
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/UtilExceptions.h>

class MFXImageHelper {
public:
    static void checkSupported(FXString ext) throw(InvalidArgument);
    static FXImage * loadImage(FXApp *a, const std::string& file);
    static FXbool scalePower2(FXImage *image);
    static FXbool saveImage(const std::string& file,
                            int width, int height, FXColor *data);


};


#endif

/****************************************************************************/

