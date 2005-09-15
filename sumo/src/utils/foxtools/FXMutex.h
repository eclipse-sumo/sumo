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
#ifndef FXMUTEX_H
#define FXMUTEX_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "fxexdefs.h"

#ifndef FXLOCKABLE_H
#include "FXLockable.h"
#endif
namespace FXEX {
class FXCondition;

/**
 * Recursive mutual exclusion object.
 * Unlocks on destruction
 */
class FXMutex : public FXLockable {
  friend class FXCondition;

private:
  FXThreadMutex mutexHandle;

private:
  // dummy copy constructor and operator= to prevent copying
  FXMutex(const FXMutex&);
  FXMutex& operator=(const FXMutex&);

public:
  /// create me a mutex :-)
  FXMutex();

  /// lock mutex
  void lock();

  /// try to lock the mutex, within some period
  FXbool trylock(FXuint ms);

  /// release mutex lock
  void unlock();

  /// dtor
  virtual ~FXMutex();
  };

} // namespace FXEX
#endif // FXMUTEX_H
