#ifdef _DEBUG
#ifdef CHECK_MEMORY_LEAKS
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
 * @file    set_assign.h
 *
 * Definition of template functions set_assign_union and set_assign_difference.
 *
 * @version 1.5, 2004/07/26
 * @author  Wu Yongwei
 *
 */

#ifndef _SET_ASSIGN_H
#define _SET_ASSIGN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <algorithm>

template <class _Container, class _InputIter>
_Container& set_assign_union(_Container& __dest,
                             _InputIter __first,
                             _InputIter __last)
{
    typename _Container::iterator __first_dest = __dest.begin();
    typename _Container::iterator  __last_dest = __dest.end();
    while (__first_dest != __last_dest && __first != __last)
    {
        if (*__first_dest < *__first)
            ++__first_dest;
        else if (*__first < *__first_dest)
        {
            __dest.insert(__first_dest, *__first);
            ++__first;
        }
        else    // *__first_dest == *__first
        {
            ++__first_dest;
            ++__first;
        }
    }
    if (__first != __last)
        std::copy(__first, __last, inserter(__dest, __last_dest));
    return __dest;
}

template <class _Container, class _InputIter, class _Compare>
_Container& set_assign_union(_Container& __dest,
                             _InputIter __first,
                             _InputIter __last,
                             _Compare __comp)
{
    typename _Container::iterator __first_dest = __dest.begin();
    typename _Container::iterator  __last_dest = __dest.end();
    while (__first_dest != __last_dest && __first != __last)
    {
        if (__comp(*__first_dest, *__first))
            ++__first_dest;
        else if (__comp(*__first, *__first_dest))
        {
            __dest.insert(__first_dest, *__first);
            ++__first;
        }
        else    // *__first_dest is equivalent to *__first
        {
            ++__first_dest;
            ++__first;
        }
    }
    if (__first != __last)
        std::copy(__first, __last, inserter(__dest, __last_dest));
    return __dest;
}

template <class _Container, class _InputIter>
_Container& set_assign_difference(_Container& __dest,
                                  _InputIter __first,
                                  _InputIter __last)
{
    typename _Container::iterator __first_dest = __dest.begin();
    typename _Container::iterator  __last_dest = __dest.end();
    while (__first_dest != __last_dest && __first != __last)
    {
        if (*__first_dest < *__first)
            ++__first_dest;
        else if (*__first < *__first_dest)
            ++__first;
        else    // *__first_dest == *__first
        {
            __dest.erase(__first_dest++);
            ++__first;
        }
    }
    return __dest;
}

template <class _Container, class _InputIter, class _Compare>
_Container& set_assign_difference(_Container& __dest,
                                  _InputIter __first,
                                  _InputIter __last,
                                  _Compare __comp)
{
    typename _Container::iterator __first_dest = __dest.begin();
    typename _Container::iterator  __last_dest = __dest.end();
    while (__first_dest != __last_dest && __first != __last)
    {
        if (__comp(*__first_dest, *__first))
            ++__first_dest;
        else if (__comp(*__first, *__first_dest))
            ++__first;
        else    // *__first_dest is equivalent to *__first
        {
            __dest.erase(__first_dest++);
            ++__first;
        }
    }
    return __dest;
}

#endif // _SET_ASSIGN_H

#endif // CHECK_MEMORY_LEAKS
#endif // _DEBUG
