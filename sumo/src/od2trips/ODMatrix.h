/****************************************************************************/
/// @file    ODMatrix.h
/// @author  Daniel Krajzewicz
/// @date    05. Apr. 2006
/// @version $Id$
///
// An O/D (origin/destination) matrix
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
#ifndef ODMatrix_h
#define ODMatrix_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
#include "ODCell.h"
#include "ODDistrictCont.h"
#include <utils/distribution/Distribution_Points.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ODMatrix
 * @brief An O/D (origin/destination) matrix
 *
 * This class is the internal representation of a loaded O/D-matrix. Beside
 *  being the storage for ODCells, the matrix also contains information about
 *  the numbers of loaded, discarded, and written vehicles.
 *
 * The matrix has a reference to the container of districts stored. This allows
 *  to validate added cell descriptions in means that using existing origins/
 *  destinations only is assured.
 *
 * In addition of being a storage, the matrix is also responsible for writing
 *  the results and contains methods for splitting the entries over time.
 */
class ODMatrix
{
public:
    /** @brief Constructor
     *
     * @param[in] dc The district container to obtain referenced distrivts from
     */
    ODMatrix(const ODDistrictCont &dc) throw();


    /// Destructor
    ~ODMatrix() throw();


    /** @brief Builds a single cell from the given values, verifying them
     *
     * At first, the number of loaded vehicles (myNoLoaded) is incremented
     *  by vehicleNumber.
     *
     * It is checked whether both the origin and the destination exist within
     *  the assigned district container (myDistricts). If one of them is missing,
     *  an error is generated, if both, a warning, because in the later case
     *  the described flow may lay completely beside the processed area. In both
     *  cases the given number of vehicles (vehicleNumber) is added to myNoDiscarded.
     *
     * If the origin/destination districts are known, a cell is built using the
     *  given values. This cell is added to the list of known cells (myContainer).
     *
     * @param[in] vehicleNumber The number of vehicles to store within the cell
     * @param[in] begin The begin of the interval the cell is valid for
     * @param[in] end The end of the interval the cell is valid for
     * @param[in] origin The origin district to use for the cell's flows
     * @param[in] destination The destination district to use for the cell's flows
     * @param[in] vehicleType The vehicle type to use for the cell's flows
     */
    void add(SUMOReal vehicleNumber, SUMOTime begin,
             SUMOTime end, const std::string &origin, const std::string &destination,
             const std::string &vehicleType) throw();


    /** @brief Writes the vehicles stored in the matrix assigning the sources and sinks
     *
     * The cells stored in myContainer are sorted, first. Then, for each time
     *  step to generate vehicles for, it is checked whether the topmost cell
     *  is valid for this time step. If so, vehicles are generated from this
     *  cell's description using computeEmission and stored in an internal vector.
     *  The pointer is moved and the check is repeated until the current cell
     *  is not valid for the current time or no further cells exist.
     *
     * Then, for the current time step, the internal list of vehicles is sorted and
     *  all vehicles that start within this time step are written.
     *
     * The left fraction of vehicles to emit is saved for each O/D-dependency
     *  over time and the number of vehicles to generate is increased as soon
     *  as this value is larger than 1, decrementing it.
     *
     * @param[in] begin The begin time to generate vehicles for
     * @param[in] end The end time to generate vehicles for
     * @param[out] strm The stream to write the generated vehicle trips to
     * @param[in] uniform Information whether departure times shallbe uniformly spread or random
     * @param[in] noVtype Whether vtype information shall not be written
     * @param[in] prefix A prefix for the vehicle names
     */
    void write(SUMOTime begin, SUMOTime end,
               OutputDevice &dev, bool uniform, bool noVtype,
               const std::string &prefix) throw();


    /** @brief Returns the number of loaded vehicles
     *
     * Returns the value of myNoLoaded
     *
     * @return The number of loaded vehicles
     */
    SUMOReal getNoLoaded() const throw();


    /** @brief Returns the number of written vehicles
     *
     * Returns the value of myNoWritten
     *
     * @return The number of written vehicles
     */
    SUMOReal getNoWritten() const throw();


    /** @brief Returns the number of discarded vehicles
     *
     * Returns the value of myNoDiscarded
     *
     * @return The number of discarded vehicles
     */
    SUMOReal getNoDiscarded() const throw();


    /** @brief Splits the stored cells dividing them on the given time line
     * @todo Describe
     */
    void applyCurve(const Distribution_Points &ps) throw();


protected:
    /**
     * @struct ODVehicle
     * @brief An internal representation of a single vehicle
     */
    struct ODVehicle {
        /// @brief The id of the vehicle
        std::string id;
        /// @brief The departure time of the vehicle
        SUMOTime depart;
        /// @brief The type of the vehicle
        std::string type;
        /// @brief The edge the vehicles shall start at
        std::string from;
        /// @brief The edge the vehicles shall end at
        std::string to;

    };


    /// Definition of a container for cells
    typedef std::vector<ODCell*> CellVector;


    /** @brief Computes the emissions stored in the given cell and saves them in "into"
     *
     * At first, the number of vehicles to emit is computed using the
     *  integer value of the vehicleNumber information from the given cell.
     *  In the case vehicleNumber has a fraction, an additional vehicle
     *  may be added in the case a chosen random number is lower than this fraction.
     *
     * If uniform is true, the departure times of the generated vehicles
     *  are spread uniformly, otherwise the departure time are chosen randomly from
     *  the interval.
     *
     * The vehicle names are generated by putting the value of vehName after the
     *  given prefix. The value of vehName is incremented with each generated vehicle.
     *
     * The number of left vehicles (the fraction if no additional vehicle was
     *  generated) is returned.
     *
     * @param[in] cell The cell to use
     * @param[in,out] vehName An incremented index of the generated vehicle
     * @param[out] into The storage to put generated vehicles into
     * @param[in] uniform Information whether departure times shallbe uniformly spread or random
     * @param[in] prefix A prefix for the vehicle names
     * @return The number of left vehicles to emit
     */
    SUMOReal computeEmissions(ODCell *cell,
                              size_t &vehName, std::vector<ODVehicle> &into, bool uniform,
                              const std::string &prefix) throw();


    /** @brief Splits the given cell dividing it on the given time line and
     *          storing the results in the given container
     *
     * For the given cell, a list of clones is generated. The number of these
     *  is equal to the number of "areas" within the given distribution
     *  description (time line in this case) and each clone's vehicleNumber
     *  is equal to the given cell's vehicle number multiplied with the area's
     *  probability. The clones are stored in the given cell vector.
     *
     * @see Distribution_Points
     * @param[in] ps The time line to apply
     * @param[in] cell The cell to split
     * @param[out] newCells The storage to put generated cells into
     * @todo describe better!!!
     */
    void applyCurve(const Distribution_Points &ps, ODCell *cell,
                    CellVector &newCells) throw();


protected:
    /// @brief The loaded cells
    CellVector myContainer;

    /// @brief The districts to retrieve sources/sinks from
    const ODDistrictCont &myDistricts;

    /// @brief Number of loaded vehicles
    SUMOReal myNoLoaded;

    /// @brief Number of written vehicles
    SUMOReal myNoWritten;

    /// @brief Number of discarded vehicles
    SUMOReal myNoDiscarded;


    /**
     * @class cell_by_begin_sorter
     * @brief Used for sorting the cells by the begin time they describe
     */
    class cell_by_begin_sorter
    {
    public:
        /// @brief constructor
        explicit cell_by_begin_sorter() { }


        /** @brief Comparing operator
         *
         * Compares two cells by the begin of the time they describe. The sort is stabilized
         * (with secondary sort keys being origin and destination) to get comparable results
         * with different platforms / compilers.
         *
         * @param[in] p1 First cell to compare
         * @param[in] p2 Second cell to compare
         * @return Whether the begin time of the first cell is lower than the one of the second
         */
        int operator()(ODCell *p1, ODCell *p2) const {
            if (p1->begin == p2->begin) {
                if (p1->origin == p2->origin) {
                    return p1->destination < p2->destination;
                }
                return p1->origin < p2->origin;
            }
            return p1->begin<p2->begin;
        }

    };


    /**
     * @class descending_departure_comperator
     * @brief Used for sorting vehicles by their departure (latest first)
     *
     * A reverse operator to what may be expected is used in order to allow
     *  prunning the sorted vector from its tail.
     */
    class descending_departure_comperator
    {
    public:
        /// @brief constructor
        descending_departure_comperator() { }


        /** @brief Comparing operator
         *
         * Compares two vehicles by their departure time
         *
         * @param[in] p1 First vehicle to compare
         * @param[in] p2 Second vehicle to compare
         * @return Whether the departure time of the first vehicle is larger than the one of the second
         */
        bool operator()(const ODVehicle &p1, const ODVehicle &p2) const {
            if (p1.depart==p2.depart) {
                return p1.id>p2.id;
            }
            return p1.depart>p2.depart;
        }

    };

private:
    /** @brief invalid copy constructor */
    ODMatrix(const ODMatrix &s);

    /** @brief invalid assignment operator */
    ODMatrix &operator=(const ODMatrix &s);

};


#endif

/****************************************************************************/

