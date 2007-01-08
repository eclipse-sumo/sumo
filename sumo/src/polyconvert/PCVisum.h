#ifndef PCVisum_h
#define PCVisum_h
/***************************************************************************
                          PCVisum.h
    A reader of polygons stored in visum-format
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Mon, 05 Dec 2005
    copyright            : (C) 2005 by DLR http://ivf.dlr.de/
    author               : Danilo Boyom
    email                : Danilot.Tete-Boyom@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.2  2007/01/08 14:43:59  dkrajzew
// code beautifying; prliminary import for Visum points added
//
// Revision 1.1  2006/08/01 07:52:47  dkrajzew
// polyconvert added
//
// Revision 1.1  2006/03/27 07:22:27  dksumo
// initial checkin
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include "PCPolyContainer.h"


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * class definitions
 * ======================================================================= */
class OptionsCont;
class NBNetBuilder;


/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * @class PCVisum
 * @brief A reader of polygons stored in visum-format
 */
class PCVisum {
public:
    PCVisum(PCPolyContainer &toFill);
    ~PCVisum();

    /** loads data from the files specified in the given option container */
    void load(std::string opt, std::string file);

    /** loads data from visum-input-file */
    void load(OptionsCont &oc);


protected:
    /// Container to store the read polygons into
    PCPolyContainer &myCont;

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
