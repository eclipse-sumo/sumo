#ifndef OptionsSaveStorage_h
#define OptionsSaveStorage_h
/***************************************************************************
                          OptionsSaveStorage.h
              Stores options and allows to save them into a xml-file
                             -------------------
    project              : SUMO
    begin                : 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.5  2005/10/07 11:46:56  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 12:21:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/05/04 09:28:01  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2004/11/23 10:36:02  dkrajzew
// debugging
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
#include <map>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class OptionsSaveStorage
 */
class OptionsSaveStorage {
public:
    /// Constructor
    OptionsSaveStorage(const std::string &structure);

    /// Destructor
    ~OptionsSaveStorage();

    /// Adds an option to save
    void insert(const std::string &name, const std::string &value);

    /// Saves the options into the named file
    bool save(const std::string &file) const;

private:
    /// Definition of the xml-structure
    std::string myStructure;

    /// Definition of the option name-to-value map
    typedef std::map<std::string, std::string> StringMap;

    /// The map of option names to their values
    StringMap myMap;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
