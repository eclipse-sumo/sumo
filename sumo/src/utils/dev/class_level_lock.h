#ifdef _DEBUG

// -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
// vim:tabstop=4:shiftwidth=4:expandtab:

/*
 * Copyright (C) 2004-2005 Wu Yongwei <adah at users dot sourceforge dot net>
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
 * @file    class_level_lock.h
 *
 * In essence Loki ClassLevelLockable re-engineered to use a fast_mutex class.
 *
 * @version 1.11, 2005/06/11
 * @author  Wu Yongwei
 *
 */

#ifndef _CLASS_LEVEL_LOCK_H
#define _CLASS_LEVEL_LOCK_H

#include "fast_mutex.h"

# ifdef _NOTHREADS
    /**
     * Helper class for class-level locking.  This is the
     * single-threaded implementation.
     */
    template <class _Host, bool _RealLock = false>
    class class_level_lock
    {
    public:
        /** Type that provides locking/unlocking semantics. */
        class lock
        {
        public:
            lock() {}
        };

        typedef _Host volatile_type;
    };
# else
    /**
     * Helper class for class-level locking.  This is the multi-threaded
     * implementation.  The main departure from Loki ClassLevelLockable
     * is that there is an additional template parameter which can make
     * the lock not lock at all even in multi-threaded environments.
     * See static_mem_pool.h for real usage.
     */
    template <class _Host, bool _RealLock = true>
    class class_level_lock
    {
        static fast_mutex _S_mtx;

    public:
        class lock;
        friend class lock;

        /** Type that provides locking/unlocking semantics. */
        class lock
        {
            lock(const lock&);
            lock& operator=(const lock&);
        public:
            lock()
            {
                if (_RealLock)
                    _S_mtx.lock();
            }
            ~lock()
            {
                if (_RealLock)
                    _S_mtx.unlock();
            }
        };

        typedef volatile _Host volatile_type;
    };

#   ifdef HAS_CLASS_TEMPLATE_PARTIAL_SPECIALIZATION
    /** Partial specialization that makes null locking. */
    template <class _Host>
    class class_level_lock<_Host, false>
    {
    public:
        /** Type that provides locking/unlocking semantics. */
        class lock
        {
        public:
            lock() {}
        };

        typedef _Host volatile_type;
    };
#   endif // HAS_CLASS_TEMPLATE_PARTIAL_SPECIALIZATION

    template <class _Host, bool _RealLock>
    fast_mutex class_level_lock<_Host, _RealLock>::_S_mtx;
# endif // _NOTHREADS

#endif // _CLASS_LEVEL_LOCK_H

#endif // _DEBUG
