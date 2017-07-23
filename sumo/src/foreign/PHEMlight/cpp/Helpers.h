/****************************************************************************/
/// @file    Helpers.h
/// @author  Martin Dippold
/// @author  Michael Behrisch
/// @date    July 2016
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2016-2017 DLR (http://www.dlr.de/) and contributors
// PHEMlight module
// Copyright 2016 Technische Universitaet Graz, https://www.tugraz.at/
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


#ifndef PHEMlightHELPERS
#define PHEMlightHELPERS

#include <string>


namespace PHEMlightdll {
    class Helpers {
        //--------------------------------------------------------------------------------------------------
        // Members 
        //--------------------------------------------------------------------------------------------------
    private:
        std::string _vClass;
    public:
        const std::string&  getvClass() const;
        void setvClass(const std::string& value);
    private:
        std::string _eClass;
    public:
        const std::string&  geteClass() const;
        void seteClass(const std::string& value);
    private:
        std::string _tClass;
    public:
        const std::string&  gettClass() const;
        void settClass(const std::string& value);
    private:
        std::string _sClass;
    public:
        const std::string&  getsClass() const;
        void setsClass(const std::string& value);
    private:
        std::string _Class;
    public:
        const std::string&  getgClass() const;
        void setgClass(const std::string& value);
    private:
        std::string _ErrMsg;
    public:
        const std::string&  getErrMsg() const;
        void setErrMsg(const std::string& value);
    private:
        std::string _commentPrefix;
    public:
        const std::string&  getCommentPrefix() const;
        void setCommentPrefix(const std::string& value);
    private:
        std::string _PHEMDataV;
    public:
        const std::string&  getPHEMDataV() const;
        void setPHEMDataV(const std::string& value);

        //Get vehicle class
    private:
        bool getvclass(const std::string& VEH);

        //Get technologie Class
        bool gettclass(const std::string& VEH);

        //Get size class
        bool getsclass(const std::string& VEH);

        //Get euro class
        bool geteclass(const std::string& VEH);

        //Set complete class string
    public:
        bool setclass(const std::string& VEH);


    };
}


#endif	//#ifndef PHEMlightHELPERS
