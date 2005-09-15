/**
 * @file   MSUnit.cpp
 * @author Christian Roessel
 * @date   Thu Aug  7 15:06:18 2003
 * @version $Id$
 * @brief
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

// $Id$


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSUnit.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

MSUnit* MSUnit::instanceM = 0;

const MSUnit::Meters oneKM = 1000.0;
const MSUnit::Seconds oneHour = 3600.0;

// Local Variables:
// mode:C++
// End:
