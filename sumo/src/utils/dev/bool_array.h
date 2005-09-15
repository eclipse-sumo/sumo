#ifdef _DEBUG

// -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
// vim:tabstop=4:shiftwidth=4:expandtab:

/*
 * Copyright (C) 2004 Wu Yongwei <adah at users dot sourceforge dot net>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 *    not claim that you wrote the original software. If you use this
 *    software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must
 *    not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 * This file is part of Stones of Nvwa:
 *      http://sourceforge.net/projects/nvwa
 *
 */

/**
 * @file    bool_array.h
 *
 * Header file for class bool_array (packed boolean array).
 *
 * @version 3.0, 2004/08/07
 * @author  Wu Yongwei
 *
 */

#ifndef _BOOL_ARRAY_H
#define _BOOL_ARRAY_H

#ifndef _BYTE_DEFINED
#define _BYTE_DEFINED
typedef unsigned char BYTE;
#endif // !_BYTE_DEFINED

#include <assert.h>     // assert
#include <stdlib.h>     // exit, free, and NULL
#include <new>          // std::bad_alloc
#include <stdexcept>    // std::out_of_range
#include <string>       // for exception constructors

/**
 * Class to represent a packed boolean array.
 *
 * This was first written in April 1995, before I knew of any existing
 * implementation of this kind of classes.  Of course, the C++ Standard
 * Template Library now demands an implementation of packed boolean
 * array as `vector&lt;bool>', but the code here should still be useful
 * for the following three reasons: (1) STL support of MSVC 6 did not
 * implement this specialization (nor did it have a `bit_vector'); (2) I
 * incorporated some useful member functions from the STL bitset into
 * this `bool_array', including `reset', `set', `flip', and `count';
 * (3) In my tests under MSVC 6 and GCC 2.95.3/3.2.3 my code is really
 * FASTER than vector&lt;bool> or the normal boolean array.
 */
class bool_array
{
    /** Class to represent a reference to an array element. */
    class _Element
    {
    public:
        _Element(BYTE* __ptr, unsigned long __idx);
        bool operator=(bool __value);
        operator bool() const;
    private:
        BYTE*   _M_byte_ptr;
        size_t  _M_byte_idx;
        size_t  _M_bit_idx;
    };

public:
    bool_array() : _M_byte_ptr(NULL), _M_length(0) {}
    explicit bool_array(unsigned long __size);
    ~bool_array() { if (_M_byte_ptr != NULL) free(_M_byte_ptr); }

    bool create(unsigned long __size);
    void initialize(bool __value);

    // Using unsigned type here can increase performance!
    _Element operator[](unsigned long __idx);
    bool at(unsigned long __idx) const;
    void reset(unsigned long __idx);
    void set(unsigned long __idx);

    unsigned long size() const { return _M_length; }
    unsigned long count() const;
    unsigned long count(unsigned long __beg, unsigned long __end) const;
    void flip();

private:
    BYTE*           _M_byte_ptr;
    unsigned long   _M_length;
    static BYTE     _S_bit_count[256];
};


/* Inline functions */

/**
 * Constructs a reference to an array element.
 *
 * @param __ptr pointer to the interal boolean data
 * @param __idx index of the array element to access
 */
inline bool_array::_Element::_Element(BYTE* __ptr, unsigned long __idx)
{
    _M_byte_ptr = __ptr;
    _M_byte_idx = (size_t)(__idx / 8);
    _M_bit_idx  = (size_t)(__idx % 8);
}

/**
 * Assigns a new boolean value to an array element.
 *
 * @param __value   the new boolean value
 * @return          the assigned boolean value
 */
inline bool bool_array::_Element::operator=(bool __value)
{
    if (__value)
        *(_M_byte_ptr + _M_byte_idx) |= 1 << _M_bit_idx;
    else
        *(_M_byte_ptr + _M_byte_idx) &= ~(1 << _M_bit_idx);
    return __value;
}

/**
 * Reads the boolean value from an array element.
 *
 * @return  the boolean value of the accessed array element
 */
inline bool_array::_Element::operator bool() const
{
    return *(_M_byte_ptr + _M_byte_idx) & (1 << _M_bit_idx) ? true : false;
}

/**
 * Constructs the packed boolean array with a specific size.
 *
 * @param __size            size of the array
 * @throw std::out_of_range if \e __size equals \c 0
 * @throw std::bad_alloc    if memory is insufficient
 */
inline bool_array::bool_array(unsigned long __size)
    : _M_byte_ptr(NULL), _M_length(0)
{
    if (__size == 0)
        throw std::out_of_range("invalid bool_array size");

    if (!create(__size))
        throw std::bad_alloc();
}

/**
 * Creates a reference to an array element.
 *
 * @param __idx index of the array element to access
 */
inline bool_array::_Element bool_array::operator[](unsigned long __idx)
{
    assert(_M_byte_ptr);
    assert(__idx < _M_length);
    return _Element(_M_byte_ptr, __idx);
}

/**
 * Reads the boolean value of an array element via an index.
 *
 * @param __idx index of the array element to access
 * @return      the boolean value of the accessed array element
 * @throw std::out_of_range when the index is too big
 */
inline bool bool_array::at(unsigned long __idx) const
{
    size_t __byte_idx, __bit_idx;
    if (__idx >= _M_length)
        throw std::out_of_range("invalid bool_array subscript");
    __byte_idx = (size_t)(__idx / 8);
    __bit_idx  = (size_t)(__idx % 8);
    return *(_M_byte_ptr + __byte_idx) & (1 << __bit_idx) ? true : false;
}

/**
 * Resets an array element to \c false via an index.
 *
 * @param __idx index of the array element to access
 * @throw std::out_of_range when the index is too big
 */
inline void bool_array::reset(unsigned long __idx)
{
    size_t __byte_idx, __bit_idx;
    if (__idx >= _M_length)
        throw std::out_of_range("invalid bool_array subscript");
    __byte_idx = (size_t)(__idx / 8);
    __bit_idx  = (size_t)(__idx % 8);
    *(_M_byte_ptr + __byte_idx) &= ~(1 << __bit_idx);
}

/**
 * Sets an array element to \c true via an index.
 *
 * @param __idx index of the array element to access
 * @throw std::out_of_range when the index is too big
 */
inline void bool_array::set(unsigned long __idx)
{
    size_t __byte_idx, __bit_idx;
    if (__idx >= _M_length)
        throw std::out_of_range("invalid bool_array subscript");
    __byte_idx = (size_t)(__idx / 8);
    __bit_idx  = (size_t)(__idx % 8);
    *(_M_byte_ptr + __byte_idx) |= 1 << __bit_idx;
}

#endif // _BOOL_ARRAY_H

#endif // _DEBUG
