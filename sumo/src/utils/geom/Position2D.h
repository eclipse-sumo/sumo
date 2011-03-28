/****************************************************************************/
/// @file    Position2D.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A position in a 2D-world
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
// included modules
// ===========================================================================
#include <iostream>
#include <cmath>

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Position2D
 * @brief A point in 2D with translation and scaling methods.
 */
class Position2D {
public:
    /// default constructor
    Position2D() : myX(0.0), myY(0.0), myZ(0.0) { }

    /// parametrised constructor
    Position2D(SUMOReal x, SUMOReal y)
            : myX(x), myY(y), myZ(0) { }

    /// parametrised constructor
    Position2D(SUMOReal x, SUMOReal y, SUMOReal z)
            : myX(x), myY(y), myZ(z) { }

    /// Destructor
    ~Position2D() { }

    /// Returns the x-position
    SUMOReal x() const {
        return myX;
    }

    /// Returns the y-position
    SUMOReal y() const {
        return myY;
    }

    /// Returns the z-position
    SUMOReal z() const {
        return myZ;
    }

    ///
    void set(SUMOReal x, SUMOReal y) {
        myX = x;
        myY = y;
    }

    ///
    void set(SUMOReal x, SUMOReal y, SUMOReal z) {
        myX = x;
        myY = y;
    }

    ///
    void set(const Position2D &pos) {
        myX = pos.myX;
        myY = pos.myY;
        myZ = pos.myZ;
    }


    /// Multiplies both positions with the given value
    void mul(SUMOReal val) {
        myX *= val;
        myY *= val;
        myZ *= val;
    }

    /// Multiplies position with the given values
    void mul(SUMOReal mx, SUMOReal my) {
        myX *= mx;
        myY *= my;
    }

    /// Multiplies position with the given values
    void mul(SUMOReal mx, SUMOReal my, SUMOReal mz) {
        myX *= mx;
        myY *= my;
        myZ *= mz;
    }

    /// Adds the given position to this one
    void add(const Position2D &pos) {
        myX += pos.myX;
        myY += pos.myY;
        myZ += pos.myZ;
    }

    /// Adds the given position to this one
    void add(SUMOReal dx, SUMOReal dy) {
        myX += dx;
        myY += dy;
    }

    /// Adds the given position to this one
    void add(SUMOReal dx, SUMOReal dy, SUMOReal dz) {
        myX += dx;
        myY += dy;
        myZ += dz;
    }

    /// Substracts the given position from this one
    void sub(SUMOReal dx, SUMOReal dy) {
        myX -= dx;
        myY -= dy;
    }

    /// Substracts the given position from this one
    void sub(SUMOReal dx, SUMOReal dy, SUMOReal dz) {
        myX -= dx;
        myY -= dy;
        myZ -= dz;
    }

    /// Substracts the given position from this one
    void sub(const Position2D &pos) {
        myX -= pos.myX;
        myY -= pos.myY;
        myZ -= pos.myZ;
    }

    void norm() {
        SUMOReal val = sqrt(myX*myX + myY*myY);
        myX = myX / val;
        myY = myY / val;
        myZ = myZ / val;
    }

    void reshiftRotate(SUMOReal xoff, SUMOReal yoff, SUMOReal rot) {
        SUMOReal x = myX * cos(rot) -myY * sin(rot) + xoff;
        SUMOReal y = myX * sin(rot) + yoff + myY * cos(rot);
        myX = x;
        myY = y;
    }


    /// Prints to the output
    friend std::ostream &operator<<(std::ostream &os, const Position2D &p) {
        os << p.x() << "," << p.y();
        if(p.z()!=SUMOReal(0.0)) {
            os << "," << p.z();
        }
        return os;
    }

    bool operator==(const Position2D &p2) const {
        return myX==p2.myX && myY==p2.myY && myZ==p2.myZ;
    }

    bool operator!=(const Position2D &p2) const {
        return myX!=p2.myX || myY!=p2.myY || myZ!=p2.myZ;
    }


    bool almostSame(const Position2D &p2, SUMOReal maxDiv=POSITION_EPS) const {
        return fabs(myX-p2.myX)<maxDiv && fabs(myY-p2.myY)<maxDiv && fabs(myZ-p2.myZ)<maxDiv;
    }


    inline SUMOReal distanceTo(const Position2D &p2) const {
        return sqrt(distanceSquaredTo(p2));
    }


    inline SUMOReal distanceSquaredTo(const Position2D &p2) const {
        return (myX-p2.myX)*(myX-p2.myX) + (myY-p2.myY)*(myY-p2.myY) + (myZ-p2.myZ)*(myZ-p2.myZ);
    }


private:
    /// The x-position
    SUMOReal myX;

    /// The y-position
    SUMOReal myY;

    /// The z-position
    SUMOReal myZ;

};


#endif

/****************************************************************************/

