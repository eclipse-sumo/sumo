/**
 * \file   vka/Helper.hh
 * \author Kevin Badalian (badalian_k@vka.rwth-aachen.de)
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date   2020-09
 * \brief  This file contains useful helper functions.
 */


#pragma once


#include <vector>
#include <string>




/**
 * \namespace vka
 */
namespace vka
{
  /**
   * \brief Split a string into substrings that are delimited by one or more
   *        special characters.
   *
   * \param ref_string The string that shall be tokenized
   * \param ref_delimiter A string containing all delimiter characters
   * \returns A vector containing the substrings
   */
  std::vector<std::string> tokenize(const std::string& ref_string,
      const std::string& ref_delimiter);

  /**
   * \brief Convert an angle in degrees to radians.
   *
   * \param x An angle in degrees
   * \returns The angle in radians
   */
  double deg2rad(double x);

  /**
   * \brief Convert an angle from radians to degrees.
   *
   * \param x An angle in radians
   * \returns The angle in degrees
   */
  double rad2deg(double x);
}

