/****************************************************************************/
/// @file    GUIParameterTableItem.h
/// @author  Daniel Krajzewicz
/// @date
/// @version $Id: $
///
// A single line in the parameter window
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
#ifndef GUIParameterTableItem_h
#define GUIParameterTableItem_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <fx.h>
#include <utils/helpers/ValueSource.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIParameterTableItem
 * This class represents a single item of a parameter table.
 * As some values may change over the simulation, this class holds the
 * information whether they change and how to ask for new values if they do
 */
class GUIParameterTableItem
{
public:
    /// Constructor for changing values (SUMOReal-typed)
    GUIParameterTableItem(FXTable *table, size_t pos,
                          const std::string &name, bool dynamic, ValueSource<SUMOReal> *src);

    /// Constructor for SUMOReal-typed, non-changing values
    GUIParameterTableItem(FXTable *table, size_t pos,
                          const std::string &name, bool dynamic, SUMOReal value);

    /// Constructor for string-typed, non-changing values
    GUIParameterTableItem(FXTable *table, size_t pos,
                          const std::string &name, bool dynamic, std::string value);

    void init(bool dynamic, std::string value);


    /// Destructor
    ~GUIParameterTableItem();

    /// Returns the infomration whether this item may change
    bool dynamic() const;

    /// Returns the name of this item
    const std::string &getName() const;

    /// Returns the position within the table
    size_t getTablePosition() const;

    /// Resets the value if it's dynamic
    void update();

    /// Returns a copy of the source - if the value is dynamic
    ValueSource<SUMOReal> *getSourceCopy() const;

private:
    /// Information whether the value may change
    bool myAmDynamic;

    /// The name of this entry
    std::string myName;

    /// The position within the table
    size_t myTablePosition;

    /** @brief The source to gain new values from
        This source is==0 if the values are not dynamic */
    ValueSource<SUMOReal> *mySource;

    /// A backup of the value to avoid the redrawing when nothing has changed
    SUMOReal myValue;

    FXTable *myTable;

};


#endif

/****************************************************************************/

