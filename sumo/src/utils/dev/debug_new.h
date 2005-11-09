#ifdef _DEBUG
#ifdef CHECK_MEMORY_LEAKS
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
 * @file    debug_new.h
 *
 * Header file for checking leaks caused by unmatched new/delete.
 *
 * @version 3.2, 2005/05/06
 * @author  Wu Yongwei
 *
 */

#ifndef _DEBUG_NEW_H
#define _DEBUG_NEW_H

#include <new>
#include <stdio.h>

/* Prototypes */
int check_leaks();
void* operator new(size_t size, const char* file, int line);
void* operator new[](size_t size, const char* file, int line);
#ifndef NO_PLACEMENT_DELETE
void operator delete(void* pointer, const char* file, int line) throw();
void operator delete[](void* pointer, const char* file, int line) throw();
#endif // NO_PLACEMENT_DELETE
#if defined(_MSC_VER) && _MSC_VER < 1300
// MSVC 6 requires the following declarations; or the non-placement
// new[]/delete[] will not compile.
void* operator new[](size_t) throw(std::bad_alloc);
void operator delete[](void*) throw();
#endif // MSVC 6

/* Macros */
#ifndef _DEBUG_NEW_NO_NEW_REDEFINITION
#define new DEBUG_NEW
#define DEBUG_NEW new(__FILE__, __LINE__)
#define debug_new new
#else
#define debug_new new(__FILE__, __LINE__)
#endif // _DEBUG_NEW_NO_NEW_REDEFINITION
#ifdef _DEBUG_NEW_EMULATE_MALLOC
#include <stdlib.h>
#define malloc(s) ((void*)(debug_new char[s]))
#define free(p) delete[] (char*)(p)
#endif // _DEBUG_NEW_EMULATE_MALLOC

/* Control variables */
extern bool new_autocheck_flag; // default to true: call check_leaks() on exit
extern bool new_verbose_flag;   // default to false: no verbose information
extern FILE* new_output_fp;     // default to stderr: output to console
extern const char* new_progname;// default to NULL; should be assigned argv[0]

/** Counter class for on-exit leakage check. */
class __debug_new_counter
{
    static int _count;
public:
    __debug_new_counter();
    ~__debug_new_counter();
};
/** Counting object for each file including debug_new.h. */
static __debug_new_counter __debug_new_count;

/**
 * @def debug_new
 *
 * The always available macro to catch file/line information on
 * allocation.  When #_DEBUG_NEW_NO_NEW_REDEFINITION is defined, \c
 * new will not be defined as a macro.  At that time, you can only use
 * the \c debug_new macro as a substitute.
 */

#endif // _DEBUG_NEW_H

#endif // CHECK_MEMORY_LEAKS
#endif // _DEBUG
