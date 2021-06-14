/**
 * \file   vka/Helper.cc
 * \author Kevin Badalian (badalian_k@vka.rwth-aachen.de)
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date   2020-09
 * \brief  This file contains useful helper functions.
 */


#include <cstring>

#define _USE_MATH_DEFINES
#include <cmath>

#include <vka/Helper.hh>




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
      const std::string& ref_delimiter)
  {
    std::vector<std::string> tokens;

    char* ptr_buffer = new char[ref_string.length() + 1];
    std::strcpy(ptr_buffer, ref_string.c_str());

    char* ptr_token = std::strtok(ptr_buffer, ref_delimiter.c_str());
    while(ptr_token != nullptr)
    {
      tokens.push_back(std::string(ptr_token));
      ptr_token = std::strtok(nullptr, ref_delimiter.c_str());
    }

    delete[] ptr_buffer;
    ptr_buffer = nullptr;
    return tokens;
  }



  /**
   * \brief Convert an angle in degrees to radians.
   *
   * \param x An angle in degrees
   * \returns The angle in radians
   */
  double deg2rad(double x)
  {
    return M_PI*x/180.0;
  }



  /**
   * \brief Convert an angle from radians to degrees.
   *
   * \param x An angle in radians
   * \returns The angle in degrees
   */
  double rad2deg(double x)
  {
    return 180.0*x/M_PI;
  }
}

