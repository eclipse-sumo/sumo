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
 * @version 3.4, 2005/09/13
 * @author  Wu Yongwei
 *
 */

#ifndef _DEBUG_NEW_H
#define _DEBUG_NEW_H

#include <new>
#include <stdio.h>

/**
 * @def HAS_PLACEMENT_DELETE
 *
 * Macro to indicate whether placement delete operators are supported on
 * a certain compiler.  Some compilers, like Borland C++ Compiler 5.5.1
 * and Digital Mars Compiler 8.42, do not support them, and the user
 * must define this macro to \c 0 to make the program compile.  Also
 * note that in that case memory leakage will occur if an exception is
 * thrown in the initialization (constructor) of a dynamically created
 * object.
 */
#ifndef HAS_PLACEMENT_DELETE
#define HAS_PLACEMENT_DELETE 1
#endif

/**
 * @def _DEBUG_NEW_REDEFINE_NEW
 *
 * Macro to indicate whether redefinition of \c new is wanted.  If one
 * wants to define one's own <code>operator new</code>, to call
 * <code>operator new</code> directly, or to call placement \c new, it
 * should be defined to \c 0 to alter the default behaviour.  Unless, of
 * course, one is willing to take the trouble to write something like:
 * @code
 * # ifdef new
 * #   define _NEW_REDEFINED
 * #   undef new
 * # endif
 *
 * // Code that uses new is here
 *
 * # ifdef _NEW_REDEFINED
 * #   ifdef DEBUG_NEW
 * #     define new DEBUG_NEW
 * #   endif
 * #   undef _NEW_REDEFINED
 * # endif
 * @endcode
 */
#ifndef _DEBUG_NEW_REDEFINE_NEW
#define _DEBUG_NEW_REDEFINE_NEW 1
#endif

/* Prototypes */
int check_leaks();
void* operator new(size_t size, const char* file, int line);
void* operator new[](size_t size, const char* file, int line);
#if HAS_PLACEMENT_DELETE
void operator delete(void* pointer, const char* file, int line) throw();
void operator delete[](void* pointer, const char* file, int line) throw();
#endif
#if defined(_MSC_VER) && _MSC_VER < 1300
// MSVC 6 requires the following declarations; or the non-placement
// new[]/delete[] will not compile.
void* operator new[](size_t) throw(std::bad_alloc);
void operator delete[](void*) throw();
#endif

/* Control variables */
extern bool new_autocheck_flag; // default to true: call check_leaks() on exit
extern bool new_verbose_flag;   // default to false: no verbose information
extern FILE* new_output_fp;     // default to stderr: output to console
extern const char* new_progname;// default to NULL; should be assigned argv[0]

/**
 * @def DEBUG_NEW
 *
 * The macro to catch file/line information on allocation.  If
 * #_DEBUG_NEW_REDEFINE_NEW is not defined, one can use this macro
 * directly; otherwise \c new will be defined to it, and one must use
 * \c new instead.
 */
#define DEBUG_NEW new(__FILE__, __LINE__)

# if _DEBUG_NEW_REDEFINE_NEW
#   define new DEBUG_NEW
# endif
# ifdef _DEBUG_NEW_EMULATE_MALLOC
#   include <stdlib.h>
#   ifdef new
#     define malloc(s) ((void*)(new char[s]))
#   else
#     define malloc(s) ((void*)(DEBUG_NEW char[s]))
#   endif
#   define free(p) delete[] (char*)(p)
# endif

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

#endif // _DEBUG_NEW_H
