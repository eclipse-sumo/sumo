// -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
// vim:tabstop=4:shiftwidth=4:expandtab:

/*
 * Copyright (C) 2013 Wu Yongwei <adah at users dot sourceforge dot net>
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
 * @file  c++11.h
 *
 * C++11 feature detection macros and workarounds.
 *
 * @date  2013-10-03
 */

#ifndef NVWA_CXX11_H
#define NVWA_CXX11_H

// Only Clang provides these macros; they need to be defined as follows
// to get a valid expression in preprocessing by other compilers.
#ifndef __has_extension
#define __has_extension(x) 0
#endif
#ifndef __has_feature
#define __has_feature(x) 0
#endif
#ifndef __has_include
#define __has_include(x) 0
#endif

// Detect whether C++11 mode is on (for GCC and Clang).  MSVC does not
// have a special C++11 mode, so it is always on for Visual C++ 2010 and
// later.
#if __cplusplus >= 201103L || \
    defined(__GXX_EXPERIMENTAL_CXX0X__) || \
    (defined(_MSC_VER) && _MSC_VER >= 1600)
#define NVWA_CXX11_MODE 1
#else
#define NVWA_CXX11_MODE 0
#endif


/* Feature checks */

#if !defined(HAVE_CXX11_ATOMIC)
#if NVWA_CXX11_MODE && \
    (__has_include(<atomic>) || \
     (defined(_MSC_VER) && _MSC_VER >= 1700) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 405 && \
      !defined(__MINGW32__)))
// Note: MinGW GCC does not support atomics out of the box as of 4.8.
#define HAVE_CXX11_ATOMIC 1
#else
#define HAVE_CXX11_ATOMIC 0
#endif
#endif

#if !defined(HAVE_CXX11_AUTO_TYPE)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_auto_type) || \
     (defined(_MSC_VER) && _MSC_VER >= 1600) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 404))
#define HAVE_CXX11_AUTO_TYPE 1
#else
#define HAVE_CXX11_AUTO_TYPE 0
#endif
#endif

#if !defined(HAVE_CXX11_EXPLICIT_CONVERSION)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_explicit_conversions) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 405))
#define HAVE_CXX11_EXPLICIT_CONVERSION 1
#else
#define HAVE_CXX11_EXPLICIT_CONVERSION 0
#endif
#endif

#if !defined(HAVE_CXX11_FINAL)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_override_control) || \
     (defined(_MSC_VER) && _MSC_VER >= 1700) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 407))
#define HAVE_CXX11_FINAL 1
#else
#define HAVE_CXX11_FINAL 0
#endif
#endif

#if !defined(HAVE_CXX11_FUTURE)
#if NVWA_CXX11_MODE && \
    (__has_include(<future>) || \
     (defined(_MSC_VER) && _MSC_VER >= 1700) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 405 && \
      !defined(__MINGW32__)))
// Note: MinGW GCC does not support futures out of the box as of 4.8.
#define HAVE_CXX11_FUTURE 1
#else
#define HAVE_CXX11_FUTURE 0
#endif
#endif

#if !defined(HAVE_CXX11_GENERALIZED_INITIALIZER)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_generalized_initializers) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 404))
#define HAVE_CXX11_GENERALIZED_INITIALIZER 1
#else
#define HAVE_CXX11_GENERALIZED_INITIALIZER 0
#endif
#endif

#if !defined(HAVE_CXX11_LAMBDA)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_lambdas) || \
     (defined(_MSC_VER) && _MSC_VER >= 1600) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 405))
#define HAVE_CXX11_LAMBDA 1
#else
#define HAVE_CXX11_LAMBDA 0
#endif
#endif

#if !defined(HAVE_CXX11_MUTEX)
#if NVWA_CXX11_MODE && \
    (__has_include(<mutex>) || \
     (defined(_MSC_VER) && _MSC_VER >= 1700) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 403 && \
      !defined(__MINGW32__)))
// Note: MinGW GCC does not support std::mutex out of the box as of 4.8.
#define HAVE_CXX11_MUTEX 1
#else
#define HAVE_CXX11_MUTEX 0
#endif
#endif

#if !defined(HAVE_CXX11_NOEXCEPT)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_noexcept) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 406))
#define HAVE_CXX11_NOEXCEPT 1
#else
#define HAVE_CXX11_NOEXCEPT 0
#endif
#endif

#if !defined(HAVE_CXX11_NULLPTR)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_nullptr) || \
     (defined(_MSC_VER) && _MSC_VER >= 1600) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 406))
#define HAVE_CXX11_NULLPTR 1
#else
#define HAVE_CXX11_NULLPTR 0
#endif
#endif

#if !defined(HAVE_CXX11_OVERRIDE)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_override_control) || \
     (defined(_MSC_VER) && _MSC_VER >= 1600) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 407))
#define HAVE_CXX11_OVERRIDE 1
#else
#define HAVE_CXX11_OVERRIDE 0
#endif
#endif

#if !defined(HAVE_CXX11_RANGE_FOR)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_range_for) || \
     (defined(_MSC_VER) && _MSC_VER >= 1700) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 406))
#define HAVE_CXX11_RANGE_FOR 1
#else
#define HAVE_CXX11_RANGE_FOR 0
#endif
#endif

#if !defined(HAVE_CXX11_RVALUE_REFERENCE)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_rvalue_references) || \
     (defined(_MSC_VER) && _MSC_VER >= 1600) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 405))
#define HAVE_CXX11_RVALUE_REFERENCE 1
#else
#define HAVE_CXX11_RVALUE_REFERENCE 0
#endif
#endif

#if !defined(HAVE_CXX11_STATIC_ASSERT)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_static_assert) || \
     (defined(_MSC_VER) && _MSC_VER >= 1600) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 403))
#define HAVE_CXX11_STATIC_ASSERT 1
#else
#define HAVE_CXX11_STATIC_ASSERT 0
#endif
#endif

#if !defined(HAVE_CXX11_THREAD)
#if NVWA_CXX11_MODE && \
    (__has_include(<thread>) || \
     (defined(_MSC_VER) && _MSC_VER >= 1700) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 404 && \
      !defined(__MINGW32__)))
// Note: MinGW GCC does not support std::thread out of the box as of 4.8.
#define HAVE_CXX11_THREAD 1
#else
#define HAVE_CXX11_THREAD 0
#endif
#endif

#if !defined(HAVE_CXX11_THREAD_LOCAL)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_thread_local) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 408))
#define HAVE_CXX11_THREAD_LOCAL 1
#else
#define HAVE_CXX11_THREAD_LOCAL 0
#endif
#endif

#if !defined(HAVE_CXX11_TYPE_TRAITS)
#if NVWA_CXX11_MODE && \
    (__has_include(<type_traits>) || \
     (defined(_MSC_VER) && _MSC_VER >= 1600) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 403))
#define HAVE_CXX11_TYPE_TRAITS 1
#else
#define HAVE_CXX11_TYPE_TRAITS 0
#endif
#endif

#if !defined(HAVE_CXX11_UNICODE_LITERAL)
#if NVWA_CXX11_MODE && \
    (__has_feature(cxx_unicode_literals) || \
     (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 405))
#define HAVE_CXX11_UNICODE_LITERAL 1
#else
#define HAVE_CXX11_UNICODE_LITERAL 0
#endif
#endif


/* Workarounds */

#if HAVE_CXX11_FINAL
#define _FINAL final
#else
#define _FINAL
#endif

#if HAVE_CXX11_OVERRIDE
#define _OVERRIDE override
#else
#define _OVERRIDE
#endif

#if HAVE_CXX11_NOEXCEPT
#define _NOEXCEPT noexcept
#define _NOEXCEPT_(x) noexcept(x)
#else
#define _NOEXCEPT throw ()
#define _NOEXCEPT_(x)
#endif

#if HAVE_CXX11_NULLPTR
#define _NULLPTR nullptr
#else
#define _NULLPTR NULL
#endif

#if HAVE_CXX11_THREAD_LOCAL
#define _THREAD_LOCAL thread_local
#else
#ifdef _MSC_VER
#define _THREAD_LOCAL __declspec(thread)
#else
#define _THREAD_LOCAL __thread
#endif
#endif

#endif // NVWA_CXX11_H
