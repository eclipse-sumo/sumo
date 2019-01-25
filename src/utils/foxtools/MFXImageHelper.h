/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2005-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MFXImageHelper.h
/// @author  Daniel Krajzewicz
/// @date    2005-05-04
/// @version $Id$
///
// static helpers for loading and saving images in various formats
/****************************************************************************/
#ifndef MFXImageHelper_h
#define MFXImageHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <vector>
#include <fx.h>
#include <utils/common/UtilExceptions.h>

class MFXImageHelper {
public:
    static void checkSupported(FXString ext);
    static FXImage* loadImage(FXApp* a, const std::string& file);
    static FXbool scalePower2(FXImage* image, int maxSize = (2 << 29));
    static FXbool saveImage(const std::string& file,
                            int width, int height, FXColor* data);


};


#endif

/****************************************************************************/

