/****************************************************************************/
/// @file    Position.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A position in the 2D- or 3D-world
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
#ifndef Position_h
#define Position_h


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
 * @class Position
 * @brief A point in 2D or 3D with translation and scaling methods.
 */
class Position {
public:
    /// default constructor
    Position() : myX(0.0), myY(0.0), myZ(0.0) { }

    /// parametrised constructor
    Position(SUMOReal x, SUMOReal y)
        : myX(x), myY(y), myZ(0) { }

    /// parametrised constructor
    Position(SUMOReal x, SUMOReal y, SUMOReal z)
        : myX(x), myY(y), myZ(z) { }

    /// Destructor
    ~Position() { }

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
        myZ = z;
    }

    ///
    void set(const Position& pos) {
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
    void add(const Position& pos) {
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
    void sub(const Position& pos) {
        myX -= pos.myX;
        myY -= pos.myY;
        myZ -= pos.myZ;
    }

    void norm2d() {
        SUMOReal val = sqrt(myX * myX + myY * myY);
        myX = myX / val;
        myY = myY / val;
    }

    void reshiftRotate(SUMOReal xoff, SUMOReal yoff, SUMOReal rot) {
        SUMOReal x = myX * cos(rot) - myY * sin(rot) + xoff;
        SUMOReal y = myX * sin(rot) + yoff + myY * cos(rot);
        myX = x;
        myY = y;
    }


    /// Prints to the output
    friend std::ostream& operator<<(std::ostream& os, const Position& p) {
        os << p.x() << "," << p.y();
        if (p.z() != SUMOReal(0.0)) {
            os << "," << p.z();
        }
        return os;
    }

    Position operator+(const Position& p2) const {
        return Position(myX + p2.myX,  myY + p2.myY, myZ + p2.myZ);
    }

    Position operator-(const Position& p2) const {
        return Position(myX - p2.myX,  myY - p2.myY, myZ - p2.myZ);
    }

    /// @brief keep the direction but modify the length of the (location) vector to length * scalar
    Position operator*(SUMOReal scalar) const {
        return Position(myX * scalar, myY * scalar, myZ * scalar);
    }

    /// @brief keep the direction but modify the length of the (location) vector to length + scalar
    Position operator+(SUMOReal offset) const {
        const SUMOReal length = distanceTo(Position(0,0,0));
        if (length == 0) {
            return *this;
        }
        const SUMOReal scalar = (length + offset) / length;
        return Position(myX * scalar, myY * scalar, myZ * scalar);
    }

    bool operator==(const Position& p2) const {
        return myX == p2.myX && myY == p2.myY && myZ == p2.myZ;
    }

    bool operator!=(const Position& p2) const {
        return myX != p2.myX || myY != p2.myY || myZ != p2.myZ;
    }


    bool almostSame(const Position& p2, SUMOReal maxDiv = POSITION_EPS) const {
        return fabs(myX - p2.myX) < maxDiv && fabs(myY - p2.myY) < maxDiv && fabs(myZ - p2.myZ) < maxDiv;
    }


    /// @brief returns the euclidean distance in 3 dimension
    inline SUMOReal distanceTo(const Position& p2) const {
        return sqrt(distanceSquaredTo(p2));
    }


    inline SUMOReal distanceSquaredTo(const Position& p2) const {
        return (myX - p2.myX) * (myX - p2.myX) + (myY - p2.myY) * (myY - p2.myY) + (myZ - p2.myZ) * (myZ - p2.myZ);
    }


    /// @brief returns the euclidean distance in the x-y-plane
    inline SUMOReal distanceTo2D(const Position& p2) const {
        return sqrt(distanceSquaredTo2D(p2));
    }


    inline SUMOReal distanceSquaredTo2D(const Position& p2) const {
        return (myX - p2.myX) * (myX - p2.myX) + (myY - p2.myY) * (myY - p2.myY);
    }

    /// @brief returns the cross product between this point and the second one
    Position crossProduct(const Position& pos) {
        return Position(
                   myY * pos.myZ - myZ * pos.myY,
                   myZ * pos.myX - myX * pos.myZ,
                   myX * pos.myY - myY * pos.myX);
    }

    /// @brief returns the dot product (scalar product) between this point and the second one
    inline SUMOReal dotProduct(const Position& pos) {
        return myX * pos.myX + myY * pos.myY + myZ * pos.myZ;
    }

    static const Position INVALID;

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

