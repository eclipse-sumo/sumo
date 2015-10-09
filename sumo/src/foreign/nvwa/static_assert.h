// -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
// vim:tabstop=4:shiftwidth=4:expandtab:

/*
 * Copyright (C) 2004-2013 Wu Yongwei <adah at users dot sourceforge dot net>
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
 *    not claim that you wrote the original software.  If you use this
 *    software in a product, an acknowledgement in the product
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
 * @file  static_assert.h
 *
 * Template class to check validity duing compile time (adapted from Loki).
 *
 * @date  2013-09-07
 */

#ifndef STATIC_ASSERT

#include "c++11.h"

#if HAVE_CXX11_STATIC_ASSERT

#define STATIC_ASSERT(_Expr, _Msg) static_assert(_Expr, #_Msg)

#else

namespace nvwa {

template <bool> struct compile_time_error;
template <>     struct compile_time_error<true> {};

#define STATIC_ASSERT(_Expr, _Msg) \
    { \
        nvwa::compile_time_error<((_Expr) != 0)> ERROR_##_Msg; \
        (void)ERROR_##_Msg; \
    }

}

#endif // HAVE_CXX11_STATIC_ASSERT

#endif // STATIC_ASSERT
