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
// Revision 1.4  2005/10/07 11:44:53  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/23 06:07:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.2  2005/09/15 12:18:59  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:38:29  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:47  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
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
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <fx.h>
#include <utils/helpers/ValueSource.h>


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

//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

#endif

// Local Variables:
// mode:C++
// End:
