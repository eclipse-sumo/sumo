#ifndef AbstractMutex_h
#define AbstractMutex_h


/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H


class AbstractMutex {
public:
    AbstractMutex() { }
    virtual ~AbstractMutex() { }
    virtual void lock() = 0;
    virtual void unlock() = 0;

};


#endif

