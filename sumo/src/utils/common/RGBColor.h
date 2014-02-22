/****************************************************************************/
/// @file    RGBColor.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A RGB-color definition
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
 * The definition of a color in the RGB-space with an alpha channel.
 * The cube is meant to lie between (0, 0, 0) and (255, 255, 255)
 */
class RGBColor {
public:
    /** @brief Constructor
     */
    RGBColor();


    /** @brief Constructor
     * @param[in] red The red component's value
     * @param[in] green The green component's value
     * @param[in] blue The blue component's value
     */
    RGBColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);


    /** @brief Copy constructor
     */
    RGBColor(const RGBColor& col);


    /// @brief Destructor
    ~RGBColor();



    /** @brief Returns the red-amount of the color
     * @return The red component's value
     */
    unsigned char red() const {
        return myRed;
    }


    /** @brief Returns the green-amount of the color
     * @return The green component's value
     */
    unsigned char green() const {
        return myGreen;
    }


    /** @brief Returns the blue-amount of the color
     * @return The blue component's value
     */
    unsigned char blue() const {
        return myBlue;
    }


    /** @brief Returns the alpha-amount of the color
     * @return The alpha component's value
     */
    unsigned char alpha() const {
        return myAlpha;
    }


    /** @brief assigns new values
     * @param[in] r The red component's value
     * @param[in] g The green component's value
     * @param[in] b The blue component's value
     * @param[in] a The alpha component's value
     */
    void set(unsigned char r, unsigned char g, unsigned char b, unsigned char a);


    /** @brief Writes the color to the given stream
     * @param[out] os The stream to write to
     * @param[in] col The color to write
     * @return The stream
     */
    friend std::ostream& operator<<(std::ostream& os, const RGBColor& col);


    bool operator==(const RGBColor& c) const;
    bool operator!=(const RGBColor& c) const;


    /** @brief Returns a new color with altered brightness
     * @param[in] change The absolute change applied to all channels (within bounds)
     * @return The new color
     */
    RGBColor changedBrightness(const char change);


    /** @brief Parses a color information
     *
     * It is assumed that the color is stored as "<RED>,<GREEN>,<BLUE>"
     *  and each color is represented as a SUMOReal.
     *  Alternatively the color can be stored as "<RED>,<GREEN>,<BLUE>,<ALPHA>"
     *  and each color is represented as an unsigned byte.
     * @param[in] coldef The color definition to parse
     * @return The parsed color
     * @exception EmptyData If the definition has less than three entries
     * @exception NumberFormatException If one of the components is not numeric
     */
    static RGBColor parseColor(std::string coldef);


    /** @brief Parses a color information
     *
     * It is assumed that the color is stored as "<RED>,<GREEN>,<BLUE>"
     *  and each color is represented as a SUMOReal.
     *  Alternatively the color can be stored as "<RED>,<GREEN>,<BLUE>,<ALPHA>"
     *  and each color is represented as an unsigned byte.
     * @param[in] coldef The color definition to parse
     * @param[in] objecttype The type of the currently parsed object
     * @param[in] objectid The id of the currently parsed object
     * @param[in] report Whether errors shall be reported
     * @param[in, out] ok Whether parsing was successful
     * @return The parsed color
     * @exception EmptyData If the definition has less than three entries
     * @exception NumberFormatException If one of the components is not numeric
     */
    static RGBColor parseColorReporting(const std::string& coldef, const std::string& objecttype,
                                        const char* objectid, bool report, bool& ok);


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
    static RGBColor interpolate(const RGBColor& minColor, const RGBColor& maxColor, SUMOReal weight);


    /** @brief Converts the given hsv-triplet to rgb
     * @param[in] h Hue (0-360)
     * @param[in] s Saturation (0-1)
     * @param[in] v Value (0-1)
     * @return The color as RGB
     * @author Alvy Ray Smith (http://www.alvyray.com/default.htm)
     */
    static RGBColor fromHSV(SUMOReal h, SUMOReal s, SUMOReal v);


    static const RGBColor RED;
    static const RGBColor GREEN;
    static const RGBColor BLUE;
    static const RGBColor YELLOW;
    static const RGBColor CYAN;
    static const RGBColor MAGENTA;
    static const RGBColor WHITE;
    static const RGBColor BLACK;
    static const RGBColor GREY;
    /// @brief The default color (for vehicle types and vehicles)
    static const RGBColor DEFAULT_COLOR;


    /// @brief The string description of the default color
    static const std::string DEFAULT_COLOR_STRING;


private:
    /// @brief The color amounts
    unsigned char myRed, myGreen, myBlue, myAlpha;

};


#endif

/****************************************************************************/

