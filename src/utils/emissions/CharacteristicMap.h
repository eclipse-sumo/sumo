/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    CharacteristicMap.h
/// @author  Kevin Badalian (badalian_k@mmp.rwth-aachen.de)
/// @date    2021-02
///
// Characteristic map for vehicle type parameters as needed by the MMPEVEM model
// Teaching and Research Area Mechatronics in Mobile Propulsion (MMP), RWTH Aachen
/****************************************************************************/


/******************************************************************************
 * ============================= Example Usage ============================== *
 ******************************************************************************
 *                                                                            *
 * Assume a function f which maps from R^2 to R^1 according to...             *
 *                                                                            *
 *     |  0 |  1 |  2 |  3 |  4   -> x_1                                      *
 * ----|------------------------                                              *
 *  -1 |  1 |  3 |  1 | -2 |                                                  *
 *   1 |  1 | -2 | -3 |  7 |  5                                               *
 *   3 | -2 | -1 |  0 |  1 |  3                                               *
 *   5 |    |  0 |  8 |  4 |  4                                               *
 *                                                                            *
 *   |                                                                        *
 *   v                                                                        *
 *  x_2                                                                       *
 *                                                                            *
 * ... so that, for example, f(3, 1) = 7. Note that f is not defined at       *
 * (4, -1) and (0, 5). There are two ways to create a CharacteristicMap       *
 * object for this function.                                                  *
 * 1) Using the standard constructor:                                         *
 *      // Axes                                                               *
 *      std::vector<std::vector<double>> axes;                                *
 *      axes.push_back(std::vector<double>{0, 1, 2, 3, 4});  // Axis 1        *
 *      axes.push_back(std::vector<double>{-1, 1, 3, 5});    // Axis 2        *
 *      // Flattened row-major map entries                                    *
 *      std::vector<double> flattenedMap{1, 3, 1, -2, std::nan(""),           *
 *          1, -2, -3, 7, 5, -2, -1, 0, 1, 3, std::nan(""), 0, 8, 4, 4};      *
 *                                                                            *
 *      CharacteristicMap map1(2,              // Mapping from R^2...         *
 *                             1,              // ... to R^1                  *
 *                             axes,                                          *
 *                             flattenedMap);                                 *
 *                                                                            *
 * 2) Using a string-encoding of the map:                                     *
 *      CharacteristicMap map2("2,1|0,1,2,3,4;-1,1,3,5|1,3,1,-2,nan,"         *
 *          "1,-2,-3,7,5,-2,-1,0,1,3,nan,0,8,4,4");                           *
 *                                                                            *
 *    See below for an in-depth explanation of the format.                    *
 *                                                                            *
 *                                                                            *
 * To evaluate the map at, for instance, p = (2.2, 2), one must call:         *
 *   std::vector<double> res = map1.eval(std::vector<double>{2.2, 2},  // p   *
 *                                       1e-3);  // eps                       *
 *   if(std::isnan(res[0]))                                                   *
 *     std::cout << "[WARNING] Couldn't evaluate the map." << std::endl;      *
 *   else                                                                     *
 *     std::cout << "res = " << res[0] << std::endl;                          *
 *                                                                            *
 * The epsilon value is used for numerical reasons and decides how much a     *
 * point must deviate from its nearest neighbor before linear interpolation   *
 * is applied or when a point is considered outside of the map. The default   *
 * is 1e-6.                                                                   *
 *                                                                            *
 *                                                                            *
 * The string-encoding of a CharacteristicMap that maps from R^m to R^n is    *
 * formally defined as                                                        *
 *   "m,n|A_1[1],A_1[2],...,A_1[l1];A_2[1],A_2[2],...,A_2[l2];...;\           *
 *       A_m[1],A_m[2],...,A_m[lm]|\                                          *
 *       M_flat[1],M_flat[2],...,M_flat[l1*l2*...*lm]"                        *
 * where A_i[j] denotes the j-th value of the i-th axis (which has li values  *
 * total) and M_flat[i] stands for the i-th entry in the row-major flattened  *
 * map. To be more specific, given a map M, its flattened version is          *
 * computed as follows (using pseudo code):                                   *
 *   M_flat = ""                                                              *
 *   for i_m in {1,2,...,lm}:        // Last axis                             *
 *     ...                                                                    *
 *       for i_2 in {1,2,...,l2}:    // Second axis                           *
 *         for i_1 in {1,2,...,l1}:  // First axis (i.e. row axis)            *
 *           for d in {1,2,...,n}:   // Image dimensions                      *
 *             M_flat += M[i_1,i_2,...,i_m][d] + ","                          *
 *   removeTrailingComma(M_flat)                                              *
 *                                                                            *
 ******************************************************************************/
#pragma once

#include <vector>
#include <string>


/**
 * \class CharacteristicMap
 * \brief The purpose of this class is to store a characteristic map (German:
 *        Kennfeld) of arbitrary dimensions and to provide functions in order to
 *        evaluate/interpolate points in it.
 */
class CharacteristicMap {
private:
    /// Dimension of the map's domain
    int domainDim;

    /// Image dimension of the map
    int imageDim;

    /// Vector containing the values along each domain axis in ascending order
    std::vector<std::vector<double>> axes;

    /// Flattened map entries
    std::vector<double> flattenedMap;

    /// Stride for each domain dimension in the flattened map
    std::vector<int> strides;

    /**
     * \brief Determine the stride for each map dimension in the flattened map.
     */
    void determineStrides();

    /**
     * \brief Compute the index of a map entry in the flattened map.
     *
     * \param[in] ref_idxs Non-flattened map indices
     * \returns Flattened map index
     * \throws std::runtime_error
     */
    int calcFlatIdx(const std::vector<int>& ref_idxs) const;

    /**
     * \brief Determine the indices of the nearest neighbor of a point in the map.
     *
     * A point has no such neighbor if it lies outside of the range of an axis
     * with respect to some epsilon.
     * \param[in] ref_p A point
     * \param[out] ref_idxs A vector into which the indices shall be written
     * \param[in] eps An epsilon value
     * \returns 0 if a nearest neighbor could be found, else -1
     * \throws std::runtime_error
     */
    int findNearestNeighborIdxs(const std::vector<double>& ref_p,
                                std::vector<int>& ref_idxs, double eps = 1e-6) const;

    /**
     * \brief Access a map entry using its indices.
     *
     * \param[in] ref_idxs A vector containing indices
     * \returns A vector containing the image values of the map at the specified
     *          location
     * \throws std::runtime_error
     */
    std::vector<double> at(const std::vector<int>& ref_idxs) const;



public:
    /**
     * \brief Constructor
     *
     * \param[in] domainDim The map's domain dimension
     * \param[in] imageDim The map's image dimension
     * \param[in] ref_axes A vector of vectors containing the entries of their
     *            respective axes in ascending order
     * \param[in] ref_flattenedMap The row-major flattened entries of the map
     *            (i.e. the map is flattened along its first axis)
     * \throws std::runtime_error
     */
    CharacteristicMap(int domainDim, int imageDim,
                      const std::vector<std::vector<double>>& ref_axes,
                      const std::vector<double>& ref_flattenedMap);

    /**
     * \brief Constructor
     *
     * \param[in] ref_mapString A string representation of a characteristic map
     *            (cf. example at the top of the file)
     * throws std::runtime_error
     */
    CharacteristicMap(const std::string& ref_mapString);

    /**
     * \brief Encode the map as a string.
     *
     * \returns A string representation of the characteristic map (cf. example at
     *          the top of the file)
     */
    std::string toString() const;

    /**
     * \brief Get the dimension of the map's domain.
     *
     * \returns The domain's dimension
     */
    int getDomainDim() const;

    /**
     * \brief Get the image dimension of the map.
     *
     * \returns The image dimension of the characteristic map
     */
    int getImageDim() const;

    /**
     * \brief Evaluate a point in the map using linear interpolation.
     *
     * Please note that the result may contain NaNs. That happens when...
     * a) ... the point in question has no nearest neighbor (that is, it lies
     *    outside of the domain). In that case, all entries of the vector are set
     *    to NaN.
     * b) ... an image value of the nearest neighbor is NaN. Then, the result is
     *    also NaN in that image dimension.
     * c) ... evaluation would require interpolating with a support point which is
     *    NaN in an image dimension. The corresponding result entry is set to NaN
     *    in that case.
     * \param[in] ref_p A point
     * \param[in] eps An epsilon value
     * \returns The (interpolated) image values of the map at the specified point
     * \throws std::runtime_error
     */
    std::vector<double> eval(const std::vector<double>& ref_p,
                             double eps = 1e-6) const;
};
