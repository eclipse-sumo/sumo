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
 * @file    bool_array.cpp
 *
 * Code for class bool_array (packed boolean array).
 *
 * @version 3.0, 2004/08/07
 * @author  Wu Yongwei
 *
 */

#include <limits.h>
#include <string.h>
#include "bool_array.h"

BYTE bool_array::_S_bit_count[256] =
{
    0, /*   0 */ 1, /*   1 */ 1, /*   2 */ 2, /*   3 */ 1, /*   4 */
    2, /*   5 */ 2, /*   6 */ 3, /*   7 */ 1, /*   8 */ 2, /*   9 */
    2, /*  10 */ 3, /*  11 */ 2, /*  12 */ 3, /*  13 */ 3, /*  14 */
    4, /*  15 */ 1, /*  16 */ 2, /*  17 */ 2, /*  18 */ 3, /*  19 */
    2, /*  20 */ 3, /*  21 */ 3, /*  22 */ 4, /*  23 */ 2, /*  24 */
    3, /*  25 */ 3, /*  26 */ 4, /*  27 */ 3, /*  28 */ 4, /*  29 */
    4, /*  30 */ 5, /*  31 */ 1, /*  32 */ 2, /*  33 */ 2, /*  34 */
    3, /*  35 */ 2, /*  36 */ 3, /*  37 */ 3, /*  38 */ 4, /*  39 */
    2, /*  40 */ 3, /*  41 */ 3, /*  42 */ 4, /*  43 */ 3, /*  44 */
    4, /*  45 */ 4, /*  46 */ 5, /*  47 */ 2, /*  48 */ 3, /*  49 */
    3, /*  50 */ 4, /*  51 */ 3, /*  52 */ 4, /*  53 */ 4, /*  54 */
    5, /*  55 */ 3, /*  56 */ 4, /*  57 */ 4, /*  58 */ 5, /*  59 */
    4, /*  60 */ 5, /*  61 */ 5, /*  62 */ 6, /*  63 */ 1, /*  64 */
    2, /*  65 */ 2, /*  66 */ 3, /*  67 */ 2, /*  68 */ 3, /*  69 */
    3, /*  70 */ 4, /*  71 */ 2, /*  72 */ 3, /*  73 */ 3, /*  74 */
    4, /*  75 */ 3, /*  76 */ 4, /*  77 */ 4, /*  78 */ 5, /*  79 */
    2, /*  80 */ 3, /*  81 */ 3, /*  82 */ 4, /*  83 */ 3, /*  84 */
    4, /*  85 */ 4, /*  86 */ 5, /*  87 */ 3, /*  88 */ 4, /*  89 */
    4, /*  90 */ 5, /*  91 */ 4, /*  92 */ 5, /*  93 */ 5, /*  94 */
    6, /*  95 */ 2, /*  96 */ 3, /*  97 */ 3, /*  98 */ 4, /*  99 */
    3, /* 100 */ 4, /* 101 */ 4, /* 102 */ 5, /* 103 */ 3, /* 104 */
    4, /* 105 */ 4, /* 106 */ 5, /* 107 */ 4, /* 108 */ 5, /* 109 */
    5, /* 110 */ 6, /* 111 */ 3, /* 112 */ 4, /* 113 */ 4, /* 114 */
    5, /* 115 */ 4, /* 116 */ 5, /* 117 */ 5, /* 118 */ 6, /* 119 */
    4, /* 120 */ 5, /* 121 */ 5, /* 122 */ 6, /* 123 */ 5, /* 124 */
    6, /* 125 */ 6, /* 126 */ 7, /* 127 */ 1, /* 128 */ 2, /* 129 */
    2, /* 130 */ 3, /* 131 */ 2, /* 132 */ 3, /* 133 */ 3, /* 134 */
    4, /* 135 */ 2, /* 136 */ 3, /* 137 */ 3, /* 138 */ 4, /* 139 */
    3, /* 140 */ 4, /* 141 */ 4, /* 142 */ 5, /* 143 */ 2, /* 144 */
    3, /* 145 */ 3, /* 146 */ 4, /* 147 */ 3, /* 148 */ 4, /* 149 */
    4, /* 150 */ 5, /* 151 */ 3, /* 152 */ 4, /* 153 */ 4, /* 154 */
    5, /* 155 */ 4, /* 156 */ 5, /* 157 */ 5, /* 158 */ 6, /* 159 */
    2, /* 160 */ 3, /* 161 */ 3, /* 162 */ 4, /* 163 */ 3, /* 164 */
    4, /* 165 */ 4, /* 166 */ 5, /* 167 */ 3, /* 168 */ 4, /* 169 */
    4, /* 170 */ 5, /* 171 */ 4, /* 172 */ 5, /* 173 */ 5, /* 174 */
    6, /* 175 */ 3, /* 176 */ 4, /* 177 */ 4, /* 178 */ 5, /* 179 */
    4, /* 180 */ 5, /* 181 */ 5, /* 182 */ 6, /* 183 */ 4, /* 184 */
    5, /* 185 */ 5, /* 186 */ 6, /* 187 */ 5, /* 188 */ 6, /* 189 */
    6, /* 190 */ 7, /* 191 */ 2, /* 192 */ 3, /* 193 */ 3, /* 194 */
    4, /* 195 */ 3, /* 196 */ 4, /* 197 */ 4, /* 198 */ 5, /* 199 */
    3, /* 200 */ 4, /* 201 */ 4, /* 202 */ 5, /* 203 */ 4, /* 204 */
    5, /* 205 */ 5, /* 206 */ 6, /* 207 */ 3, /* 208 */ 4, /* 209 */
    4, /* 210 */ 5, /* 211 */ 4, /* 212 */ 5, /* 213 */ 5, /* 214 */
    6, /* 215 */ 4, /* 216 */ 5, /* 217 */ 5, /* 218 */ 6, /* 219 */
    5, /* 220 */ 6, /* 221 */ 6, /* 222 */ 7, /* 223 */ 3, /* 224 */
    4, /* 225 */ 4, /* 226 */ 5, /* 227 */ 4, /* 228 */ 5, /* 229 */
    5, /* 230 */ 6, /* 231 */ 4, /* 232 */ 5, /* 233 */ 5, /* 234 */
    6, /* 235 */ 5, /* 236 */ 6, /* 237 */ 6, /* 238 */ 7, /* 239 */
    4, /* 240 */ 5, /* 241 */ 5, /* 242 */ 6, /* 243 */ 5, /* 244 */
    6, /* 245 */ 6, /* 246 */ 7, /* 247 */ 5, /* 248 */ 6, /* 249 */
    6, /* 250 */ 7, /* 251 */ 6, /* 252 */ 7, /* 253 */ 7, /* 254 */
    8  /* 255 */
}; // End _S_bit_count

/**
 * Creates the packed boolean array with a specific size.
 *
 * @param __size    size of the array
 * @return          \c false if \e __size equals \c 0 or is too big, or
 *                  if memory is insufficient; \c true if \e __size has
 *                  a suitable value and memory allocation is
 *                  successful.
 */
bool bool_array::create(unsigned long __size)
{
    if (__size == 0)
        return false;
    // Will be optimized away by a decent compiler if ULONG_MAX == UINT_MAX
    if (ULONG_MAX > UINT_MAX && ((__size - 1) / 8 + 1) > UINT_MAX)
        return false;

    size_t __byte_cnt = (size_t)((__size - 1) / 8 + 1);
    if (_M_byte_ptr)
        free(_M_byte_ptr);
    _M_length = 0;

    // Uses malloc/free instead of new/delete to avoid exception handling
    // differences between compilers
    if (!(_M_byte_ptr = (BYTE*)malloc(__byte_cnt)))
        return false;

    _M_length = __size;
    return true;
}

/**
 * Initializes all array elements to a specific value optimally.
 *
 * @param __value   the boolean value to assign to all elements
 */
void bool_array::initialize(bool __value)
{
    assert(_M_byte_ptr);
    size_t __byte_cnt = (size_t)((_M_length - 1) / 8) + 1;
    memset(_M_byte_ptr, __value ? ~0 : 0, __byte_cnt);
    if (__value)
    {
        int __valid_bits_in_last_byte = (_M_length - 1) % 8 + 1;
        _M_byte_ptr[__byte_cnt - 1] &= ~(~0 << __valid_bits_in_last_byte);
    }
}

/**
 * Counts elements with a \c true value.
 *
 * @return  the count of \c true elements
 */
unsigned long bool_array::count() const
{
    assert(_M_byte_ptr);
    unsigned long __true_cnt = 0;
    size_t __byte_cnt = (size_t)((_M_length - 1) / 8) + 1;
    for (size_t __i = 0; __i < __byte_cnt; ++__i)
        __true_cnt += _S_bit_count[_M_byte_ptr[__i]];
    return __true_cnt;
}

/**
 * Counts elements with a \c true value in a specified range.
 *
 * @param __beg beginning of the range
 * @param __end end of the range (exclusive)
 * @return      the count of \c true elements
 */
unsigned long bool_array::count(unsigned long __beg, unsigned long __end) const
{
    assert(_M_byte_ptr);
    unsigned long __true_cnt = 0;
    size_t __byte_idx_beg, __byte_idx_end;
    BYTE __byte_val;

    if (__beg >= __end)
        return 0;
    if (__end > _M_length)
        throw std::out_of_range("invalid bool_array subscript");
    --__end;

    __byte_idx_beg = (size_t)(__beg / 8);
    __byte_val = _M_byte_ptr[__byte_idx_beg];
    __byte_val &= ~0 << (__beg % 8);

    __byte_idx_end = (size_t)(__end / 8);
    if (__byte_idx_beg < __byte_idx_end)
    {
        __true_cnt = _S_bit_count[__byte_val];
        __byte_val = _M_byte_ptr[__byte_idx_end];
    }
    __byte_val &= ~(~0 << (__end % 8 + 1));
    __true_cnt += _S_bit_count[__byte_val];

    for (++__byte_idx_beg; __byte_idx_beg < __byte_idx_end; ++__byte_idx_beg)
        __true_cnt += _S_bit_count[_M_byte_ptr[__byte_idx_beg]];
    return __true_cnt;
}

/**
 * Changes all \c true elements to \c false, and \c false ones to \c true.
 */
void bool_array::flip()
{
    assert(_M_byte_ptr);
    size_t __byte_cnt = (size_t)((_M_length - 1) / 8) + 1;
    for (size_t __i = 0; __i < __byte_cnt; ++__i)
        _M_byte_ptr[__i] = ~_M_byte_ptr[__i];
    int __valid_bits_in_last_byte = (_M_length - 1) % 8 + 1;
    _M_byte_ptr[__byte_cnt - 1] &= ~(~0 << __valid_bits_in_last_byte);
}

#endif // _DEBUG
