/***************************************************************************
                          XMLSubSys.cpp
			  Utility for initialisation and closing of the XML-subsystem
                             -------------------
    project              : SUMO
    begin                : Mon, 1 Jul 2002
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.1  2002/10/16 14:54:04  dkrajzew
// initial commit for xml-related utility functions
//
// Revision 1.1  2002/07/31 17:29:16  roessel
// Initial sourceforge commit.
//
// Revision 1.1  2002/07/02 08:29:47  dkrajzew
// A class to encapsulate the XML-Subsystem initialisation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <util/PlatformUtils.hpp>
#include <utils/convert/TplConvert.h>
#include "XMLSubSys.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
bool XMLSubSys::init() {
    try {
        XMLPlatformUtils::Initialize();
        return true;
    } catch (const XMLException& toCatch) {
        cerr << "Error during XML-initialization: "
            << TplConvert<XMLCh>::_2str(toCatch.getMessage()) << endl;
        return false;
    }
}


void XMLSubSys::close() {
    XMLPlatformUtils::Terminate();
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "XMLSubSys.icc"
//#endif

// Local Variables:
// mode:C++
// End:

