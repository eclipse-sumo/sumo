#ifndef GUIParameterTableItem_h
#define GUIParameterTableItem_h
//---------------------------------------------------------------------------//
//                        GUIParameterTableItem.h -
//  A single line in the parameter window
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                :
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.3  2004/03/19 12:40:14  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/11/12 14:09:12  dkrajzew
// clean up after recent changes; comments added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <fx.h>
#include <helpers/ValueSource.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIParameterTableItem
 * This class represents a single item of a parameter table.
 * As some values may change over the simulation, this class holds the
 * information whether they change and how to ask for new values if they do
 */
class GUIParameterTableItem
{
public:
    /// Constructor for changing values (double-typed)
    GUIParameterTableItem(FXTable *table, size_t pos,
        const std::string &name, bool dynamic, ValueSource<double> *src);

    /// Constructor for double-typed, non-changing values
    GUIParameterTableItem(FXTable *table, size_t pos,
        const std::string &name, bool dynamic, double value);

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
    ValueSource<double> *getSourceCopy() const;

private:
    /// Information whether the value may change
    bool myAmDynamic;

    /// The name of this entry
    std::string myName;

    /// The position within the table
    size_t myTablePosition;

    /** @brief The source to gain new values from
        This source is==0 if the values are not dynamic */
    ValueSource<double> *mySource;

    /// A backup of the value to avoid the redrawing when nothing has changed
    double myValue;

    FXTable *myTable;

};

//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

#endif

// Local Variables:
// mode:C++
// End:
