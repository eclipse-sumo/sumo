/********************************************************************************
*                                                                               *
*               Mutal exclusion object (required for threads)                   *
*                                                                               *
*********************************************************************************
* Copyright (C) 2003 by Mathew Robertson.   All Rights Reserved.                *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
********************************************************************************/
#ifndef MFXMUTEX_H
#define MFXMUTEX_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/common/AbstractMutex.h>

#ifndef WIN32
typedef void*		FXThreadMutex;        // handle to a mutex
#else
#include <windows.h>
typedef HANDLE	 	FXThreadMutex;        // handle to a mutex
#endif




/**
 * Recursive mutual exclusion object.
 * Unlocks on destruction
 */
class MFXMutex : public AbstractMutex {
public:
    /// create me a mutex :-)
    MFXMutex();

    /// lock mutex
    void lock();

    /// release mutex lock
    void unlock();

    /// dtor
    virtual ~MFXMutex();

    inline FXbool locked() {
        return lock_?TRUE:FALSE;
    }

protected:
    FXuint lock_;          // lock count

private:
    FXThreadMutex mutexHandle;

private:
    // dummy copy constructor and operator= to prevent copying
    MFXMutex(const MFXMutex&);
    MFXMutex& operator=(const MFXMutex&);

};

#endif // FXMUTEX_H
