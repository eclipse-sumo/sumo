/****************************************************************************/
/// @file    SysUtils.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// A few system-specific functions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <stdlib.h>
#include "SysUtils.h"

#ifndef WIN32
#include <sys/time.h>
#else
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
long
SysUtils::getCurrentMillis() {
#ifndef WIN32
    timeval current;
    gettimeofday(&current, 0);
    long nanosecs =
        (long) current.tv_sec * 1000L + (long) current.tv_usec / 1000L;
    return nanosecs;
#else
    LARGE_INTEGER val, val2;
    BOOL check = QueryPerformanceCounter(&val);
    check = QueryPerformanceFrequency(&val2);
    return (long)(val.QuadPart * 1000 / val2.QuadPart);
#endif
}


#ifdef _MSC_VER
long
SysUtils::getWindowsTicks() {
    return (long) GetTickCount();
}
#endif


unsigned long
SysUtils::runHiddenCommand(const std::string& cmd) {
#ifdef _MSC_VER
    // code inspired by http://www.codeproject.com/Articles/2537/Running-console-applications-silently
    STARTUPINFO StartupInfo;
    PROCESS_INFORMATION ProcessInfo;
    unsigned long rc;

    memset(&StartupInfo, 0, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(STARTUPINFO);
    StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
    StartupInfo.wShowWindow = SW_HIDE;

    // "/c" option - Do the command then terminate the command window
    std::string winCmd = "CMD.exe /c " + cmd;
    char* args = new char[winCmd.size()];
    args[0] = 0;
    strcpy(args, winCmd.c_str());
    if (!CreateProcess(NULL, args, NULL, NULL, FALSE,
                       CREATE_NEW_CONSOLE, NULL, NULL, &StartupInfo, &ProcessInfo)) {
        delete args;
        return (unsigned long)GetLastError();
    }

    WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
    if (!GetExitCodeProcess(ProcessInfo.hProcess, &rc)) {
        rc = 0;
    }

    CloseHandle(ProcessInfo.hThread);
    CloseHandle(ProcessInfo.hProcess);

    delete args;
    return rc;
#else
    return (unsigned long)system(cmd.c_str());
#endif
}

/****************************************************************************/

