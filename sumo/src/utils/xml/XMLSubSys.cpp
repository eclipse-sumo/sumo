/****************************************************************************/
/// @file    XMLSubSys.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2002
/// @version $Id$
///
// Utility for initialising and closing the XML-subsystem
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <utils/common/TplConvert.h>
#include "XMLSubSys.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
bool
XMLSubSys::init()
{
    try {
        XMLPlatformUtils::Initialize();
        return true;
    } catch (const XMLException& toCatch) {
        cerr << "Error during XML-initialization: "
        << TplConvert<XMLCh>::_2str(toCatch.getMessage()) << endl;
        return false;
    }
}


void
XMLSubSys::close()
{
    XMLPlatformUtils::Terminate();
}



/****************************************************************************/

