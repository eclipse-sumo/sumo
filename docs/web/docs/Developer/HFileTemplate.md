---
title: Developer/HFileTemplate
permalink: /Developer/HFileTemplate/
---

```
/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see <https://eclipse.org/sumo>
// Copyright (C) <YEAR OF CREATION>-<CURRENT YEAR> German Aerospace Center (DLR) and others.
// <You can add an additional copyright line for your institution here>
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// <http://www.eclipse.org/legal/epl-v20.html>
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    <FILENAME>
/// @author  <AUTHOR'S NAME, ONE SEPARATE LINE FOR EACH AUTHOR>
/// @author  <AUTHOR'S NAME 2>
/// @author  <AUTHOR'S NAME 3>
/// @date    <FILE CREATION DATE>
/// @version $Id: $
///
// <A BRIEF DESCRIPTION OF THE FILE'S CONTENTS>
/****************************************************************************/
#ifndef <FILENAME_h>
#define <FILENAME_h>

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h> // you can leave this one out if you don't need any defines from config.h, especially in libsumo / utils

#include "myOwnClass1.h"
#include "myOwnClass2.h"
```

```
// ===========================================================================
// class declarations
// ===========================================================================
class myOwnClass1; // always prefer a declaration over an include
class myOwnClass2;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class OwnClass (Can be preceded by GNE, MS, NB, etc... depending on the project)
 * brief description of the class
 */
class OwnClass : public ParentClass  {

public:
     /**
      * @class SubClass 
      * brief description of the sub-class
      */
     class SubClass {

     public:
         /// @brief Constructor
         SubClass();

         /// @brief Destructor
         ~SubClass();

     private:
         /// @brief description of parameter
         <parameterType> myPrivateParameter;
     };

    /** @brief Constructor
     * @param[in] parameter1 each parameter must be documented
     * @param[in] parameter2 each parameter must be documented
       ...
     */
    OwnClass(<parameterType> parameter1, <parameterType> parameter2,... <parameterType> parametern);

    /// @brief Destructor (don't need documentation)
    ~OwnClass();

    /// @brief Returns parameter1 (Simple functions should be documented with one line)
    <parameterType> getParameter1() const;

    /** @brief some functions can throw exceptions but don't use throw declarations
     * @param[in] parameter1 description of the parameter
     * @throw <exceptionName> description of the situation that provoque an exception
     * @return description of the returned value
     */
    bool setParameter1(<type> parameter1) const;

    /// @brief if the class is abstract, should have pure virtual functions (virtual .... = 0;)
    //  @note: Extra information about a function should be written using @note
    virtual void pureVirtualFunction() = 0;

protected:
    /// @brief briefly description of the parameter
    <parameterType> myProtectedParameter1;

private:
    /// @brief every private parameter must start with "my"
    <parameterType> myPrivateParameter2;

    /// @brief C++ Standard Library classes must be preceded by std::
    std::vector<std::string> myPrivateParameter3;
};

#endif
```