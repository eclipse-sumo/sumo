/**
 * @file   MSDetectorFileOutput.cpp
 * @author Christian Roessel
 * @date   Started Mon Sep 22 13:36:13 2003
 * @version $Id$
 * @brief  
 * 
 * 
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSDetectorFileOutput.h"

// static member initialization
std::string MSDetectorFileOutput::infoEndM = std::string("</detector>");
