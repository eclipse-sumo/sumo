#ifndef FontStorage_h
#define FontStorage_h
//---------------------------------------------------------------------------//
//                        FontStorage.h -
//  A temporary storage for fonts
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.5  2004/11/23 10:35:13  dkrajzew
// debugging
//
// Revision 1.4  2003/07/30 12:55:10  dkrajzew
// bug on handling of compiled fonts (partially?) patched
//
// Revision 1.3  2003/06/18 11:24:29  dkrajzew
// parsing of character sets from char-arrays implemented
//
// Revision 1.2  2003/06/05 14:28:06  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <map>
#include "lfontrenderer.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class FontStorage {
public:
    /// Constructor
    FontStorage();

    /// Destructor
    ~FontStorage();

    /// Reads the font from the given file
    void add(const std::string &name, const std::string &file);


    void add(const std::string &fontname, const unsigned char * const characters);

    /// Returns the definition of the named font
    const LFont &get(const std::string &name) const;

    /// returns the information whether the named font was loaded
    bool has(const std::string &name) const;

private:
    /// Definitions of the map from font names to fonts
    typedef std::map<std::string, LFont> FontMap;

    /// The fonts known
    FontMap myFonts;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

