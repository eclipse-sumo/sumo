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
// Revision 1.2  2004/11/23 10:36:02  dkrajzew
// debugging
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
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
