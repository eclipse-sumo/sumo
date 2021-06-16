/**
 * \file vka/CharacteristicMap.cc
 * \author Kevin Badalian (badalian_k@vka.rwth-aachen.de)
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date 2021-02
 * \brief This file contains the CharacteristicMap class.
 */


/*****************************************************************************
 * ============================= Example Usage ============================= *
 *****************************************************************************
 *                                                                           *
 * Assume a function f which maps from R^2 to R^1 according to...            *
 *                                                                           *
 *     |  0 |  1 |  2 |  3 |  4   -> x_1                                     *
 * ----|------------------------                                             *
 *  -1 |  1 |  3 |  1 | -2 |                                                 *
 *   1 |  1 | -2 | -3 |  7 |  5                                              *
 *   3 | -2 | -1 |  0 |  1 |  3                                              *
 *   5 |    |  0 |  8 |  4 |  4                                              *
 *                                                                           *
 *   |                                                                       *
 *   v                                                                       *
 *  x_2                                                                      *
 *                                                                           *
 * ... so that, for example, f(3, 1) = 7. Note that f is not defined at      *
 * (4, -1) and (0, 5). There are two ways to create a CharacteristicMap      *
 * object for this function.                                                 *
 * 1) Using the standard constructor:                                        *
 *      // Axes                                                              *
 *      std::vector<std::vector<double>> axes;                               *
 *      axes.push_back(std::vector<double>{0, 1, 2, 3, 4});  // Axis 1       *
 *      axes.push_back(std::vector<double>{-1, 1, 3, 5});    // Axis 2       *
 *      // Flattened row-major map entries                                   *
 *      std::vector<double> flattenedMap{1, 3, 1, -2, std::nan(""),          *
 *          1, -2, -3, 7, 5, -2, -1, 0, 1, 3, std::nan(""), 0, 8, 4, 4};     *
 *                                                                           *
 *      vka::CharacteristicMap map1(2,              // Mapping from R^2...   *
 *                                  1,              // ... to R^1            *
 *                                  axes,                                    *
 *                                  flattenedMap);                           *
 *                                                                           *
 * 2) Using a string-encoding of the map:                                    *
 *      vka::CharacteristicMap map2("2,1|0,1,2,3,4;-1,1,3,5|1,3,1,-2,nan,"   *
 *          "1,-2,-3,7,5,-2,-1,0,1,3,nan,0,8,4,4");                          *
 *                                                                           *
 *    See below for an in-depth explanation of the format.                   *
 *                                                                           *
 *                                                                           *
 * To evaluate the map at, for instance, p = (2.2, 2), one must call:        *
 *   std::vector<double> res = map1.eval(std::vector<double>{2.2, 2},  // p  *
 *                                       1e-3);  // eps                      *
 *   if(std::isnan(res[0]))                                                  *
 *     std::cout << "[WARNING] Couldn't evaluate the map." << std::endl;     *
 *   else                                                                    *
 *     std::cout << "res = " << res[0] << std::endl;                         *
 *                                                                           *
 * The epsilon value is used for numerical reasons and decides how much a    *
 * point must deviate from its nearest neighbor before linear interpolation  *
 * is applied or when a point is considered outside of the map. The default  *
 * is 1e-6.                                                                  *
 *                                                                           *
 *                                                                           *
 * The string-encoding of a CharacteristicMap that maps from R^m to R^n is   *
 * formally defined as                                                       *
 *   "m,n|A_1[1],A_1[2],...,A_1[l1];A_2[1],A_2[2],...,A_2[l2];...;\          *
 *       A_m[1],A_m[2],...,A_m[lm]|\                                         *
 *       M_flat[1],M_flat[2],...,M_flat[l1*l2*...*lm]"                       *
 * where A_i[j] denotes the j-th value of the i-th axis (which has li values *
 * total) and M_flat[i] stands for the i-th entry in the row-major flattened *
 * map. To be more specific, given a map M, its flattened version is         *
 * computed as follows (using pseudo code):                                  *
 *   M_flat = ""                                                             *
 *   for i_m in {1,2,...,lm}:        // Last axis                            *
 *     ...                                                                   *
 *       for i_2 in {1,2,...,l2}:    // Second axis                          *
 *         for i_1 in {1,2,...,l1}:  // First axis (i.e. row axis)           *
 *           for d in {1,2,...,n}:   // Image dimensions                     *
 *             M_flat += M[i_1,i_2,...,i_m][d] + ","                         *
 *   removeTrailingComma(M_flat)                                             *
 *                                                                           *
 *****************************************************************************/


#include <vka/CharacteristicMap.hh>
#include <vka/Helper.hh>

#include <cmath>
#include <stdexcept>




/**
 * \namespace vka
 */
namespace vka
{
  /**
   * \brief Determine the stride for each map dimension in the flattened map.
   */
  void CharacteristicMap::determineStrides()
  {
    strides.clear();
    strides.reserve(domainDim);
    strides.push_back(1*imageDim);
    for(int i = 1; i < domainDim; i++)
      strides.push_back(axes[i - 1].size()*strides[i - 1]);
  }



  /**
   * \brief Compute the index of a map entry in the flattened map.
   *
   * \param[in] ref_idxs Non-flattened map indices
   * \returns Flattened map index
   * \throws std::runtime_error
   */
  int CharacteristicMap::calcFlatIdx(const std::vector<int>& ref_idxs) const
  {
    if(static_cast<int>(ref_idxs.size()) != domainDim)
    {
      throw std::runtime_error("The number of indices differs from the map's"
          " domain dimension.");
    }

    int flatIdx = 0;
    for(int i = 0; i < domainDim; i++)
    {
      if(ref_idxs[i] < 0)
        throw std::runtime_error("The argument indices aren't non-negative.");
      flatIdx += ref_idxs[i]*strides[i];
    }
    return flatIdx;
  }



  /**
   * \brief Determine the indices of the nearest neighbor of a point in the
   *        map.
   *
   * A point has no such neighbor if it lies outside of the range of an axis
   * with respect to some epsilon.
   * \param[in] ref_p A point
   * \param[out] ref_idxs A vector into which the indices shall be written
   * \param[in] eps An epsilon value
   * \returns 0 if a nearest neighbor could be found, else -1
   * \throws std::runtime_error
   */
  int CharacteristicMap::findNearestNeighborIdxs(
      const std::vector<double>& ref_p, std::vector<int>& ref_idxs,
      double eps) const
  {
    if(static_cast<int>(ref_p.size()) != domainDim)
    {
      throw std::runtime_error("The argument point's size doesn't match the"
          " domain dimension.");
    }

    ref_idxs = std::vector<int>(domainDim, -1);
    for(int i = 0; i < domainDim; i++)
    {
      if(axes[i][0] - eps <= ref_p[i] && ref_p[i] < axes[i][0])
        ref_idxs[i] = 0;
      else if(axes[i][axes[i].size() - 1] <= ref_p[i]
          && ref_p[i] < axes[i][axes[i].size() - 1] + eps)
      {
        ref_idxs[i] = axes[i].size() - 1;
      }
      else
      {
        for(int j = 0; j < static_cast<int>(axes[i].size()) - 1; j++)
        {
          if(axes[i][j] <= ref_p[i] && ref_p[i] < axes[i][j + 1])
          {
            // Pick the index that is closest to the point
            if(ref_p[i] - axes[i][j] <= axes[i][j + 1] - ref_p[i])
            {
              ref_idxs[i] = j;
              break;
            }
            else
            {
              ref_idxs[i] = j + 1;
              break;
            }
          }
        }
      }

      // The point lies outside of the valid range
      if(ref_idxs[i] == -1)
        return -1;
    }

    return 0;
  }



  /**
   * \brief Access a map entry using its indices.
   *
   * \param[in] ref_idxs A vector containing indices
   * \returns A vector containing the image values of the map at the specified
   *          location
   * \throws std::runtime_error
   */
  std::vector<double> CharacteristicMap::at(
      const std::vector<int>& ref_idxs) const
  {
    if(static_cast<int>(ref_idxs.size()) != domainDim)
    {
      throw std::runtime_error("The number of indices differs from the map's"
          " domain dimension.");
    }
    
    int flatIdx = calcFlatIdx(ref_idxs);
    return std::vector<double>(flattenedMap.begin() + flatIdx,
        flattenedMap.begin() + flatIdx + imageDim);
  }



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
  CharacteristicMap::CharacteristicMap(int domainDim, int imageDim,
      const std::vector<std::vector<double>>& ref_axes,
      const std::vector<double>& ref_flattenedMap)
   : domainDim(domainDim),
     imageDim(imageDim),
     axes(ref_axes),
     flattenedMap(ref_flattenedMap)
  {
    // Check whether the dimensions are consistent
    if(static_cast<int>(axes.size()) != domainDim)
    {
      throw std::runtime_error("The number of axes doesn't match the specified"
          " domain dimension.");
    }
    int expectedEntryCnt = 1*imageDim;
    for(auto& ref_axis : axes)
      expectedEntryCnt *= ref_axis.size();
    if(static_cast<int>(flattenedMap.size()) != expectedEntryCnt)
    {
      throw std::runtime_error("The number of map entries isn't equal to the"
          " product of the axes' dimensions times the image dimension.");
    }
    
    determineStrides();
  }



  /**
   * \brief Constructor
   *
   * \param[in] ref_mapString A string representation of a characteristic map
   *            (cf. example at the top of the file)
   * throws std::runtime_error
   */
  CharacteristicMap::CharacteristicMap(const std::string& ref_mapString)
  {
    // Split the map string into its three main parts
    std::vector<std::string> tokens = tokenize(ref_mapString, "|");
    if(tokens.size() != 3)
    {
      throw std::runtime_error("The map string isn't made up of the 3 parts"
          " dimensions, axes, and flattened entries.");
    }

    // Extract the domain and image dimensions
    std::vector<std::string> dimensionTokens = tokenize(tokens[0], ",");
    if(dimensionTokens.size() != 2)
    {
      throw std::runtime_error("The domain and image dimensions aren't"
          " specified correctly.");
    }
    domainDim = std::stoi(dimensionTokens[0]);
    imageDim = std::stoi(dimensionTokens[1]);

    // Create the map axes
    std::vector<std::string> axisTokens = tokenize(tokens[1], ";");
    if(static_cast<int>(axisTokens.size()) != domainDim)
    {
      throw std::runtime_error("The number of axes doesn't match the specified"
          " domain dimension.");
    }
    for(auto& ref_axisToken : axisTokens)
    {
      std::vector<std::string> axisEntryTokens = tokenize(ref_axisToken, ",");
      std::vector<double> axisEntries;
      for(auto& ref_axisEntryToken : axisEntryTokens)
        axisEntries.push_back(std::stod(ref_axisEntryToken));
      axes.push_back(axisEntries);
    }

    // Create the flattened map
    std::vector<std::string> flattenedMapTokens = tokenize(tokens[2], ",");
    int expectedEntryCnt = 1*imageDim;
    for(auto& ref_axis : axes)
      expectedEntryCnt *= ref_axis.size();
    if(static_cast<int>(flattenedMapTokens.size()) != expectedEntryCnt)
    {
      throw std::runtime_error("The number of map entries isn't equal to the"
          " product of the axes' dimensions times the image dimension.");
    }
    flattenedMap.reserve(expectedEntryCnt);
    for(auto& ref_flattenedMapToken : flattenedMapTokens)
      flattenedMap.push_back(std::stod(ref_flattenedMapToken));

    determineStrides();
  }



  /**
   * \brief Encode the map as a string.
   *
   * \returns A string representation of the characteristic map (cf. example at
   *          the top of the file)
   */
  std::string CharacteristicMap::toString() const
  {
    // Write the domain and image dimensions
    std::string mapString = std::to_string(domainDim) + ","
        + std::to_string(imageDim) + "|";

    // Add the axes
    for(int i = 0; i < static_cast<int>(axes.size()); i++)
    {
      for(int j = 0; j < static_cast<int>(axes[i].size()); j++)
      {
        mapString += std::to_string(axes[i][j])
            + (j == static_cast<int>(axes[i].size()) - 1 ? "" : ",");
      }
      mapString += (i == static_cast<int>(axes.size()) - 1 ? "|" : ";");
    }

    // Append the flattened map entries
    for(int i = 0; i < static_cast<int>(flattenedMap.size()); i++)
    {
      mapString += std::to_string(flattenedMap[i])
          + (i == static_cast<int>(flattenedMap.size()) - 1 ? "" : ",");
    }

    return mapString;
  }



  /**
   * \brief Get the dimension of the map's domain.
   *
   * \returns The domain's dimension
   */
  int CharacteristicMap::getDomainDim() const
  {
    return domainDim;
  }



  /**
   * \brief Get the image dimension of the map.
   *
   * \returns The image dimension of the characteristic map
   */
  int CharacteristicMap::getImageDim() const
  {
    return imageDim;
  }



  /**
   * \brief Evaluate a point in the map using linear interpolation.
   *
   * Please note that the result may contain NaNs. That happens when...
   * a) ... the point in question has no nearest neighbor (that is, it lies
   *    outside of the domain). In that case, all entries of the vector are set
   *    to NaN.
   * b) ... an image value of the nearest neighbor is NaN. Then, the result is
   *    also NaN in that image dimension.
   * c) ... evaluation would require interpolating with a support point which
   *    is NaN in an image dimension. The corresponding result entry is set to
   *    NaN in that case.
   * \param[in] ref_p A point
   * \param[in] eps An epsilon value
   * \returns The (interpolated) image values of the map at the specified point
   * \throws std::runtime_error
   */
  std::vector<double> CharacteristicMap::eval(const std::vector<double>& ref_p,
      double eps) const
  {
    if(static_cast<int>(ref_p.size()) != domainDim)
    {
      throw std::runtime_error("The argument's size doesn't match the domain"
          " dimension.");
    }

    // Find the nearest neighbor and its image values
    std::vector<int> nnIdxs;
    if(findNearestNeighborIdxs(ref_p, nnIdxs, eps))
      return std::vector<double>(imageDim, std::stod("nan"));
    // Image values of the nearest neighbor
    const std::vector<double> y_nn = at(nnIdxs);
    // The result is based on the image values of the nearest neighbor
    std::vector<double> y = y_nn;

    // Interpolate
    for(int i = 0; i < domainDim; i++)
    {
      // Depending on the configuration of the points, different neighbors will
      // be used for interpolation
      const double s = ref_p[i] - axes[i][nnIdxs[i]];
      if(std::abs(s) <= eps)
        continue;
      bool b_constellation1 = s < 0 && nnIdxs[i] > 0;
      bool b_constellation2 = s >= 0
          && nnIdxs[i] == static_cast<int>(axes[i].size()) - 1
          && nnIdxs[i] > 0;
      bool b_constellation3 = s < 0 && nnIdxs[i] == 0
          && nnIdxs[i] < static_cast<int>(axes[i].size()) - 1;
      bool b_constellation4 = s >= 0
          && nnIdxs[i] < static_cast<int>(axes[i].size()) - 1;

      double dx = 1;
      // Axis neighbor indices (i.e. the indices of the second support point)
      std::vector<int> anIdxs = nnIdxs;
      if(b_constellation1 || b_constellation2)
      {
        anIdxs[i] -= 1;
        dx = axes[i][nnIdxs[i]] - axes[i][anIdxs[i]];
      }
      else if(b_constellation3 || b_constellation4)
      {
        anIdxs[i] += 1;
        dx = axes[i][anIdxs[i]] - axes[i][nnIdxs[i]];
      }
      else
        continue;
      // Image values of the axis neighbor
      const std::vector<double> y_an = at(anIdxs);

      for(int j = 0; j < imageDim; j++)
      {
        double dy = 0;
        if(b_constellation1 || b_constellation2)
          dy = y_nn[j] - y_an[j];
        else
          dy = y_an[j] - y_nn[j];

        // Update
        y[j] += s*dy/dx;
      }
    }

    return y;
  }
}

