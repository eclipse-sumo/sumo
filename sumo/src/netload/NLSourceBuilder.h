#ifndef NLSourceBuilder_h
#define NLSourceBuilder_h
/***************************************************************************
                          NLSourceBuilder.h
                          A building helper for the sources
                             -------------------
    begin                : Mon, 22 Jul 2002
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSSource;
class MSEventControl;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * This class builds the sources
 */
class NLSourceBuilder {
public:
    /// builds a MSTriggeredSource - source
    static MSSource *buildTriggeredSource(const std::string &id,
        std::string file, std::string base);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLSourceBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
//
