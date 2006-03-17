#ifndef RODFFrame_h
#define RODFFrame_h
/***************************************************************************
                          RODFFrame.cpp
    Some IO-initialisations for the DFROUTER
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
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

// $Log$
// Revision 1.4  2006/03/17 09:04:26  dkrajzew
// class-documentation added/patched
//
// Revision 1.3  2006/01/16 13:37:47  dkrajzew
// debugging
//
// Revision 1.2  2006/01/16 10:46:24  dkrajzew
// some initial work on  the dfrouter
//
// Revision 1.1  2005/12/08 12:59:41  ericnicolay
// *** empty log message ***
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


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RODFFrame
 * @brief Some IO-initialisations for the DFROUTER
 */
class RODFFrame {
public:
    /// Builds the simulation options
    static void fillOptions(OptionsCont &oc);

    static bool checkOptions(OptionsCont &oc);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

