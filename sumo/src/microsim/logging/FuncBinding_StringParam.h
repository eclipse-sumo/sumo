
#ifndef FuncBinding_StringParam_H
#define FuncBinding_StringParam_H
//---------------------------------------------------------------------------//
//                        FuncBinding_StringParam.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.1  2006/11/14 06:42:21  dkrajzew
// tls tracker now support switches between logics
//
//
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

#include <utils/helpers/ValueSource.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template< class _T, typename _R  >
class FuncBinding_StringParam : public ValueSource<_R>
{
public:
    /// Type of the function to execute.
    typedef _R ( _T::* Operation )(const std::string &) const;

    FuncBinding_StringParam( _T* source, Operation operation,
                const std::string &param )
                :
        mySource( source ),
        myOperation( operation ),
        myParam(param)
        {}

    /// Destructor.
    ~FuncBinding_StringParam()
        {}

    _R getValue() const
        {
            return ( mySource->*myOperation )(myParam);
        }

    ValueSource<_R> *copy() const {
        return new FuncBinding_StringParam<_T, _R>(
            mySource, myOperation, myParam);
    }


protected:

private:
    /// The object the action is directed to.
    _T* mySource;

    /// The object's operation to perform.
    Operation myOperation;

    std::string myParam;

};

#endif // FuncBinding_StringParam_H

// Local Variables:
// mode:C++
// End:
