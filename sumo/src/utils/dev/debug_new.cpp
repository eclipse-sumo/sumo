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
 * @file    debug_new.cpp
 *
 * Implementation of debug versions of new and delete to check leakage.
 *
 * @version 3.12, 2005/07/13
 * @author  Wu Yongwei
 *
 */

#include <new>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __unix__
#include <alloca.h>
#endif
#ifdef _WIN32
#include <malloc.h>
#endif
#include "fast_mutex.h"
#include "static_assert.h"

#if !_FAST_MUTEX_CHECK_INITIALIZATION && !defined(_NOTHREADS)
#error "_FAST_MUTEX_CHECK_INITIALIZATION not set: check_leaks may not work"
#endif

/**
 * @def _DEBUG_NEW_ALIGNMENT
 *
 * The alignment requirement of allocated memory blocks.  It must be a
 * power of two.
 */
#ifndef _DEBUG_NEW_ALIGNMENT
#define _DEBUG_NEW_ALIGNMENT 16
#endif

/**
 * @def _DEBUG_NEW_CALLER_ADDRESS
 *
 * The expression to return the caller address.  #print_position will
 * later on use this address to print the position information of memory
 * operation points.
 */
#ifndef _DEBUG_NEW_CALLER_ADDRESS
#ifdef __GNUC__
#define _DEBUG_NEW_CALLER_ADDRESS __builtin_return_address(0)
#else
#define _DEBUG_NEW_CALLER_ADDRESS NULL
#endif
#endif

/**
 * @def _DEBUG_NEW_ERROR_ACTION
 *
 * The action to take when an error occurs.  The default behaviour is to
 * call \e abort, unless \c _DEBUG_NEW_ERROR_CRASH is defined, in which
 * case a segmentation fault will be triggered instead (which can be
 * useful on platforms like Windows that do not generate a core dump
 * when \e abort is called).
 */
#ifndef _DEBUG_NEW_ERROR_ACTION
#ifndef _DEBUG_NEW_ERROR_CRASH
#define _DEBUG_NEW_ERROR_ACTION abort()
#else
#define _DEBUG_NEW_ERROR_ACTION do { *((char*)0) = 0; abort(); } while (0)
#endif
#endif

/**
 * @def _DEBUG_NEW_FILENAME_LEN
 *
 * The length of file name stored if greater than zero.  If it is zero,
 * only a const char pointer will be stored.  Currently the default
 * behaviour is to copy the file name, because I found that the exit
 * leakage check cannot access the address of the file name sometimes
 * (in my case, a core dump will occur when trying to access the file
 * name in a shared library after a \c SIGINT).  If the default value is
 * too small for you, try defining it to \c 52, which makes the size of
 * new_ptr_list_t 64 (it is 32 by default) on 32-bit platforms.
 */
#ifndef _DEBUG_NEW_FILENAME_LEN
#define _DEBUG_NEW_FILENAME_LEN  200
#endif

/**
 * @def _DEBUG_NEW_HASHTABLESIZE
 *
 * The size of the hash bucket for the table to store pointers to
 * allocated memory.  To ensure good performance, always make it a power
 * of two.
 */
#ifndef _DEBUG_NEW_HASHTABLESIZE
#define _DEBUG_NEW_HASHTABLESIZE 16384
#endif

/**
 * @def _DEBUG_NEW_HASH
 *
 * The hash function for the pointers.  This one has good performance in
 * test for me.
 */
#ifndef _DEBUG_NEW_HASH
#define _DEBUG_NEW_HASH(p) (((size_t)(p) >> 8) % _DEBUG_NEW_HASHTABLESIZE)
#endif

/**
 * @def _DEBUG_NEW_PROGNAME
 *
 * The program (executable) name to be set at compile time.  It is
 * better to assign the full program path to #new_progname in \e main
 * (at run time) than to use this (compile-time) macro, but this macro
 * serves well as a quick hack.  Note also that SUMOReal quotation marks
 * need to be used around the program name, i.e., one should specify a
 * command-line option like <code>-D_DEBUG_NEW_PROGNAME=\"a.out\"</code>
 * in \e bash, or <code>-D_DEBUG_NEW_PROGNAME=\"a.exe\"</code> in the
 * Windows command prompt.
 */
#ifndef _DEBUG_NEW_PROGNAME
#define _DEBUG_NEW_PROGNAME NULL
#endif

/**
 * @def _DEBUG_NEW_USE_ADDR2LINE
 *
 * Whether to use \e addr2line to convert a caller address to file/line
 * information.  Defining it to a non-zero value will enable the
 * conversion (automatically done if GCC is detected).  Defining it to
 * zero will disable the conversion.
 */
#ifndef _DEBUG_NEW_USE_ADDR2LINE
#ifdef __GNUC__
#define _DEBUG_NEW_USE_ADDR2LINE 1
#else
#define _DEBUG_NEW_USE_ADDR2LINE 0
#endif
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4073)  // #pragma init_seg(lib) used
#pragma warning(disable: 4290)  // C++ exception specification ignored
#pragma init_seg(lib)
#endif

/**
 * This macro is defined when no redefinition of \c new is wanted.  This
 * is to ensure that overloading and direct calling of <code>operator
 * new</code> is possible.
 */
#define _DEBUG_NEW_NO_NEW_REDEFINITION
#include "debug_new.h"

/**
 * Gets the aligned value of memory block size.
 */
#define align(s) \
        (((s) + _DEBUG_NEW_ALIGNMENT - 1) & ~(_DEBUG_NEW_ALIGNMENT - 1))

/**
 * Structure to store the position information where \c new occurs.
 */
struct new_ptr_list_t
{
    new_ptr_list_t*     next;
    union
    {
#if _DEBUG_NEW_FILENAME_LEN == 0
    const char*         file;
#else
    char                file[_DEBUG_NEW_FILENAME_LEN];
#endif
    void*               addr;
    };
    int                 line;
    size_t              size;
};

/**
 * The extra memory allocated by <code>operator new</code>.
 */
const int aligned_list_item_size = align(sizeof(new_ptr_list_t));

/**
 * Array of pointer lists of a hash value.
 */
static new_ptr_list_t* new_ptr_list[_DEBUG_NEW_HASHTABLESIZE];

/**
 * Array of mutex guards to protect simultaneous access to the pointer
 * lists of a hash value.
 */
static fast_mutex new_ptr_lock[_DEBUG_NEW_HASHTABLESIZE];

/**
 * The mutex guard to protect simultaneous output to #new_output_fp.
 */
static fast_mutex new_output_lock;

/**
 * Total memory allocated in bytes.
 */
static size_t total_mem_alloc = 0;

/**
 * Flag to control whether #check_leaks will be automatically called on
 * program exit.
 */
bool new_autocheck_flag = true;

/**
 * Flag to control whether verbose messages are output.
 */
bool new_verbose_flag = false;

/**
 * Pointer to the output stream.  The default output is \e stderr, and
 * one may change it to a user stream if needed (say, #new_verbose_flag
 * is \c true and there are a lot of (de)allocations).
 */
FILE* new_output_fp = stdout;

/**
 * Pointer to the program name.  Its initial value is the macro
 * #_DEBUG_NEW_PROGNAME.  You should try to assign the program path to
 * it early in your application.  Assigning <code>argv[0]</code> to it
 * in \e main is one way.  If you use \e bash or \e ksh (or similar),
 * the following statement is probably what you want:
 * `<code>new_progname = getenv("_");</code>'.
 */
const char* new_progname = _DEBUG_NEW_PROGNAME;

#if _DEBUG_NEW_USE_ADDR2LINE
/**
 * Tries printing the position information from an instruction address.
 * This is the version that uses \e addr2line.
 *
 * @param addr  the instruction address to convert and print
 * @return      \c true if the address is converted successfully (and
 *              the result is printed); \c false if no useful
 *              information is got (and nothing is printed)
 */
static bool print_position_from_addr(const void* addr)
{
    static const void* last_addr = NULL;
    static char last_info[256] = "";
    if (addr == last_addr)
    {
        fprintf(new_output_fp, "%s", last_info);
        return true;
    }
    if (new_progname)
    {
        const char addr2line_cmd[] = "addr2line -e ";
#if   defined(__CYGWIN__) || defined(_WIN32)
        const int  exeext_len = 4;
#else
        const int  exeext_len = 0;
#endif
#if  !defined(__CYGWIN__) && defined(__unix__)
        const char ignore_err[] = " 2>/dev/null";
#elif defined(__CYGWIN__) || \
        (defined(_WIN32) && defined(WINVER) && WINVER >= 0x0500)
        const char ignore_err[] = " 2>nul";
#else
        const char ignore_err[] = "";
#endif
        char* cmd = (char*)alloca(strlen(new_progname)
                                  + exeext_len
                                  + sizeof addr2line_cmd - 1
                                  + sizeof ignore_err - 1
                                  + sizeof(void*) * 2
                                  + 4 /* SP + "0x" + null */);
        strcpy(cmd, addr2line_cmd);
        strcpy(cmd + sizeof addr2line_cmd - 1, new_progname);
        size_t len = strlen(cmd);
#if   defined(__CYGWIN__) || defined(_WIN32)
        if (len <= 4
                || (strcmp(cmd + len - 4, ".exe") != 0 &&
                    strcmp(cmd + len - 4, ".EXE") != 0))
        {
            strcpy(cmd + len, ".exe");
            len += 4;
        }
#endif
        sprintf(cmd + len, " %p%s", addr, ignore_err);
        FILE* fp = popen(cmd, "r");
        if (fp)
        {
            char buffer[sizeof last_info] = "";
            len = 0;
            if (fgets(buffer, sizeof buffer, fp))
            {
                len = strlen(buffer);
                if (buffer[len - 1] == '\n')
                    buffer[--len] = '\0';
            }
            int res = pclose(fp);
            // Display the file/line information only if the command
            // is executed successfully and the output points to a
            // valid position
            if (res == 0 && len > 0 && !
                    (buffer[len - 1] == '0' && buffer[len - 2] == ':'))
            {
                fprintf(new_output_fp, "%s", buffer);
                last_addr = addr;
                strcpy(last_info, buffer);
                return true;
            }
        }
    }
    return false;
}
#else
/**
 * Tries printing the position information from an instruction address.
 * This is the stub version that does nothing at all.
 *
 * @return      \c false always
 */
static bool print_position_from_addr(const void*)
{
    return false;
}
#endif // _DEBUG_NEW_USE_ADDR2LINE

/**
 * Prints the position information of a memory operation point.  When \c
 * _DEBUG_NEW_USE_ADDR2LINE is defined to a non-zero value, this
 * function will try to convert a given caller address to file/line
 * information with \e addr2line.
 *
 * @param ptr   source file name if \e line is non-zero; caller address
 *              otherwise
 * @param line  source line number if non-zero; indication that \e ptr
 *              is the caller address otherwise
 */
static void print_position(const void* ptr, int line)
{
    line &= ~INT_MIN;       // Result from new[] if highest bit set: Ignore
    if (line != 0)          // Is file/line information present?
    {
        fprintf(new_output_fp, "%s:%d", (const char*)ptr, line);
    }
    else if (ptr != NULL)   // Is caller address present?
    {
        if (!print_position_from_addr(ptr)) // Fail to get source position?
            fprintf(new_output_fp, "%p", ptr);
    }
    else                    // No information is present
    {
        fprintf(new_output_fp, "<Unknown>");
    }
}

/**
 * Searches for the raw pointer given a user pointer.  The term `raw
 * pointer' here refers to the pointer to the pointer to originally
 * <em>malloc</em>'d memory.
 *
 * @param pointer       user pointer to search for
 * @param hash_index    hash index of the user pointer
 * @return              the raw pointer if searching is successful; or
 *                      \c NULL otherwise
 */
static new_ptr_list_t** search_pointer(void* pointer, size_t hash_index)
{
    new_ptr_list_t** raw_ptr = &new_ptr_list[hash_index];
    while (*raw_ptr)
    {
        if ((char*)*raw_ptr + aligned_list_item_size == pointer)
        {
            return raw_ptr;
        }
        raw_ptr = &(*raw_ptr)->next;
    }
    return NULL;
}

/**
 * Frees memory and adjusts pointers relating to a raw pointer.  If the
 * highest bit of \c line (set from a previous <code>new[]</code> call)
 * does not agree with \c array_mode, program will abort with an error
 * message.
 *
 * @param raw_ptr       raw pointer to free
 * @param addr          pointer to the caller
 * @param array_mode    flag indicating whether it is invoked by a
 *                      <code>delete[]</code> call
 */
static void free_pointer(new_ptr_list_t** raw_ptr, void* addr, bool array_mode)
{
    new_ptr_list_t* ptr = *raw_ptr;
    int array_mode_mismatch = array_mode ^ ((ptr->line & INT_MIN) != 0);
    if (array_mode_mismatch)
    {
        const char* msg;
        if (array_mode)
            msg = "delete[] after new";
        else
            msg = "delete after new[]";
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "%s: pointer %p (size %u)\n\tat ",
                msg,
                (char*)ptr + aligned_list_item_size,
                ptr->size);
        print_position(addr, 0);
        fprintf(new_output_fp, "\n\toriginally allocated at ");
        if ((ptr->line & ~INT_MIN) != 0)
            print_position(ptr->file, ptr->line);
        else
            print_position(ptr->addr, ptr->line);
        fprintf(new_output_fp, "\n");
        fflush(new_output_fp);
        _DEBUG_NEW_ERROR_ACTION;
    }
    total_mem_alloc -= ptr->size;
    if (new_verbose_flag)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "delete: freeing  %p (size %u, %u bytes still allocated)\n",
                (char*)ptr + aligned_list_item_size,
                ptr->size, total_mem_alloc);
    }
    *raw_ptr = ptr->next;
    free(ptr);
    return;
}

/**
 * Checks for memory leaks.
 *
 * @return  zero if no leakage is found; the number of leaks otherwise
 */
int check_leaks()
{
    int leak_cnt = 0;
    for (int i = 0; i < _DEBUG_NEW_HASHTABLESIZE; ++i)
    {
        fast_mutex_autolock lock(new_ptr_lock[i]);
        new_ptr_list_t* ptr = new_ptr_list[i];
        if (ptr == NULL)
            continue;
        while (ptr)
        {
            fast_mutex_autolock lock(new_output_lock);
            fprintf(new_output_fp,
                    "Leaked object at %p (size %u, ",
                    (char*)ptr + aligned_list_item_size,
                    ptr->size);
            if ((ptr->line & ~INT_MIN) != 0)
                print_position(ptr->file, ptr->line);
            else
                print_position(ptr->addr, ptr->line);
            fprintf(new_output_fp, ")\n");
            ptr = ptr->next;
            ++leak_cnt;
        }
    }
    return leak_cnt;
}

void* operator new(size_t size, const char* file, int line)
{
    assert((line & INT_MIN) == 0);
    static_assert((_DEBUG_NEW_ALIGNMENT & (_DEBUG_NEW_ALIGNMENT - 1)) == 0,
                  Alignment_must_be_power_of_two);
    size_t s = size + aligned_list_item_size;
    new_ptr_list_t* ptr = (new_ptr_list_t*)malloc(s);
    if (ptr == NULL)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "new:  out of memory when allocating %u bytes\n",
                size);
        fflush(new_output_fp);
        _DEBUG_NEW_ERROR_ACTION;
    }
    void* pointer = (char*)ptr + aligned_list_item_size;
    size_t hash_index = _DEBUG_NEW_HASH(pointer);
#if _DEBUG_NEW_FILENAME_LEN == 0
    ptr->file = file;
#else
    if (line)
        strncpy(ptr->file, file, _DEBUG_NEW_FILENAME_LEN - 1)
                [_DEBUG_NEW_FILENAME_LEN - 1] = '\0';
    else
        ptr->addr = (void*)file;
#endif
    ptr->line = line;
    ptr->size = size;
    {
        fast_mutex_autolock lock(new_ptr_lock[hash_index]);
        ptr->next = new_ptr_list[hash_index];
        new_ptr_list[hash_index] = ptr;
    }
    if (new_verbose_flag)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "new:  allocated  %p (size %u, ",
                pointer, size);
        if (line != 0)
            print_position(ptr->file, ptr->line);
        else
            print_position(ptr->addr, ptr->line);
        fprintf(new_output_fp, ")\n");
    }
    total_mem_alloc += size;
    return pointer;
}

void* operator new[](size_t size, const char* file, int line)
{
    void* pointer = operator new(size, file, line);
    new_ptr_list_t* ptr =
            (new_ptr_list_t*)((char*)pointer - aligned_list_item_size);
    assert((ptr->line & INT_MIN) == 0);
    ptr->line |= INT_MIN;   // Result from new[] if highest bit set: Set
    return pointer;
}

void* operator new(size_t size) throw(std::bad_alloc)
{
    return operator new(size, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0);
}

void* operator new[](size_t size) throw(std::bad_alloc)
{
    return operator new[](size, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0);
}

#if !defined(__BORLANDC__) || __BORLANDC__ > 0x551
void* operator new(size_t size, const std::nothrow_t&) throw()
{
    return operator new(size, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0);
}

void* operator new[](size_t size, const std::nothrow_t&) throw()
{
    return operator new[](size, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0);
}
#endif

void operator delete(void* pointer) throw()
{
    if (pointer == NULL)
        return;
    size_t hash_index = _DEBUG_NEW_HASH(pointer);
    fast_mutex_autolock lock(new_ptr_lock[hash_index]);
    new_ptr_list_t** raw_ptr = search_pointer(pointer, hash_index);
    if (raw_ptr == NULL)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp, "delete: invalid pointer %p at ", pointer);
        print_position(_DEBUG_NEW_CALLER_ADDRESS, 0);
        fprintf(new_output_fp, "\n");
        fflush(new_output_fp);
        _DEBUG_NEW_ERROR_ACTION;
    }
    free_pointer(raw_ptr, _DEBUG_NEW_CALLER_ADDRESS, false);
}

void operator delete[](void* pointer) throw()
{
    if (pointer == NULL)
        return;
    size_t hash_index = _DEBUG_NEW_HASH(pointer);
    fast_mutex_autolock lock(new_ptr_lock[hash_index]);
    new_ptr_list_t** raw_ptr = search_pointer(pointer, hash_index);
    if (raw_ptr == NULL)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp, "delete[]: invalid pointer %p at ", pointer);
        print_position(_DEBUG_NEW_CALLER_ADDRESS, 0);
        fprintf(new_output_fp, "\n");
        fflush(new_output_fp);
        _DEBUG_NEW_ERROR_ACTION;
    }
    free_pointer(raw_ptr, _DEBUG_NEW_CALLER_ADDRESS, true);
}

// Some older compilers like Borland C++ Compiler 5.5.1 and Digital Mars
// Compiler 8.29 do not support placement delete operators.
// NO_PLACEMENT_DELETE needs to be defined when using such compilers.
// Also note that in that case memory leakage will occur if an exception
// is thrown in the initialization (constructor) of a dynamically
// created object.
#ifndef NO_PLACEMENT_DELETE
void operator delete(void* pointer, const char* file, int line) throw()
{
    if (new_verbose_flag)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "info: exception thrown on initializing object at %p (",
                pointer);
        print_position(file, line);
        fprintf(new_output_fp, ")\n");
    }
    operator delete(pointer);
}

void operator delete[](void* pointer, const char* file, int line) throw()
{
    if (new_verbose_flag)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "info: exception thrown on initializing objects at %p (",
                pointer);
        print_position(file, line);
        fprintf(new_output_fp, ")\n");
    }
    operator delete[](pointer);
}

void operator delete(void* pointer, const std::nothrow_t&) throw()
{
    operator delete(pointer, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0);
}

void operator delete[](void* pointer, const std::nothrow_t&) throw()
{
    operator delete[](pointer, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0);
}
#endif // NO_PLACEMENT_DELETE

int __debug_new_counter::_count = 0;

/**
 * Constructor to increment the count.
 */
__debug_new_counter::__debug_new_counter()
{
    ++_count;
}

/**
 * Destructor to decrement the count.  When the count is zero,
 * #check_leaks will be called.
 */
__debug_new_counter::~__debug_new_counter()
{
    if (--_count == 0 && new_autocheck_flag)
        if (check_leaks())
        {
            new_verbose_flag = true;
#if defined(__GNUC__) && __GNUC__ >= 3
            if (!getenv("GLIBCPP_FORCE_NEW") && !getenv("GLIBCXX_FORCE_NEW"))
                fprintf(new_output_fp,
"*** WARNING:  GCC 3 or later is detected, please make sure the\n"
"    environment variable GLIBCPP_FORCE_NEW (GCC 3.2 and 3.3) or\n"
"    GLIBCXX_FORCE_NEW (GCC 3.4 and later) is defined.  Check the\n"
"    README file for details.\n");
#endif
        }
}

#endif // _DEBUG
