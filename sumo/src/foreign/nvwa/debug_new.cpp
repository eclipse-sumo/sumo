// -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
// vim:tabstop=4:shiftwidth=4:expandtab:
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif
#ifdef CHECK_MEMORY_LEAKS

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
 * @file  debug_new.cpp
 *
 * Implementation of debug versions of new and delete to check leakage.
 *
 * @date  2013-12-31
 */

#include <new>                  // std::bad_alloc/nothrow_t
#include <assert.h>             // assert
#include <stdio.h>              // fprintf/stderr
#include <stdlib.h>             // abort
#include <string.h>             // strcpy/strncpy/sprintf
#if defined(__unix__) || defined(__unix) || \
        (defined(__APPLE__) && defined(__MACH__))
#include <alloca.h>             // alloca
#endif
#ifdef _WIN32
#include <malloc.h>             // alloca
#endif
#include "_nvwa.h"              // NVWA_NAMESPACE_*
#include "c++11.h"              // _NOEXCEPT
#include "fast_mutex.h"         // nvwa::fast_mutex
#include "static_assert.h"      // STATIC_ASSERT

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
 * The expression to return the caller address.  nvwa#print_position will
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
 * value is non-zero (thus to copy the file name) on non-Windows
 * platforms, because I once found that the exit leakage check could not
 * access the address of the file name on Linux (in my case, a core dump
 * occurred when check_leaks tried to access the file name in a shared
 * library after a \c SIGINT).  This value makes the size of
 * new_ptr_list_t \c 64 on non-Windows 32-bit platforms.
 */
#ifndef _DEBUG_NEW_FILENAME_LEN
#ifdef _WIN32
#define _DEBUG_NEW_FILENAME_LEN 0
#else
#define _DEBUG_NEW_FILENAME_LEN 44
#endif
#endif

/**
 * @def _DEBUG_NEW_PROGNAME
 *
 * The program (executable) name to be set at compile time.  It is
 * better to assign the full program path to nvwa#new_progname in \e main
 * (at run time) than to use this (compile-time) macro, but this macro
 * serves well as a quick hack.  Note also that double quotation marks
 * need to be used around the program name, i.e., one should specify a
 * command-line option like <code>-D_DEBUG_NEW_PROGNAME=\\"a.out\"</code>
 * in \e bash, or <code>-D_DEBUG_NEW_PROGNAME=\\"a.exe\"</code> in the
 * Windows command prompt.
 */
#ifndef _DEBUG_NEW_PROGNAME
#define _DEBUG_NEW_PROGNAME NULL
#endif

/**
 * @def _DEBUG_NEW_STD_OPER_NEW
 *
 * Macro to indicate whether the standard-conformant behaviour of
 * <code>operator new</code> is wanted.  It is on by default now, but
 * the user may set it to \c 0 to revert to the old behaviour.
 */
#ifndef _DEBUG_NEW_STD_OPER_NEW
#define _DEBUG_NEW_STD_OPER_NEW 1
#endif

/**
 * @def _DEBUG_NEW_TAILCHECK
 *
 * Macro to indicate whether a writing-past-end check will be performed.
 * Define it to a positive integer as the number of padding bytes at the
 * end of a memory block for checking.
 */
#ifndef _DEBUG_NEW_TAILCHECK
#define _DEBUG_NEW_TAILCHECK 0
#endif

/**
 * @def _DEBUG_NEW_TAILCHECK_CHAR
 *
 * Value of the padding bytes at the end of a memory block.
 */
#ifndef _DEBUG_NEW_TAILCHECK_CHAR
#define _DEBUG_NEW_TAILCHECK_CHAR 0xCC
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
#pragma warning(disable: 4074)  // #pragma init_seg(compiler) used
#pragma warning(disable: 4290)  // C++ exception specification ignored
#if _MSC_VER >= 1400            // Visual Studio 2005 or later
#pragma warning(disable: 4996)  // Use the `unsafe' strncpy
#endif
#pragma init_seg(compiler)
#endif

#undef  _DEBUG_NEW_EMULATE_MALLOC
#undef  _DEBUG_NEW_REDEFINE_NEW
/**
 * Macro to indicate whether redefinition of \c new is wanted.  Here it
 * is defined to \c 0 to disable the redefinition of \c new.
 */
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "debug_new.h"

/**
 * Gets the aligned value of memory block size.
 */
#define ALIGN(s) \
        (((s) + _DEBUG_NEW_ALIGNMENT - 1) & ~(_DEBUG_NEW_ALIGNMENT - 1))

NVWA_NAMESPACE_BEGIN

/**
 * The platform memory alignment.  The current value works well in
 * platforms I have tested: Windows XP, Windows 7 x64, and Mac OS X
 * Leopard.  It may be smaller than the real alignment, but must be
 * bigger than \c sizeof(size_t) for it work.  nvwa#debug_new_recorder
 * uses it to detect misaligned pointer returned by `<code>new
 * NonPODType[size]</code>'.
 */
const size_t PLATFORM_MEM_ALIGNMENT = sizeof(size_t) * 2;

/**
 * Structure to store the position information where \c new occurs.
 */
struct new_ptr_list_t
{
    new_ptr_list_t* next;       ///< Pointer to the next memory block
    new_ptr_list_t* prev;       ///< Pointer to the previous memory block
    size_t          size;       ///< Size of the memory block
    union
    {
#if _DEBUG_NEW_FILENAME_LEN == 0
    const char*     file;       ///< Pointer to the file name of the caller
#else
    char            file[_DEBUG_NEW_FILENAME_LEN]; ///< File name of the caller
#endif
    void*           addr;       ///< Address of the caller to \e new
    };
    unsigned        line   :31; ///< Line number of the caller; or \c 0
    unsigned        is_array:1; ///< Non-zero iff <em>new[]</em> is used
    unsigned        magic;      ///< Magic number for error detection
};

/**
 * Definition of the constant magic number used for error detection.
 */
static const unsigned DEBUG_NEW_MAGIC = 0x4442474E;

/**
 * The extra memory allocated by <code>operator new</code>.
 */
static const int ALIGNED_LIST_ITEM_SIZE = ALIGN(sizeof(new_ptr_list_t));

/**
 * List of all new'd pointers.
 */
static new_ptr_list_t new_ptr_list = {
    &new_ptr_list,
    &new_ptr_list,
    0,
    {
#if _DEBUG_NEW_FILENAME_LEN == 0
        NULL
#else
        ""
#endif
    },
    0,
    0,
    DEBUG_NEW_MAGIC
};

/**
 * The mutex guard to protect simultaneous access to the pointer list.
 */
static fast_mutex new_ptr_lock;

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
FILE* new_output_fp = stderr;

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
        if (last_info[0] == '\0')
            return false;
        fprintf(new_output_fp, "%s", last_info);
        return true;
    }
    if (new_progname)
    {
#if defined(__APPLE__) && defined(__MACH__)
        const char addr2line_cmd[] = "atos -o ";
#else
        const char addr2line_cmd[] = "addr2line -e ";
#endif
#if   defined(__CYGWIN__) || defined(_WIN32)
        const int  exeext_len = 4;
#else
        const int  exeext_len = 0;
#endif
#if  !defined(__CYGWIN__) && \
        (defined(__unix__) || defined(__unix) || \
         (defined(__APPLE__) && defined(__MACH__)))
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
            // valid position, but the result will be cached if only
            // the command is executed successfully.
            if (res == 0 && len > 0)
            {
                last_addr = addr;
                if (buffer[len - 1] == '0' && buffer[len - 2] == ':')
                    last_info[0] = '\0';
                else
                {
                    fprintf(new_output_fp, "%s", buffer);
                    strcpy(last_info, buffer);
                    return true;
                }
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

#if _DEBUG_NEW_TAILCHECK
/**
 * Checks whether the padding bytes at the end of a memory block is
 * tampered with.
 *
 * @param ptr  pointer to a new_ptr_list_t struct
 * @return     \c true if the padding bytes are untouched; \c false
 *             otherwise
 */
static bool check_tail(new_ptr_list_t* ptr)
{
    const unsigned char* const tail_ptr = (unsigned char*)ptr +
                            ALIGNED_LIST_ITEM_SIZE + ptr->size;
    for (int i = 0; i < _DEBUG_NEW_TAILCHECK; ++i)
        if (tail_ptr[i] != _DEBUG_NEW_TAILCHECK_CHAR)
            return false;
    return true;
}
#endif

/**
 * Allocates memory and initializes control data.
 *
 * @param size      size of the required memory block
 * @param file      null-terminated string of the file name
 * @param line      line number
 * @param is_array  boolean value whether this is an array operation
 * @return          pointer to the user-requested memory area; \c NULL
 *                  if memory allocation is not successful
 */
static void* alloc_mem(size_t size, const char* file, int line, bool is_array)
{
    assert(line >= 0);
#if _DEBUG_NEW_TYPE == 1
    STATIC_ASSERT(_DEBUG_NEW_ALIGNMENT >= PLATFORM_MEM_ALIGNMENT,
                  Alignment_too_small);
#endif
    STATIC_ASSERT((_DEBUG_NEW_ALIGNMENT & (_DEBUG_NEW_ALIGNMENT - 1)) == 0,
                  Alignment_must_be_power_of_two);
    STATIC_ASSERT(_DEBUG_NEW_TAILCHECK >= 0, Invalid_tail_check_length);
    size_t s = size + ALIGNED_LIST_ITEM_SIZE + _DEBUG_NEW_TAILCHECK;
    new_ptr_list_t* ptr = (new_ptr_list_t*)malloc(s);
    if (ptr == NULL)
    {
#if _DEBUG_NEW_STD_OPER_NEW
        return NULL;
#else
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "Out of memory when allocating %lu bytes\n",
                (unsigned long)size);
        fflush(new_output_fp);
        _DEBUG_NEW_ERROR_ACTION;
#endif
    }
    void* usr_ptr = (char*)ptr + ALIGNED_LIST_ITEM_SIZE;
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
    ptr->is_array = is_array;
    ptr->size = size;
    ptr->magic = DEBUG_NEW_MAGIC;
    {
        fast_mutex_autolock lock(new_ptr_lock);
        ptr->prev = new_ptr_list.prev;
        ptr->next = &new_ptr_list;
        new_ptr_list.prev->next = ptr;
        new_ptr_list.prev = ptr;
    }
#if _DEBUG_NEW_TAILCHECK
    memset((char*)usr_ptr + size, _DEBUG_NEW_TAILCHECK_CHAR,
                                  _DEBUG_NEW_TAILCHECK);
#endif
    if (new_verbose_flag)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "new%s: allocated %p (size %lu, ",
                is_array ? "[]" : "",
                usr_ptr, (unsigned long)size);
        if (line != 0)
            print_position(ptr->file, ptr->line);
        else
            print_position(ptr->addr, ptr->line);
        fprintf(new_output_fp, ")\n");
    }
    total_mem_alloc += size;
    return usr_ptr;
}

/**
 * Frees memory and adjusts pointers.
 *
 * @param usr_ptr   pointer to the previously allocated memory
 * @param addr      pointer to the caller
 * @param is_array  flag indicating whether it is invoked by a
 *                  <code>delete[]</code> call
 */
static void free_pointer(void* usr_ptr, void* addr, bool is_array)
{
    if (usr_ptr == NULL)
        return;
    new_ptr_list_t* ptr =
            (new_ptr_list_t*)((char*)usr_ptr - ALIGNED_LIST_ITEM_SIZE);
    if (ptr->magic != DEBUG_NEW_MAGIC)
    {
        {
            fast_mutex_autolock lock(new_output_lock);
            fprintf(new_output_fp, "delete%s: invalid pointer %p (",
                    is_array ? "[]" : "", usr_ptr);
            print_position(addr, 0);
            fprintf(new_output_fp, ")\n");
        }
        check_mem_corruption();
        fflush(new_output_fp);
        _DEBUG_NEW_ERROR_ACTION;
    }
    if (is_array != ptr->is_array)
    {
        const char* msg;
        if (is_array)
            msg = "delete[] after new";
        else
            msg = "delete after new[]";
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "%s: pointer %p (size %lu)\n\tat ",
                msg,
                (char*)ptr + ALIGNED_LIST_ITEM_SIZE,
                (unsigned long)ptr->size);
        print_position(addr, 0);
        fprintf(new_output_fp, "\n\toriginally allocated at ");
        if (ptr->line != 0)
            print_position(ptr->file, ptr->line);
        else
            print_position(ptr->addr, ptr->line);
        fprintf(new_output_fp, "\n");
        fflush(new_output_fp);
        _DEBUG_NEW_ERROR_ACTION;
    }
#if _DEBUG_NEW_TAILCHECK
    if (!check_tail(ptr))
    {
        check_mem_corruption();
        fflush(new_output_fp);
        _DEBUG_NEW_ERROR_ACTION;
    }
#endif
    {
        fast_mutex_autolock lock(new_ptr_lock);
        total_mem_alloc -= ptr->size;
        ptr->magic = 0;
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
    }
    if (new_verbose_flag)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "delete%s: freed %p (size %lu, %lu bytes still allocated)\n",
                is_array ? "[]" : "",
                (char*)ptr + ALIGNED_LIST_ITEM_SIZE,
                (unsigned long)ptr->size, (unsigned long)total_mem_alloc);
    }
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
    fast_mutex_autolock lock_ptr(new_ptr_lock);
    fast_mutex_autolock lock_output(new_output_lock);
    new_ptr_list_t* ptr = new_ptr_list.next;
    while (ptr != &new_ptr_list)
    {
        const char* const usr_ptr = (char*)ptr + ALIGNED_LIST_ITEM_SIZE;
        if (ptr->magic != DEBUG_NEW_MAGIC)
        {
            fprintf(new_output_fp,
                    "warning: heap data corrupt near %p\n",
                    usr_ptr);
        }
#if _DEBUG_NEW_TAILCHECK
        if (!check_tail(ptr))
        {
            fprintf(new_output_fp,
                    "warning: overwritten past end of object at %p\n",
                    usr_ptr);
        }
#endif
        fprintf(new_output_fp,
                "Leaked object at %p (size %lu, ",
                usr_ptr,
                (unsigned long)ptr->size);
        if (ptr->line != 0)
            print_position(ptr->file, ptr->line);
        else
            print_position(ptr->addr, ptr->line);
        fprintf(new_output_fp, ")\n");
        ptr = ptr->next;
        ++leak_cnt;
    }
    if (new_verbose_flag || leak_cnt)
        fprintf(new_output_fp, "*** %d leaks found\n", leak_cnt);
    return leak_cnt;
}

/**
 * Checks for heap corruption.
 *
 * @return  zero if no problem is found; the number of found memory
 *          corruptions otherwise
 */
int check_mem_corruption()
{
    int corrupt_cnt = 0;
    fast_mutex_autolock lock_ptr(new_ptr_lock);
    fast_mutex_autolock lock_output(new_output_lock);
    fprintf(new_output_fp, "*** Checking for memory corruption: START\n");
    for (new_ptr_list_t* ptr = new_ptr_list.next;
            ptr != &new_ptr_list;
            ptr = ptr->next)
    {
        const char* const usr_ptr = (char*)ptr + ALIGNED_LIST_ITEM_SIZE;
        if (ptr->magic == DEBUG_NEW_MAGIC
#if _DEBUG_NEW_TAILCHECK
                && check_tail(ptr)
#endif
                )
            continue;
#if _DEBUG_NEW_TAILCHECK
        if (ptr->magic != DEBUG_NEW_MAGIC)
        {
#endif
            fprintf(new_output_fp,
                    "Heap data corrupt near %p (size %lu, ",
                    usr_ptr,
                    (unsigned long)ptr->size);
#if _DEBUG_NEW_TAILCHECK
        }
        else
        {
            fprintf(new_output_fp,
                    "Overwritten past end of object at %p (size %lu, ",
                    usr_ptr,
                    (unsigned long)ptr->size);
        }
#endif
        if (ptr->line != 0)
            print_position(ptr->file, ptr->line);
        else
            print_position(ptr->addr, ptr->line);
        fprintf(new_output_fp, ")\n");
        ++corrupt_cnt;
    }
    fprintf(new_output_fp, "*** Checking for memory corruption: %d FOUND\n",
            corrupt_cnt);
    return corrupt_cnt;
}

/**
 * Processes the allocated memory and inserts file/line informatin.
 * It will only be done when it can ensure the memory is allocated by
 * one of our operator new variants.
 *
 * @param usr_ptr  pointer returned by a new-expression
 */
void debug_new_recorder::_M_process(void* usr_ptr)
{
    if (usr_ptr == NULL)
        return;

    // In an expression `new NonPODType[size]', the pointer returned is
    // not the pointer returned by operator new[], but offset by size_t
    // to leave room for the size.  It needs to be compensated here.
    size_t offset = (char*)usr_ptr - (char*)NULL;
    if (offset % PLATFORM_MEM_ALIGNMENT != 0) {
        offset -= sizeof(size_t);
        if (offset % PLATFORM_MEM_ALIGNMENT != 0) {
            fast_mutex_autolock lock(new_output_lock);
            fprintf(new_output_fp,
                    "warning: memory unaligned; skipping processing (%s:%d)\n",
                    _M_file, _M_line);
            return;
        }
        usr_ptr = (char*)usr_ptr - sizeof(size_t);
    }

    new_ptr_list_t* ptr =
            (new_ptr_list_t*)((char*)usr_ptr - ALIGNED_LIST_ITEM_SIZE);
    if (ptr->magic != DEBUG_NEW_MAGIC || ptr->line != 0)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "warning: debug_new used with placement new (%s:%d)\n",
                _M_file, _M_line);
        return;
    }
    if (new_verbose_flag) {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "info: pointer %p allocated from %s:%d\n",
                usr_ptr, _M_file, _M_line);
    }
#if _DEBUG_NEW_FILENAME_LEN == 0
    ptr->file = _M_file;
#else
    strncpy(ptr->file, _M_file, _DEBUG_NEW_FILENAME_LEN - 1)
            [_DEBUG_NEW_FILENAME_LEN - 1] = '\0';
#endif
    ptr->line = _M_line;
}

/**
 * Count of source files that use debug_new.
 */
int debug_new_counter::_S_count = 0;

/**
 * Constructor to increment the count.
 */
debug_new_counter::debug_new_counter()
{
    ++_S_count;
}

/**
 * Destructor to decrement the count.  When the count is zero,
 * #check_leaks will be called.
 */
debug_new_counter::~debug_new_counter()
{
    if (--_S_count == 0 && new_autocheck_flag)
        if (check_leaks())
        {
            new_verbose_flag = true;
#if defined(__GNUC__) && __GNUC__ == 3
            if (!getenv("GLIBCPP_FORCE_NEW") && !getenv("GLIBCXX_FORCE_NEW"))
                fprintf(new_output_fp,
"*** WARNING:  GCC 3 is detected, please make sure the environment\n"
"    variable GLIBCPP_FORCE_NEW (GCC 3.2 and 3.3) or GLIBCXX_FORCE_NEW\n"
"    (GCC 3.4) is defined.  Check the README file for details.\n");
#endif
        }
}

NVWA_NAMESPACE_END

#if NVWA_USE_NAMESPACE
using namespace nvwa;
#endif // NVWA_USE_NAMESPACE

/**
 * Allocates memory with file/line information.
 *
 * @param size  size of the required memory block
 * @param file  null-terminated string of the file name
 * @param line  line number
 * @return      pointer to the memory allocated; or \c NULL if memory is
 *              insufficient (#_DEBUG_NEW_STD_OPER_NEW is 0)
 * @throw bad_alloc memory is insufficient (#_DEBUG_NEW_STD_OPER_NEW is 1)
 */
void* operator new(size_t size, const char* file, int line)
{
    void* ptr = alloc_mem(size, file, line, false);
#if _DEBUG_NEW_STD_OPER_NEW
    if (ptr)
        return ptr;
    else
        throw std::bad_alloc();
#else
    return ptr;
#endif
}

/**
 * Allocates array memory with file/line information.
 *
 * @param size  size of the required memory block
 * @param file  null-terminated string of the file name
 * @param line  line number
 * @return      pointer to the memory allocated; or \c NULL if memory is
 *              insufficient (#_DEBUG_NEW_STD_OPER_NEW is 0)
 * @throw bad_alloc memory is insufficient (#_DEBUG_NEW_STD_OPER_NEW is 1)
 */
void* operator new[](size_t size, const char* file, int line)
{
    void* ptr = alloc_mem(size, file, line, true);
#if _DEBUG_NEW_STD_OPER_NEW
    if (ptr)
        return ptr;
    else
        throw std::bad_alloc();
#else
    return ptr;
#endif
}

/**
 * Allocates memory without file/line information.
 *
 * @param size  size of the required memory block
 * @return      pointer to the memory allocated; or \c NULL if memory is
 *              insufficient (#_DEBUG_NEW_STD_OPER_NEW is 0)
 * @throw bad_alloc memory is insufficient (#_DEBUG_NEW_STD_OPER_NEW is 1)
 */
void* operator new(size_t size) _GLIBCXX_THROW (std::bad_alloc) // modified to let it compile with c++11, MB 2015-04-17
{
    return operator new(size, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0);
}

/**
 * Allocates array memory without file/line information.
 *
 * @param size  size of the required memory block
 * @return      pointer to the memory allocated; or \c NULL if memory is
 *              insufficient (#_DEBUG_NEW_STD_OPER_NEW is 0)
 * @throw bad_alloc memory is insufficient (#_DEBUG_NEW_STD_OPER_NEW is 1)
 */
void* operator new[](size_t size) _GLIBCXX_THROW (std::bad_alloc) // modified to let it compile with c++11, MB 2015-04-17
{
    return operator new[](size, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0);
}

/**
 * Allocates memory with no-throw guarantee.
 *
 * @param size  size of the required memory block
 * @return      pointer to the memory allocated; or \c NULL if memory is
 *              insufficient
 */
void* operator new(size_t size, const std::nothrow_t&) _NOEXCEPT
{
    return alloc_mem(size, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0, false);
}

/**
 * Allocates array memory with no-throw guarantee.
 *
 * @param size  size of the required memory block
 * @return      pointer to the memory allocated; or \c NULL if memory is
 *              insufficient
 */
void* operator new[](size_t size, const std::nothrow_t&) _NOEXCEPT
{
    return alloc_mem(size, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0, true);
}

/**
 * Deallocates memory.
 *
 * @param ptr  pointer to the previously allocated memory
 */
void operator delete(void* ptr) _NOEXCEPT
{
    free_pointer(ptr, _DEBUG_NEW_CALLER_ADDRESS, false);
}

/**
 * Deallocates array memory.
 *
 * @param ptr  pointer to the previously allocated memory
 */
void operator delete[](void* ptr) _NOEXCEPT
{
    free_pointer(ptr, _DEBUG_NEW_CALLER_ADDRESS, true);
}

/**
 * Placement deallocation function.  For details, please check Section
 * 5.3.4 of the C++ 1998 or 2011 Standard.
 *
 * @param ptr   pointer to the previously allocated memory
 * @param file  null-terminated string of the file name
 * @param line  line number
 *
 * @see   http://www.csci.csusb.edu/dick/c++std/cd2/expr.html#expr.new
 * @see   http://wyw.dcweb.cn/leakage.htm
 */
void operator delete(void* ptr, const char* file, int line) _NOEXCEPT
{
    if (new_verbose_flag)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "info: exception thrown on initializing object at %p (",
                ptr);
        print_position(file, line);
        fprintf(new_output_fp, ")\n");
    }
    operator delete(ptr);
}

/**
 * Placement deallocation function.  For details, please check Section
 * 5.3.4 of the C++ 1998 or 2011 Standard.
 *
 * @param ptr   pointer to the previously allocated memory
 * @param file  null-terminated string of the file name
 * @param line  line number
 */
void operator delete[](void* ptr, const char* file, int line) _NOEXCEPT
{
    if (new_verbose_flag)
    {
        fast_mutex_autolock lock(new_output_lock);
        fprintf(new_output_fp,
                "info: exception thrown on initializing objects at %p (",
                ptr);
        print_position(file, line);
        fprintf(new_output_fp, ")\n");
    }
    operator delete[](ptr);
}

/**
 * Placement deallocation function.  For details, please check Section
 * 5.3.4 of the C++ 1998 or 2011 Standard.
 *
 * @param ptr  pointer to the previously allocated memory
 */
void operator delete(void* ptr, const std::nothrow_t&) _NOEXCEPT
{
    operator delete(ptr, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0);
}

/**
 * Placement deallocation function.  For details, please check Section
 * 5.3.4 of the C++ 1998 or 2011 Standard.
 *
 * @param ptr  pointer to the previously allocated memory
 */
void operator delete[](void* ptr, const std::nothrow_t&) _NOEXCEPT
{
    operator delete[](ptr, (char*)_DEBUG_NEW_CALLER_ADDRESS, 0);
}

#endif
