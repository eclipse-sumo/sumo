#ifndef MSDetectorFileOutput_H
#define MSDetectorFileOutput_H

/**
 * @file   MSDetectorFileOutput.h
 * @author Christian Roessel
 * @date   Thu Jul 31 12:28:19 2003
 * @version $Id$
 * @brief  Declaration of pure virtual base class MSDetectorFileOutput
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

// $Id$

#include <string>
#include "MSNet.h"

class MSDetectorFileOutput
{
public:
    virtual std::string  getNamePrefix( void )           = 0;
    virtual std::string& getXMLHeader( void )            = 0;
    virtual std::string  getXMLOutput( MSNet::Time )     = 0;
    virtual std::string  getXMLDetectorInfoStart( void ) = 0;
    virtual std::string& getXMLDetectorInfoEnd( void )   = 0;
};

#endif

// Local Variables:
// mode:C++
// End:
