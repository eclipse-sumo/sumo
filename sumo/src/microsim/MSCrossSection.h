#ifndef MSCROSSSECTION_H
#define MSCROSSSECTION_H

/**
 * @file   MSCrossSection.h
 * @author Christian Roessel
 * @date   Started Tue Nov 25 15:23:28 2003
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

class MSLane;
#include "MSUnit.h"
#include <vector>

struct MSCrossSection
{
    MSCrossSection( MSLane& lane
                    , MSUnit::Meters pos ) 
        : laneM( lane )
        , posM( pos )
        {}

    MSLane& laneM;
    MSUnit::Meters posM;
};

namespace Detector
{
    typedef std::vector< MSCrossSection > CrossSections;
    typedef CrossSections::iterator CrossSectionsIt;
}



#endif // MSCROSSSECTION_H
