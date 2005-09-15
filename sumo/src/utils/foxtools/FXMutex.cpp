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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <fxver.h>
#include <xincs.h>
#include <fxdefs.h>
using namespace FX;
//#include "exincs.h"
//#include "fxexdefs.h"
#include "FXMutex.h"

#ifndef WIN32
#include <pthread.h>
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

using namespace FXEX;
namespace FXEX {

// FXMutex constructor
FXMutex::FXMutex() : FXLockable() {
#ifndef WIN32
  FXint status=0;
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  status=pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
  FXASSERT(status==0);
  FXMALLOC(&mutexHandle,pthread_mutex_t,1);
  status=pthread_mutex_init((pthread_mutex_t*)mutexHandle,&attr);
  FXASSERT(status==0);
  pthread_mutexattr_destroy(&attr);
#else
  mutexHandle=CreateMutex(NULL,FALSE,NULL);
  FXASSERT(mutexHandle!=NULL);
#endif
  }

// Note: lock_ is not safe here because it is not protected, but
//       if you are causing the destructor to be executed while
//       some other thread is accessing the mutexHandle, then you have
//       a design flaw in your program, and so it should crash!
FXMutex::~FXMutex() {
  if (lock_) fxerror("FXMutex: mutex still locked\n");
#if !defined(WIN32)
  pthread_mutex_destroy((pthread_mutex_t*)mutexHandle);
  FXFREE(&mutexHandle);
#else
  CloseHandle(mutexHandle);
#endif
  }

// lock_ is safe because we dont increment it until we
// have entered the locked state - cha-ching, correct
void FXMutex::lock() {
#if !defined(WIN32)
  pthread_mutex_lock((pthread_mutex_t*)mutexHandle);
#else
  WaitForSingleObject(mutexHandle,INFINITE);
#endif
  lock_++;
  }

// try to lock mutex - lock_ is safe because we dont increment it
// unles we enter the locked state
FXbool FXMutex::trylock(FXuint ms){
#ifndef WIN32
  struct timespec due={0,0};
      due.tv_nsec = ms * 1000000;
  if (pthread_mutex_timedlock((pthread_mutex_t*)mutexHandle,&due)==0){ lock_++; return TRUE; }
  return FALSE;
#else
  DWORD result=WaitForSingleObject(mutexHandle,ms);
  if (result==WAIT_OBJECT_0){ lock_++; return TRUE; }
  return FALSE;
#endif
  }

// lock_ is safe because we decrement it, before leaving the locked state
void FXMutex::unlock() {
  lock_--;
#if !defined(WIN32)
  pthread_mutex_unlock((pthread_mutex_t*)mutexHandle);
#else
  ReleaseMutex(mutexHandle);
#endif
  }

}
