/********************************************************************************
*                                                                               *
*               Lockable object base type (used in threading)                   *
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
#ifndef FXLOCKABLE_H
#define FXLOCKABLE_H

namespace FXEX {

/**
 * A lockable class is a base type for lockable objects.
 * We use lock/unlock & acquire/release, to make it easier on programmers :-)
 */
class FXLockable {

protected:
  FXuint lock_;          // lock count

protected:
  FXLockable();

public:
  /// are we locked?
  inline FXbool locked() { return lock_?TRUE:FALSE; }

  /// lock mutex
  virtual void lock()=0;

  /// try to lock, within some period
  virtual FXbool trylock(FXuint ms)=0;

  /// release mutex lock
  virtual void unlock()=0;

  /// lock mutex
  inline void acquire() { lock(); }

  /// try to lock
  inline FXbool tryacquire(FXint ms) { return trylock(ms); }

  /// release mutex lock
  inline void release() { unlock(); }

  /// dtor
  virtual ~FXLockable(){}
  };

} // namespace FXEX
#endif // FXLOCKABLE_H
