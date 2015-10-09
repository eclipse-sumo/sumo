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
 * @file  fast_mutex.h
 *
 * A fast mutex implementation for POSIX and Win32.
 *
 * @date  2013-08-02
 */

#ifndef NVWA_FAST_MUTEX_H
#define NVWA_FAST_MUTEX_H

#include "_nvwa.h"              // NVWA_NAMESPACE_*
#include "c++11.h"              // HAVE_CXX11_MUTEX

# if !defined(_NOTHREADS)
#   if !defined(NVWA_USE_CXX11_MUTEX) && HAVE_CXX11_MUTEX != 0 && \
            !defined(_WIN32THREADS) && defined(_WIN32) && defined(_MT) && \
            (!defined(_MSC_VER) || defined(_DLL))
//      Prefer using std::mutex on Windows to avoid the namespace
//      pollution caused by <windows.h>.  However, MSVC has a re-entry
//      issue with its std::mutex implementation, and std::mutex should
//      not be used unless /MD or /MDd is used.  For more information,
//      check out:
//
//        https://connect.microsoft.com/VisualStudio/feedback/details/776596/std-mutex-not-a-constexpr-with-mtd-compiler-flag
//        http://stackoverflow.com/questions/14319344/stdmutex-lock-hangs-when-overriding-the-new-operator
//
#       define NVWA_USE_CXX11_MUTEX 1
#   endif

#   if !defined(_WIN32THREADS) && \
            (defined(_WIN32) && defined(_MT))
//      Automatically use _WIN32THREADS when specifying -MT/-MD in MSVC,
//      or -mthreads in MinGW GCC.
#       define _WIN32THREADS
#   elif !defined(_PTHREADS) && \
            defined(_REENTRANT)
//      Automatically use _PTHREADS when specifying -pthread in GCC or Clang.
#       define _PTHREADS
#   endif
# endif

# ifndef NVWA_USE_CXX11_MUTEX
#   define NVWA_USE_CXX11_MUTEX 0
# endif

# if !defined(_PTHREADS) && !defined(_WIN32THREADS) && \
        !defined(_NOTHREADS) && NVWA_USE_CXX11_MUTEX == 0
#   define _NOTHREADS
# endif

# if defined(_NOTHREADS)
#   if defined(_PTHREADS) || defined(_WIN32THREADS) || \
            NVWA_USE_CXX11_MUTEX != 0
#       undef _NOTHREADS
#       error "Cannot define multi-threaded mode with -D_NOTHREADS"
#   endif
# endif

# if defined(__MINGW32__) && defined(_WIN32THREADS) && !defined(_MT)
#   error "Be sure to specify -mthreads with -D_WIN32THREADS"
# endif

# ifndef _FAST_MUTEX_CHECK_INITIALIZATION
/**
 * Macro to control whether to check for initialization status for each
 * lock/unlock operation.  Defining it to a non-zero value will enable
 * the check, so that the construction/destruction of a static object
 * using a static fast_mutex not yet constructed or already destroyed
 * will work (with lock/unlock operations ignored).  Defining it to zero
 * will disable to check.
 */
#   define _FAST_MUTEX_CHECK_INITIALIZATION 1
# endif

# if defined(_PTHREADS) && defined(_WIN32THREADS)
//  Some C++ libraries have _PTHREADS defined even on Win32 platforms.
//  Thus this hack.
#   undef _PTHREADS
# endif

# ifdef _DEBUG
#   include <stdio.h>
#   include <stdlib.h>
/** Macro for fast_mutex assertions.  Real version (for debug mode). */
#   define _FAST_MUTEX_ASSERT(_Expr, _Msg) \
        if (!(_Expr)) { \
            fprintf(stderr, "fast_mutex::%s\n", _Msg); \
            abort(); \
        }
# else
/** Macro for fast_mutex assertions.  Fake version (for release mode). */
#   define _FAST_MUTEX_ASSERT(_Expr, _Msg) \
        ((void)0)
# endif

# if NVWA_USE_CXX11_MUTEX != 0
#   include <mutex>
NVWA_NAMESPACE_BEGIN
/**
 * Macro alias to `volatile' semantics.  Here it is truly volatile since
 * it is in a multi-threaded (C++11) environment.
 */
#   define __VOLATILE volatile
    /**
     * Class for non-reentrant fast mutexes.  This is the implementation
     * using the C++11 mutex.
     */
    class fast_mutex
    {
        std::mutex _M_mtx_impl;
#       if _FAST_MUTEX_CHECK_INITIALIZATION
        bool _M_initialized;
#       endif
#       ifdef _DEBUG
        bool _M_locked;
#       endif
    public:
        fast_mutex()
#       ifdef _DEBUG
            : _M_locked(false)
#       endif
        {
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            _M_initialized = true;
#       endif
        }
        ~fast_mutex()
        {
            _FAST_MUTEX_ASSERT(!_M_locked, "~fast_mutex(): still locked");
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            _M_initialized = false;
#       endif
        }
        void lock()
        {
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            if (!_M_initialized)
                return;
#       endif
            _M_mtx_impl.lock();
#       ifdef _DEBUG
            _FAST_MUTEX_ASSERT(!_M_locked, "lock(): already locked");
            _M_locked = true;
#       endif
        }
        void unlock()
        {
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            if (!_M_initialized)
                return;
#       endif
#       ifdef _DEBUG
            _FAST_MUTEX_ASSERT(_M_locked, "unlock(): not locked");
            _M_locked = false;
#       endif
            _M_mtx_impl.unlock();
        }
    private:
        fast_mutex(const fast_mutex&);
        fast_mutex& operator=(const fast_mutex&);
    };
NVWA_NAMESPACE_END
# endif // NVWA_USE_CXX11_MUTEX != 0

# if defined(_PTHREADS) && NVWA_USE_CXX11_MUTEX == 0
#   include <pthread.h>
NVWA_NAMESPACE_BEGIN
/**
 * Macro alias to `volatile' semantics.  Here it is truly volatile since
 * it is in a multi-threaded (POSIX threads) environment.
 */
#   define __VOLATILE volatile
    /**
     * Class for non-reentrant fast mutexes.  This is the implementation
     * for POSIX threads.
     */
    class fast_mutex
    {
        pthread_mutex_t _M_mtx_impl;
#       if _FAST_MUTEX_CHECK_INITIALIZATION
        bool _M_initialized;
#       endif
#       ifdef _DEBUG
        bool _M_locked;
#       endif
    public:
        fast_mutex()
#       ifdef _DEBUG
            : _M_locked(false)
#       endif
        {
            ::pthread_mutex_init(&_M_mtx_impl, NULL);
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            _M_initialized = true;
#       endif
        }
        ~fast_mutex()
        {
            _FAST_MUTEX_ASSERT(!_M_locked, "~fast_mutex(): still locked");
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            _M_initialized = false;
#       endif
            ::pthread_mutex_destroy(&_M_mtx_impl);
        }
        void lock()
        {
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            if (!_M_initialized)
                return;
#       endif
            ::pthread_mutex_lock(&_M_mtx_impl);
#       ifdef _DEBUG
            // The following assertion should _always_ be true for a
            // real `fast' pthread_mutex.  However, this assertion can
            // help sometimes, when people forget to use `-lpthread' and
            // glibc provides an empty implementation.  Having this
            // assertion is also more consistent.
            _FAST_MUTEX_ASSERT(!_M_locked, "lock(): already locked");
            _M_locked = true;
#       endif
        }
        void unlock()
        {
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            if (!_M_initialized)
                return;
#       endif
#       ifdef _DEBUG
            _FAST_MUTEX_ASSERT(_M_locked, "unlock(): not locked");
            _M_locked = false;
#       endif
            ::pthread_mutex_unlock(&_M_mtx_impl);
        }
    private:
        fast_mutex(const fast_mutex&);
        fast_mutex& operator=(const fast_mutex&);
    };
NVWA_NAMESPACE_END
# endif // _PTHREADS

# if defined(_WIN32THREADS) && NVWA_USE_CXX11_MUTEX == 0
#   ifndef WIN32_LEAN_AND_MEAN
#     define WIN32_LEAN_AND_MEAN
#   endif /* WIN32_LEAN_AND_MEAN */
#   include <windows.h>
NVWA_NAMESPACE_BEGIN
/**
 * Macro alias to `volatile' semantics.  Here it is truly volatile since
 * it is in a multi-threaded (Win32 threads) environment.
 */
#   define __VOLATILE volatile
    /**
     * Class for non-reentrant fast mutexes.  This is the implementation
     * for Win32 threads.
     */
    class fast_mutex
    {
        CRITICAL_SECTION _M_mtx_impl;
#       if _FAST_MUTEX_CHECK_INITIALIZATION
        bool _M_initialized;
#       endif
#       ifdef _DEBUG
        bool _M_locked;
#       endif
    public:
        fast_mutex()
#       ifdef _DEBUG
            : _M_locked(false)
#       endif
        {
            ::InitializeCriticalSection(&_M_mtx_impl);
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            _M_initialized = true;
#       endif
        }
        ~fast_mutex()
        {
            _FAST_MUTEX_ASSERT(!_M_locked, "~fast_mutex(): still locked");
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            _M_initialized = false;
#       endif
            ::DeleteCriticalSection(&_M_mtx_impl);
        }
        void lock()
        {
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            if (!_M_initialized)
                return;
#       endif
            ::EnterCriticalSection(&_M_mtx_impl);
#       ifdef _DEBUG
            _FAST_MUTEX_ASSERT(!_M_locked, "lock(): already locked");
            _M_locked = true;
#       endif
        }
        void unlock()
        {
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            if (!_M_initialized)
                return;
#       endif
#       ifdef _DEBUG
            _FAST_MUTEX_ASSERT(_M_locked, "unlock(): not locked");
            _M_locked = false;
#       endif
            ::LeaveCriticalSection(&_M_mtx_impl);
        }
    private:
        fast_mutex(const fast_mutex&);
        fast_mutex& operator=(const fast_mutex&);
    };
NVWA_NAMESPACE_END
# endif // _WIN32THREADS

# ifdef _NOTHREADS
NVWA_NAMESPACE_BEGIN
/**
 * Macro alias to `volatile' semantics.  Here it is not truly volatile
 * since it is in a single-threaded environment.
 */
#   define __VOLATILE
    /**
     * Class for non-reentrant fast mutexes.  This is the null
     * implementation for single-threaded environments.
     */
    class fast_mutex
    {
#       ifdef _DEBUG
        bool _M_locked;
#       endif
    public:
        fast_mutex()
#       ifdef _DEBUG
            : _M_locked(false)
#       endif
        {
        }
        ~fast_mutex()
        {
            _FAST_MUTEX_ASSERT(!_M_locked, "~fast_mutex(): still locked");
        }
        void lock()
        {
#       ifdef _DEBUG
            _FAST_MUTEX_ASSERT(!_M_locked, "lock(): already locked");
            _M_locked = true;
#       endif
        }
        void unlock()
        {
#       ifdef _DEBUG
            _FAST_MUTEX_ASSERT(_M_locked, "unlock(): not locked");
            _M_locked = false;
#       endif
        }
    private:
        fast_mutex(const fast_mutex&);
        fast_mutex& operator=(const fast_mutex&);
    };
NVWA_NAMESPACE_END
# endif // _NOTHREADS

NVWA_NAMESPACE_BEGIN
/** An acquistion-on-initialization lock class based on fast_mutex. */
class fast_mutex_autolock
{
    fast_mutex& _M_mtx;
public:
    explicit fast_mutex_autolock(fast_mutex& mtx) : _M_mtx(mtx)
    {
        _M_mtx.lock();
    }
    ~fast_mutex_autolock()
    {
        _M_mtx.unlock();
    }
private:
    fast_mutex_autolock(const fast_mutex_autolock&);
    fast_mutex_autolock& operator=(const fast_mutex_autolock&);
};
NVWA_NAMESPACE_END

#endif // NVWA_FAST_MUTEX_H
