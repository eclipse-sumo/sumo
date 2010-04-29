/****************************************************************************/
/// @file    RGBColor.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A RGB-color definition
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RGBColor_h
#define RGBColor_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RGBColor
 * The definition of a color in the RGB-space.
 * The cube is meant to lie between (0, 0, 0) and (1, 1, 1)
 */
class RGBColor {
public:
    /** @brief Constructor
     */
    RGBColor() throw();


    /** @brief Constructor
     * @param[in] red The red component's value
     * @param[in] green The green component's value
     * @param[in] blue The blue component's value
     */
    RGBColor(SUMOReal red, SUMOReal green, SUMOReal blue) throw();


    /** @brief Copy constructor
     */
    RGBColor(const RGBColor &col) throw();


    /// @brief Destructor
    ~RGBColor() throw();



    /** @brief Returns the red-amount of the color
     * @return The red component's value
     */
    SUMOReal red() const throw() {
        return myRed;
    }


    /** @brief Returns the green-amount of the color
     * @return The red component's value
     */
    SUMOReal green() const throw() {
        return myGreen;
    }


    /** @brief Returns the blue-amount of the color
     * @return The red component's value
     */
    SUMOReal blue() const throw() {
        return myBlue;
    }


    /** @brief assigns new values
     * @param[in] r The red component's value
     * @param[in] g The green component's value
     * @param[in] b The blue component's value
     */
    void set(SUMOReal r, SUMOReal g, SUMOReal b) throw();


    /** @brief Writes the color to the given stream
     * @param[out] os The stream to write to
     * @param[in] col The color to write
     * @return The stream
     */
    friend std::ostream &operator<<(std::ostream &os, const RGBColor &col);


    bool operator==(const RGBColor &c) const;
    bool operator!=(const RGBColor &c) const;


    /** @brief Parses a color information
     *
     * It is assumed that the color is stored as "<RED>,<GREEN>,<BLUE>"
     * And each color is represented as a SUMOReal.
     * @param[in] coldef The color definition to parse
     * @return The parsed color
     * @exception EmptyData If the definition has less than three entries
     * @exception NumberFormatException If one of the components is not numeric
     */
    static RGBColor parseColor(const std::string &coldef) throw(EmptyData, NumberFormatException);


    /** @brief Parses a color information
     *
     * It is assumed that the color is stored as "<RED>,<GREEN>,<BLUE>"
     * And each color is represented as a SUMOReal.
     * @param[in] coldef The color definition to parse
     * @param[in] objecttype The type of the currently parsed object
     * @param[in] objectid The id of the currently parsed object
     * @param[in] report Whether errors shall be reported
     * @param[in, out] ok Whether parsing was successful
     * @return The parsed color
     * @exception EmptyData If the definition has less than three entries
     * @exception NumberFormatException If one of the components is not numeric
     */
    static RGBColor parseColorReporting(const std::string &coldef, const char *objecttype, const char *objectid, bool report, bool &ok) throw();


    /** @brief Returns the default color by parsing DEFAULT_COLOR_STRING
     * @return The default color
     */
    static RGBColor getDefaultColor() throw();


    /** @brief Interpolates between two colors
     *
     * The interpolated color is calculated as a weighted average of
     *  the RGB values of minColor and maxColor, giving weight to maxColor
     *  and 1-weight to minColor.
     * @param[in] minColor The color to interpolate from
     * @param[in] maxColor The color to interpolate to
     * @param[in] weight The weight of the first color
     * @return The interpolated color
     */
    static RGBColor interpolate(const RGBColor &minColor, const RGBColor &maxColor, SUMOReal weight) throw();


    /** @brief Converts the given hsv-triplet to rgb
     * @param[in] h Hue (0-360)
     * @param[in] s Saturation (0-1)
     * @param[in] v Value (0-1)
     * @return The color as RGB
     * @author Alvy Ray Smith (http://www.alvyray.com/default.htm)
     */
    static RGBColor fromHSV(SUMOReal h, SUMOReal s, SUMOReal v) throw();


    /// @brief The string description of the default color
    static const std::string DEFAULT_COLOR_STRING;


    /// @brief The default color (for vehicle types and vehicles)
    static const RGBColor DEFAULT_COLOR;


private:
    /// @brief The color amounts
    SUMOReal myRed, myGreen, myBlue;

};


#endif

/****************************************************************************/

