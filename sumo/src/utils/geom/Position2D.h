/****************************************************************************/
/// @file    Position2D.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A position in a 2D-world
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
#ifndef Position2D_h
#define Position2D_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#include <iostream>
#include <cmath>

#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class Position2D
{
public:
    /// default constructor
    Position2D() : _x(0.0), _y(0.0)
    { }

    /// parametrised constructor
    Position2D(SUMOReal x, SUMOReal y)
            : _x(x), _y(y)
    { }

    /// Destructor
    ~Position2D()
    { }

    /// Returns the x-position
    SUMOReal x() const
    {
        return _x;
    }

    /// Returns the y-position
    SUMOReal y() const
    {
        return _y;
    }

    ///
    void set(SUMOReal x, SUMOReal y)
    {
        _x = x;
        _y = y;
    }

    ///
    void set(const Position2D &pos)
    {
        _x = pos._x;
        _y = pos._y;
    }


    /// Multiplies both positions with the given value
    void mul(SUMOReal val)
    {
        _x *= val;
        _y *= val;
    }

    /// Multiplies position with the given values
    void mul(SUMOReal mx, SUMOReal my)
    {
        _x *= mx;
        _y *= my;
    }

    /// Adds the given position to this one
    void add(const Position2D &pos)
    {
        _x += pos._x;
        _y += pos._y;
    }

    /// Adds the given position to this one
    void add(SUMOReal dx, SUMOReal dy)
    {
        _x += dx;
        _y += dy;
    }

    /// Substracts the given position from this one
    void sub(SUMOReal dx, SUMOReal dy)
    {
        _x -= dx;
        _y -= dy;
    }

    /// Substracts the given position from this one
    void sub(const Position2D &pos)
    {
        _x -= pos._x;
        _y -= pos._y;
    }

    SUMOReal scalar() const
    {
        return sqrt(_x*_x + _y*_y);
    }

    void norm()
    {
        SUMOReal val = scalar();
        _x = _x / val;
        _y = _y / val;
    }

    void reshiftRotate(SUMOReal xoff, SUMOReal yoff, SUMOReal rot)
    {
        SUMOReal x = _x * cos(rot) - _y * sin(rot) + xoff;
        SUMOReal y = _x * sin(rot) + yoff + _y * cos(rot);
        _x = x;
        _y = y;
    }


    /// Prints to the output
    friend std::ostream &operator<<(std::ostream &os, const Position2D &p)
    {
        os << p.x() << "," << p.y();
        return os;
    }

    friend bool operator==(const Position2D &p1, const Position2D &p2)
    {
        return p1.x()==p2.x() && p1.y()==p2.y();
    }

    friend bool operator!=(const Position2D &p1, const Position2D &p2)
    {
        return p1.x()!=p2.x() || p1.y()!=p2.y();
    }



private:
    /// The x-position
    SUMOReal _x;

    /// The y-position
    SUMOReal _y;

};


#endif

/****************************************************************************/

