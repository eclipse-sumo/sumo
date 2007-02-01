/****************************************************************************/
/// @file    ODmatrix.h
/// @author  Daniel Krajzewicz
/// @date    05. Apr. 2006
/// @version $Id: $
///
// missing_desc
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
#ifndef ODmatrix_h
#define ODmatrix_h
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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ODMatrix
 * This class is the iternal representation of the loaded matrix.
 * It also computes and writes the trips.
 */
class ODMatrix
{
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
    struct ODVehicle
    {
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
    class cell_by_begin_sorter
    {
    public:
        /// constructor
        explicit cell_by_begin_sorter()
        { }

    public:
        /// comparing operation
        int operator()(ODCell *p1, ODCell *p2) const
        {
            return p1->begin<p2->begin;
        }

    };

    /**
     * @class descending_departure_comperator
     * Used for sorting vehicles by their departure (latest first)
     */
    class descending_departure_comperator
    {
    public:
        /// constructor
        descending_departure_comperator()
        { }

        /// comparing operation
        bool operator()(const ODVehicle &p1, const ODVehicle &p2) const
        {
            return p1.depart>p2.depart;
        }

    };

};


#endif

/****************************************************************************/

