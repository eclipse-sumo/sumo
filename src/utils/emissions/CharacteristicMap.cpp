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
/// @file    CharacteristicMap.cpp
/// @author  Kevin Badalian (badalian_k@mmp.rwth-aachen.de)
/// @date    2021-02
///
// Characteristic map for vehicle type parameters as needed by the MMPEVEM model
// Teaching and Research Area Mechatronics in Mobile Propulsion (MMP), RWTH Aachen
/****************************************************************************/
#include <config.h>

#include <cmath>
#include <cstring>
#include <stdexcept>

#include <utils/common/StringTokenizer.h>
#include <utils/emissions/CharacteristicMap.h>


// ===========================================================================
// method definitions
// ===========================================================================
void
CharacteristicMap::determineStrides() {
    strides.clear();
    strides.reserve(domainDim);
    strides.push_back(imageDim);
    for (int i = 1; i < domainDim; i++) {
        strides.push_back((int)axes[i - 1].size() * strides[i - 1]);
    }
}


int
CharacteristicMap::calcFlatIdx(const std::vector<int>& ref_idxs) const {
    if (static_cast<int>(ref_idxs.size()) != domainDim) {
        throw std::runtime_error("The number of indices differs from the map's"
                                 " domain dimension.");
    }

    int flatIdx = 0;
    for (int i = 0; i < domainDim; i++) {
        if (ref_idxs[i] < 0) {
            throw std::runtime_error("The argument indices aren't non-negative.");
        }
        flatIdx += ref_idxs[i] * strides[i];
    }
    return flatIdx;
}


int
CharacteristicMap::findNearestNeighborIdxs(const std::vector<double>& ref_p,
        std::vector<int>& ref_idxs, double eps) const {
    if (static_cast<int>(ref_p.size()) != domainDim) {
        throw std::runtime_error("The argument point's size doesn't match the"
                                 " domain dimension.");
    }

    ref_idxs = std::vector<int>(domainDim, -1);
    for (int i = 0; i < domainDim; i++) {
        if (axes[i][0] - eps <= ref_p[i] && ref_p[i] < axes[i][0]) {
            ref_idxs[i] = 0;
        } else if (axes[i][axes[i].size() - 1] <= ref_p[i]
                   && ref_p[i] < axes[i][axes[i].size() - 1] + eps) {
            ref_idxs[i] = (int)axes[i].size() - 1;
        } else {
            for (int j = 0; j < static_cast<int>(axes[i].size()) - 1; j++) {
                if (axes[i][j] <= ref_p[i] && ref_p[i] < axes[i][j + 1]) {
                    // Pick the index that is closest to the point
                    if (ref_p[i] - axes[i][j] <= axes[i][j + 1] - ref_p[i]) {
                        ref_idxs[i] = j;
                        break;
                    } else {
                        ref_idxs[i] = j + 1;
                        break;
                    }
                }
            }
        }

        // The point lies outside of the valid range
        if (ref_idxs[i] == -1) {
            return -1;
        }
    }

    return 0;
}


std::vector<double>
CharacteristicMap::at(const std::vector<int>& ref_idxs) const {
    if (static_cast<int>(ref_idxs.size()) != domainDim) {
        throw std::runtime_error("The number of indices differs from the map's"
                                 " domain dimension.");
    }

    int flatIdx = calcFlatIdx(ref_idxs);
    return std::vector<double>(flattenedMap.begin() + flatIdx,
                               flattenedMap.begin() + flatIdx + imageDim);
}


CharacteristicMap::CharacteristicMap(int domainDim, int imageDim,
                                     const std::vector<std::vector<double>>& ref_axes,
                                     const std::vector<double>& ref_flattenedMap)
    : domainDim(domainDim),
      imageDim(imageDim),
      axes(ref_axes),
      flattenedMap(ref_flattenedMap) {
    // Check whether the dimensions are consistent
    if (static_cast<int>(axes.size()) != domainDim) {
        throw std::runtime_error("The number of axes doesn't match the specified"
                                 " domain dimension.");
    }
    int expectedEntryCnt = imageDim;
    for (auto& ref_axis : axes) {
        expectedEntryCnt *= (int)ref_axis.size();
    }
    if (static_cast<int>(flattenedMap.size()) != expectedEntryCnt) {
        throw std::runtime_error("The number of map entries isn't equal to the"
                                 " product of the axes' dimensions times the image dimension.");
    }

    determineStrides();
}


CharacteristicMap::CharacteristicMap(const std::string& ref_mapString) {
    // Split the map string into its three main parts
    const std::vector<std::string> tokens = StringTokenizer(ref_mapString, "|").getVector();
    if (tokens.size() != 3) {
        throw std::runtime_error("The map string isn't made up of the 3 parts"
                                 " dimensions, axes, and flattened entries.");
    }

    // Extract the domain and image dimensions
    const std::vector<std::string> dimensionTokens = StringTokenizer(tokens[0], ",").getVector();
    if (dimensionTokens.size() != 2) {
        throw std::runtime_error("The domain and image dimensions aren't specified"
                                 " correctly.");
    }
    domainDim = std::stoi(dimensionTokens[0]);
    imageDim = std::stoi(dimensionTokens[1]);

    // Create the map axes
    const std::vector<std::string> axisTokens = StringTokenizer(tokens[1], ";").getVector();
    if (static_cast<int>(axisTokens.size()) != domainDim) {
        throw std::runtime_error("The number of axes doesn't match the specified"
                                 " domain dimension.");
    }
    for (auto& ref_axisToken : axisTokens) {
        std::vector<std::string> axisEntryTokens = StringTokenizer(ref_axisToken, ",").getVector();
        std::vector<double> axisEntries;
        for (auto& ref_axisEntryToken : axisEntryTokens) {
            axisEntries.push_back(std::stod(ref_axisEntryToken));
        }
        axes.push_back(axisEntries);
    }

    // Create the flattened map
    const std::vector<std::string> flattenedMapTokens = StringTokenizer(tokens[2], ",").getVector();
    int expectedEntryCnt = imageDim;
    for (auto& ref_axis : axes) {
        expectedEntryCnt *= (int)ref_axis.size();
    }
    if (static_cast<int>(flattenedMapTokens.size()) != expectedEntryCnt) {
        throw std::runtime_error("The number of map entries isn't equal to the"
                                 " product of the axes' dimensions times the image dimension.");
    }
    flattenedMap.reserve(expectedEntryCnt);
    for (auto& ref_flattenedMapToken : flattenedMapTokens) {
        flattenedMap.push_back(std::stod(ref_flattenedMapToken));
    }

    determineStrides();
}


std::string
CharacteristicMap::toString() const {
    // Write the domain and image dimensions
    std::string mapString = std::to_string(domainDim) + ","
                            + std::to_string(imageDim) + "|";

    // Add the axes
    for (int i = 0; i < static_cast<int>(axes.size()); i++) {
        for (int j = 0; j < static_cast<int>(axes[i].size()); j++) {
            mapString += std::to_string(axes[i][j])
                         + (j == static_cast<int>(axes[i].size()) - 1 ? "" : ",");
        }
        mapString += (i == static_cast<int>(axes.size()) - 1 ? "|" : ";");
    }

    // Append the flattened map entries
    for (int i = 0; i < static_cast<int>(flattenedMap.size()); i++) {
        mapString += std::to_string(flattenedMap[i])
                     + (i == static_cast<int>(flattenedMap.size()) - 1 ? "" : ",");
    }

    return mapString;
}


int
CharacteristicMap::getDomainDim() const {
    return domainDim;
}


int
CharacteristicMap::getImageDim() const {
    return imageDim;
}


std::vector<double>
CharacteristicMap::eval(const std::vector<double>& ref_p, double eps) const {
    if (static_cast<int>(ref_p.size()) != domainDim) {
        throw std::runtime_error("The argument's size doesn't match the domain"
                                 " dimension.");
    }

    // Find the nearest neighbor and its image values
    std::vector<int> nnIdxs;
    if (findNearestNeighborIdxs(ref_p, nnIdxs, eps)) {
        return std::vector<double>(imageDim, std::stod("nan"));
    }
    // Image values of the nearest neighbor
    const std::vector<double> y_nn = at(nnIdxs);
    // The result is based on the image values of the nearest neighbor
    std::vector<double> y = y_nn;

    // Interpolate
    for (int i = 0; i < domainDim; i++) {
        // Depending on the configuration of the points, different neighbors will be
        // used for interpolation
        const double s = ref_p[i] - axes[i][nnIdxs[i]];
        if (std::abs(s) <= eps) {
            continue;
        }
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
        if (b_constellation1 || b_constellation2) {
            anIdxs[i] -= 1;
            dx = axes[i][nnIdxs[i]] - axes[i][anIdxs[i]];
        } else if (b_constellation3 || b_constellation4) {
            anIdxs[i] += 1;
            dx = axes[i][anIdxs[i]] - axes[i][nnIdxs[i]];
        } else {
            continue;
        }
        // Image values of the axis neighbor
        const std::vector<double> y_an = at(anIdxs);

        for (int j = 0; j < imageDim; j++) {
            double dy = 0;
            if (b_constellation1 || b_constellation2) {
                dy = y_nn[j] - y_an[j];
            } else {
                dy = y_an[j] - y_nn[j];
            }

            // Update
            y[j] += s * dy / dx;
        }
    }

    return y;
}
