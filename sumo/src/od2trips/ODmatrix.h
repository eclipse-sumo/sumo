#ifndef ODMatrix_h
#define ODMatrix_h
//---------------------------------------------------------------------------//
//                        ODmatrix.h -
//  some matrix usage functions
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
// Revision 1.14  2006/05/15 05:56:24  dkrajzew
// debugged splitting of matrices
//
// Revision 1.13  2006/04/07 05:25:15  dkrajzew
// complete od2trips rework
//
// Revision 1.12  2005/10/07 11:42:00  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.11  2005/09/23 06:04:23  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.10  2005/09/15 12:04:48  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/05/04 08:44:57  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.8  2004/07/02 09:38:21  dkrajzew
// coding style adaptations
//
// Revision 1.7  2003/08/21 12:57:59  dkrajzew
// buffer overflow bug#1 removed
//
// Revision 1.6  2003/08/04 11:37:37  dkrajzew
// added the generation of colors from districts
//
// Revision 1.5  2003/05/20 09:46:53  dkrajzew
// usage of split and non-split od-matrices from visum and vissim rechecked
//
// Revision 1.2  2003/02/07 10:44:19  dkrajzew
// updated
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

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <utils/common/SUMOTime.h>
#include <utils/gfx/RGBColor.h>
#include "ODCell.h"
#include "ODDistrictCont.h"
#include <utils/distribution/Distribution_Points.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ODMatrix
 * This class is the iternal representation of the loaded matrix.
 * It also computes and writes the trips.
 */
class ODMatrix {
public:
    /// Constructor
    ODMatrix();

    /// Destrctor
    ~ODMatrix();

    /** @brief Adds a loaded matrix cell
     *
     * The matrix will get the owner of the cell, the cell must not be deleted
     *  somewhere else
     */
    void add(ODCell *cell);

    /// Writes the vehicles stored in the matrix
    void write(SUMOTime begin, SUMOTime end,
        std::ofstream &strm, const ODDistrictCont &dc, bool uniform,
        const std::string &prefix);


    /// Returns the number of loaded vehicles
    SUMOReal getNoLoaded() const;

    /// Returns the number of written vehicles
    SUMOReal getNoWritten() const;

    /// Splits the stored cells dividing them on the given time line
    void applyCurve(const Distribution_Points &ps);

protected:
    /**
     * @struct ODVehicle
     * An internal representation of a single vehicle to write
     */
    struct ODVehicle {
        /// The id of the vehicle
        std::string id;
        /// The departure time of the vehicle
        SUMOTime depart;
        /// The type of the vehicle
        std::string type;
        /// The edge the vehicles shall start at
        std::string from;
        /// The edge the vehicles shall end at
        std::string to;
        /// The color of the vehicle
        RGBColor color;
    };

    /// Definition of a container for cells
    typedef std::vector<ODCell*> CellVector;

protected:
    /// Computes the emissions stored in the given cell and writes them to "into"
    SUMOReal computeEmissions(const ODDistrictCont &dc, ODCell *cell,
        size_t &vehName, std::vector<ODVehicle> &into, bool uniform,
        const std::string &prefix);

    /** @brief Splits the given cell dividing it on the given time line and
     *      stores the results in the given container
     */
    void applyCurve(const Distribution_Points &ps, ODCell *cell,
        CellVector &newCells);

protected:
    /// The loaded cells
    CellVector myContainer;

    /// Number of loaded vehicles
    SUMOReal myNoLoaded;

    /// Number of written vehicles
    SUMOReal myNoWritten;

    /**
     * @class cell_by_begin_sorter
     * Used for sorting the cells by the begin time they describe
     */
    class cell_by_begin_sorter {
    public:
        /// constructor
        explicit cell_by_begin_sorter() { }

    public:
        /// comparing operation
        int operator() (ODCell *p1, ODCell *p2) const {
            return p1->begin<p2->begin;
        }

    };

    /**
     * @class descending_departure_comperator
     * Used for sorting vehicles by their departure (latest first)
     */
    class descending_departure_comperator {
    public:
        /// constructor
        descending_departure_comperator() { }

        /// comparing operation
        bool operator() (const ODVehicle &p1, const ODVehicle &p2) const {
            return p1.depart>p2.depart;
        }

    };

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

