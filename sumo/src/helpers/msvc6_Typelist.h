////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
//
// Last update: Feb 22, 2003
//
// renamed MakeTypeList to MakeTypelist.
//
// Rani Sharoni's VC 7 port is heavily based on explicit template specialization
// inside class templates.
// This approach has two problems:
// First: It does not seem to work correctly with VC 6
// Second: The C++ Standard allows explicit specialization only in namespace-scope.
//
// In general my solutions need more template-classes. But I hope that they
// are all conforming to the C++ Standard.


#ifndef TYPELIST_INC_
#define TYPELIST_INC_

#include "msvc6_Nulltype.h"
#include "msvc6_TypeManip.h"
#include "msvc6_static_check.h"

#define ASSERT_TYPELIST(TList)	\
typedef ::Loki::Private::static_assert_test<sizeof(::Loki::TL::Private::TList_is_not_legal_Typelist	\
< (bool) ( ::Loki::TL::Private::IsTypelist<TList>::value ) >)> static_assert_typedef_

////////////////////////////////////////////////////////////////////////////////
// macros TYPELIST_1, TYPELIST_2, ... TYPELIST_50
// Each takes a number of arguments equal to its numeric suffix
// The arguments are type names. TYPELIST_NN generates a typelist containing
//     all types passed as arguments, in that order.
// Example: TYPELIST_2(char, int) generates a type containing char and int.
////////////////////////////////////////////////////////////////////////////////

#define TYPELIST_1(T1) ::Loki::Typelist<T1, ::Loki::NullType>

#define TYPELIST_2(T1, T2) ::Loki::Typelist<T1, TYPELIST_1(T2) >

#define TYPELIST_3(T1, T2, T3) ::Loki::Typelist<T1, TYPELIST_2(T2, T3) >

#define TYPELIST_4(T1, T2, T3, T4) \
    ::Loki::Typelist<T1, TYPELIST_3(T2, T3, T4) >

#define TYPELIST_5(T1, T2, T3, T4, T5) \
    ::Loki::Typelist<T1, TYPELIST_4(T2, T3, T4, T5) >

#define TYPELIST_6(T1, T2, T3, T4, T5, T6) \
    ::Loki::Typelist<T1, TYPELIST_5(T2, T3, T4, T5, T6) >

#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) \
    ::Loki::Typelist<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >

#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) \
    ::Loki::Typelist<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >

#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) \
    ::Loki::Typelist<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >

#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) \
    ::Loki::Typelist<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >

#define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) \
    ::Loki::Typelist<T1, TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >

#define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) \
    ::Loki::Typelist<T1, TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12) >

#define TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) \
    ::Loki::Typelist<T1, TYPELIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13) >

#define TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14) \
    ::Loki::Typelist<T1, TYPELIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14) >

#define TYPELIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15) \
    ::Loki::Typelist<T1, TYPELIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15) >

#define TYPELIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16) \
    ::Loki::Typelist<T1, TYPELIST_15(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16) >

#define TYPELIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17) \
    ::Loki::Typelist<T1, TYPELIST_16(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17) >

#define TYPELIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18) \
    ::Loki::Typelist<T1, TYPELIST_17(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18) >

#define TYPELIST_19(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19) \
    ::Loki::Typelist<T1, TYPELIST_18(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19) >

#define TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) \
    ::Loki::Typelist<T1, TYPELIST_19(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) >

#define TYPELIST_21(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21) \
    ::Loki::Typelist<T1, TYPELIST_20(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21) >

#define TYPELIST_22(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22) \
    ::Loki::Typelist<T1, TYPELIST_21(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22) >

#define TYPELIST_23(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23) \
    ::Loki::Typelist<T1, TYPELIST_22(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23) >

#define TYPELIST_24(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24) \
    ::Loki::Typelist<T1, TYPELIST_23(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24) >

#define TYPELIST_25(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25) \
    ::Loki::Typelist<T1, TYPELIST_24(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25) >

#define TYPELIST_26(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26) \
    ::Loki::Typelist<T1, TYPELIST_25(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26) >

#define TYPELIST_27(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27) \
    ::Loki::Typelist<T1, TYPELIST_26(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27) >

#define TYPELIST_28(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28) \
    ::Loki::Typelist<T1, TYPELIST_27(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28) >

#define TYPELIST_29(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29) \
    ::Loki::Typelist<T1, TYPELIST_28(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29) >

#define TYPELIST_30(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30) \
    ::Loki::Typelist<T1, TYPELIST_29(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30) >

#define TYPELIST_31(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31) \
    ::Loki::Typelist<T1, TYPELIST_30(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31) >

#define TYPELIST_32(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32) \
    ::Loki::Typelist<T1, TYPELIST_31(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32) >

#define TYPELIST_33(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33) \
    ::Loki::Typelist<T1, TYPELIST_32(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33) >

#define TYPELIST_34(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33, T34) \
    ::Loki::Typelist<T1, TYPELIST_33(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33, T34) >

#define TYPELIST_35(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35) \
    ::Loki::Typelist<T1, TYPELIST_34(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35) >

#define TYPELIST_36(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36) \
    ::Loki::Typelist<T1, TYPELIST_35(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36) >

#define TYPELIST_37(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37) \
    ::Loki::Typelist<T1, TYPELIST_36(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37) >

#define TYPELIST_38(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38) \
    ::Loki::Typelist<T1, TYPELIST_37(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38) >

#define TYPELIST_39(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39) \
    ::Loki::Typelist<T1, TYPELIST_38(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39) >

#define TYPELIST_40(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40) \
    ::Loki::Typelist<T1, TYPELIST_39(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40) >

#define TYPELIST_41(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41) \
    ::Loki::Typelist<T1, TYPELIST_40(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41) >

#define TYPELIST_42(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42) \
    ::Loki::Typelist<T1, TYPELIST_41(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42) >

#define TYPELIST_43(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43) \
    ::Loki::Typelist<T1, TYPELIST_42(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43) >

#define TYPELIST_44(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43, T44) \
    ::Loki::Typelist<T1, TYPELIST_43(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43, T44) >

#define TYPELIST_45(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45) \
    ::Loki::Typelist<T1, TYPELIST_44(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45) >

#define TYPELIST_46(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46) \
    ::Loki::Typelist<T1, TYPELIST_45(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46) >

#define TYPELIST_47(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47) \
    ::Loki::Typelist<T1, TYPELIST_46(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47) >

#define TYPELIST_48(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48) \
    ::Loki::Typelist<T1, TYPELIST_47(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48) >

#define TYPELIST_49(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49) \
    ::Loki::Typelist<T1, TYPELIST_48(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49) >

#define TYPELIST_50(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49, T50) \
    ::Loki::Typelist<T1, TYPELIST_49(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49, T50) >

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template Typelist
// The building block of typelists of any length
// Use it through the TYPELIST_NN macros
// Defines nested types:
//     Head (first element, a non-typelist type by convention)
//     Tail (second element, can be another typelist)
////////////////////////////////////////////////////////////////////////////////
	template <class T, class U>
	struct Typelist
	{
		typedef T Head;
		typedef U Tail;
	};

	namespace TL
	{

		namespace Private
		{
			template <bool x> struct TList_is_not_legal_Typelist;

			template <> struct TList_is_not_legal_Typelist<true>{};

			struct Typelist_tag {};
			struct NullType_tag {};
			struct NoneList_tag {};

			enum
			{
				NoneList_ID = 0,
				Typelist_ID = 1,
				AtomList_ID	= 2,
				NullType_ID = 4

			};
////////////////////////////////////////////////////////////////////////////////
// class template IsTypelist
// detects if type is Typelist (including Nulltype)
// Invocation :
// IsTypelist<T>::value
////////////////////////////////////////////////////////////////////////////////
			template<typename T>
			struct IsTypelist
			{
				private:
					typedef TypeTag<1>::X List;
					typedef TypeTag<2>::X AtomList;
					typedef TypeTag<3>::X NullList;
					typedef TypeTag<4>::X NoList;

					// VC 6.0 does not allow overloads
					// for check(Type2Type< Typelist<Head, Tail> >)
					// and check(Type2Type<NullType>);
					// so we must use to different functions
					template<class Head, class Tail>
					static TypeTag<1>::X	check(Type2Type< Typelist<Head, Tail> >);
					static TypeTag<4>::X	check(...);

					template <class U>
					static TypeTag<2>::X	check2(Type2Type< Typelist<U, NullType> >);
					static TypeTag<4>::X	check2(...);

					static TypeTag<3>::X	check3(Type2Type<NullType>);
					static TypeTag<4>::X	check3(...);


				public:
					enum
					{
						temp1	= sizeof(check(Type2Type<T>())) == sizeof(TypeTag<1>::X) ? Typelist_ID : NoneList_ID,
						temp2	= sizeof(check2(Type2Type<T>())) == sizeof(TypeTag<2>::X) ? AtomList_ID : NoneList_ID,
						temp4	= temp2 ? Typelist_ID :NoneList_ID,
						temp3	= sizeof(check3(Type2Type<T>())) == sizeof(TypeTag<3>::X) ? NullType_ID : NoneList_ID,
						value	= temp1 || temp2 || temp3,
						type_id	= (temp1 ^ temp4) | temp2 | temp3
					};
					typedef typename Select
					<
						type_id == Typelist_ID || type_id == AtomList_ID,
						Typelist_tag,
						typename Select<type_id == NullType_ID, NullType_tag, NoneList_tag>::Result
					>
					::Result type_tag;



			};

		}	// end of namespace Private
////////////////////////////////////////////////////////////////////////////////
// class template MakeTypelist
// Takes a number of arguments equal to its numeric suffix
// The arguments are type names.
// MakeTypeList<T1, T2, ...>::Result
// returns a typelist that is of T1, T2, ...
////////////////////////////////////////////////////////////////////////////////
// MakeTypeList-Template from Rani Sharoni's VC 7 port.
		template
		<	typename T1  = NullType, typename T2  = NullType, typename T3  = NullType,
			typename T4  = NullType, typename T5  = NullType, typename T6  = NullType,
			typename T7  = NullType, typename T8  = NullType, typename T9  = NullType,
			typename T10 = NullType, typename T11 = NullType, typename T12 = NullType,
			typename T13 = NullType, typename T14 = NullType, typename T15 = NullType,
			typename T16 = NullType, typename T17 = NullType, typename T18 = NullType
		>
		struct MakeTypelist
		{
			private:
				typedef typename MakeTypelist
				<
				T2 , T3 , T4 ,
				T5 , T6 , T7 ,
				T8 , T9 , T10,
				T11, T12, T13,
				T14, T15, T16,
				T17, T18
				>
				::Result TailResult;

			public:
				typedef Typelist<T1, TailResult> Result;
		};

		template<>
		struct MakeTypelist
		<
			NullType, NullType, NullType,
			NullType, NullType, NullType,
			NullType, NullType, NullType,
			NullType, NullType, NullType,
			NullType, NullType, NullType,
			NullType, NullType, NullType
		>
		{
			typedef NullType Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template Length
// Computes the length of a typelist
// Invocation (TList is a typelist):
// Length<TList>::value
// returns a compile-time constant containing the length of TList, not counting
// the end terminator (which by convention is NullType)
////////////////////////////////////////////////////////////////////////////////
		template <class TList>
		struct Length
		{
			private:
				ASSERT_TYPELIST(TList);
				typedef typename TList::Head Head;
				typedef typename TList::Tail Tail;
			public:

			enum {value = 1 + Length<Tail>::value};
		};

		// explicit specialization for an empty list.
		// this is the border case for the recursive length-calculation
		template <>
		struct Length<NullType>
		{
			enum {value = 0};
		};


////////////////////////////////////////////////////////////////////////////////
// class template TypeAt
// Finds the type at a given index in a typelist
// Invocation (TList is a typelist and index is a compile-time integral
//     constant):
// TypeAt<TList, index>::Result
// returns the type in position 'index' in TList
// If you pass an out-of-bounds index, the result is a compile-time error
////////////////////////////////////////////////////////////////////////////////
namespace Private
{
	// The type at Index i is the type at i-1 of the List's Tail
	template <unsigned int Index>
	struct TypeAtImpl
	{
		template <class TList>
		struct In
		{
			ASSERT_TYPELIST(TList);
			typedef typename TList::Head Head;
			typedef typename TList::Tail Tail;
			typedef typename TypeAtImpl<Index-1>::template In<Tail>::Result Result;
		};

	};
	// the border case is represented by an explicit specialization
	// The type at Index 0 is the type of the head.
	template <>
	struct TypeAtImpl<0>
	{
		template <class TList>
		struct In
		{
			ASSERT_TYPELIST(TList);
			typedef typename TList::Head Head;
			typedef Head Result;
		};
	};
}	// end of namespace Private

	template <class TList, unsigned int Index>
	struct TypeAt
	{
		typedef typename Private::TypeAtImpl<Index>::template In<TList>::Result Result ;
	};

////////////////////////////////////////////////////////////////////////////////
// class template TypeAtNonStrict
// Finds the type at a given index in a typelist
// Invocations (TList is a typelist and index is a compile-time integral
//     constant):
// a) TypeAt<TList, index>::Result
// returns the type in position 'index' in TList, or NullType if index is
//     out-of-bounds
// b) TypeAt<TList, index, D>::Result
// returns the type in position 'index' in TList, or D if index is out-of-bounds
////////////////////////////////////////////////////////////////////////////////
	template <class TList, unsigned int i, class DefType = NullType>
	struct TypeAtNonStrict;
namespace Private
{
	// if TList is not NullType, check if Index is 0.
	// if Index is 0, the result is TList::Head
	// if Index is > 0, the result is the result of appliying TypeAtNonStrict
	// to the list's and Index-1
	template <class TList>
	struct TypeAtNonStrictImpl
	{
		template <class DefType, unsigned int Index>
		struct In
		{
			ASSERT_TYPELIST(TList);
			typedef typename Select
			<
				Index == 0,				// The condition
				typename TList::Head,	// true-case
				typename TypeAtNonStrict<typename TList::Tail, Index-1, DefType>::Result
			>::Result Result;
		};
	};

	// if TList is NullType the result is *always* the specified DefaultType.
	template <>
	struct TypeAtNonStrictImpl<NullType>
	{
		template <class DefType, unsigned int Index>
		struct In
		{
			typedef DefType Result;
		};
	};

}	// end of namespace Private
	template <class TList, unsigned int i, class DefType>
	struct TypeAtNonStrict
	{
		typedef typename
		Private::TypeAtNonStrictImpl<TList>::template In<DefType, i>::Result Result;
	};

////////////////////////////////////////////////////////////////////////////////
// class template IndexOf
// Finds the index of a type in a typelist
// Invocation (TList is a typelist and T is a type):
// IndexOf<TList, T>::value
// returns the position of T in TList, or -1 if T is not found in TList
////////////////////////////////////////////////////////////////////////////////
	template <class TList, class T>
	struct IndexOf;
namespace Private
{
	// If TList is a typelist and TList::Head is T, then the Index is 0
	// If TList::Head is not T, compute the result of IndexOf applied to
	// TList's tail and T into a temporary value temp.
	// If temp is -1, then value is -1
	// Else value is 1 + temp
	template <class TList>
	struct IndexOfImpl
	{
		template <class T>
		struct In
		{
			ASSERT_TYPELIST(TList);
			typedef typename TList::Head Head;
			typedef typename TList::Tail Tail;
			private:
				enum {temp = IsEqualType<T, Head>::value != 0 ? 0
							: IndexOf<Tail, T>::temp};

			public:
				enum {value = temp == -1 ? -1 : 1 + temp};
		};
	};

	// T cannot be in an empty list.
	// Therefore return -1 to indicate Not-In-List
	template <>
	struct IndexOfImpl<NullType>
	{
		template <class T>
		struct In
		{
			enum {value = -1};
		};
	};

}	// end of namespace Private

	// The primary IndexOfImpl-Template is always one step ahead.
	// Therefore if T is in list, we need to subtract one from the result.
	template <class TList, class T>
	struct IndexOf
	{
		enum {temp = Private::IndexOfImpl<TList>::template In<T>::value};
		enum {value = temp == -1 ? -1 : temp - 1};
	};
////////////////////////////////////////////////////////////////////////////////
// class template Append
// Appends a type or a typelist to another
// Invocation (TList is a typelist and T is either a type or a typelist):
// Append<TList, T>::Result
// returns a typelist that is TList followed by T and NullType-terminated
////////////////////////////////////////////////////////////////////////////////
	template <class TList, class T>
	struct Append;

namespace Private
{
	template <class TList>
	struct AppendImpl
	{	// if TList is not NullType the result
		// is a typelist having TList::Head as its Head and
		// and the result of appending T to TList::Tail as its tail.
		ASSERT_TYPELIST(TList);
		template <class T>
		struct In
		{
			typedef Typelist<typename TList::Head,
				typename Append<typename TList::Tail, T>::Result> Result;
		};

	};

	template <>
	struct AppendImpl<NullType>
	{	// if TList is NullType, check if T is NullType, a single type
		// or a typelist
		// If TList is NullType and T is NullType
		// the result is NullType, too
		//
		// If TList is NullType and T is not NullType.
		// Check if T is a Typelist
		//
		// if TList is NullType and T is a typelist the result is T
		// if TList is NullType and T is not a typelist
		// the result is a typelist containing only T
		template <class T>
		struct In
		{
			typedef typename Select
			<
				IsEqualType<T, NullType>::value,	// is T == Nulltype?
				NullType,							// yes
				typename Select						// no. check if T is a Typelist
				<
					IsTypelist<T>::value,			// is T a typelist?
					T,								// yes
					Typelist<T, NullType>			// no
				>::Result
			>::Result Result;
		};
	};

}	// end of namespace Private

	template <class TList, class T>
	struct Append
	{
		typedef typename Private::AppendImpl<TList>::template In<T>::Result Result;
	};

////////////////////////////////////////////////////////////////////////////////
// class template Erase
// Erases the first occurence, if any, of a type in a typelist
// Invocation (TList is a typelist and T is a type):
// Erase<TList, T>::Result
// returns a typelist that is TList without the first occurence of T
////////////////////////////////////////////////////////////////////////////////
	template <class TList, class T>
	struct Erase;
namespace Private
{
	template <class TList>
	struct EraseImpl
	{	// TList is not NullType.
		// Check if TList::Head is equal to T
		// if T is the same as TList::Head, then the Result is TList::Tail
		//
		// if TList is not NullType and TList::Head is not equal to T,
		// then the Result is a Typelist having TList::Head as its Head
		// and the result of applying Erase to the tail of list as its
		// tail.
		template <class T>
		struct In
		{
			ASSERT_TYPELIST(TList);
			typedef typename TList::Head Head;
			typedef typename TList::Tail Tail;
			typedef typename Select
			<
				IsEqualType<Head, T>::value,	// is T equal to Head?
				Tail,							// Yes. Result is tail
				Typelist<typename TList::Head,	// No. recurse
				typename Erase<typename TList::Tail, T>::Result>
			>::Result Result;

		};
	};

	// if TList is NullType the result is NullType.
	template <>
	struct EraseImpl<NullType>
	{
		template <class T>
		struct In
		{
			typedef NullType Result;
		};
	};
}	// end of namespace Private

	template <class TList, class T>
	struct Erase
	{
		typedef typename Private::EraseImpl<TList>::template In<T>::Result Result;
	};

////////////////////////////////////////////////////////////////////////////////
// class template EraseAll
// Erases all occurences, if any, of a type in a typelist
// Invocation (TList is a typelist and T is a type):
// EraseAll<TList, T>::Result
// returns a typelist that is TList without any occurence of T
////////////////////////////////////////////////////////////////////////////////
	template <class TList, class T>
	struct EraseAll;
namespace Private
{
	template <class TList>
	struct EraseAllImpl
	{	// TList is not NullType.
		// Check if TList::Head is equal to T
		// If T is equal to TLIst::Head the result is the result of EraseAll
		// applied to TList::Tail
		//
		// If T is not equal to TList::Head the result is a Typelist with
		// TList::Head as its head and the result of applying EraseAll to TList::Tail
		// as its tail.
		template <class T>
		struct In
		{
			ASSERT_TYPELIST(TList);
			typedef typename TList::Head Head;
			typedef typename TList::Tail Tail;
			typedef typename Select
			<
				IsEqualType<Head, T>::value,				// is T == Head?
				typename EraseAll<Tail, T>::Result,			// Yes
				Typelist<Head, typename						// No
				EraseAll<Tail, T>::Result>
			>::Result Result;
		};
	};

	// if TList is NullType the result is NullType.
	template <>
	struct EraseAllImpl<NullType>
	{
		template <class T>
		struct In
		{
			typedef NullType Result;
		};
	};
}	// end of namespace Private

	template <class TList, class T>
	struct EraseAll
	{
		typedef typename Private::EraseAllImpl<TList>::template In<T>::Result Result;
	};

////////////////////////////////////////////////////////////////////////////////
// class template NoDuplicates
// Removes all duplicate types in a typelist
// Invocation (TList is a typelist):
// NoDuplicates<TList, T>::Result
////////////////////////////////////////////////////////////////////////////////
// NoDuplicates taken from Rani Sharoni's Loki VC7-Port.
	template <class TList>
    struct NoDuplicates
    {
    private:
        typedef typename TList::Head Head;
        typedef typename TList::Tail Tail;

        ASSERT_TYPELIST(TList);

        typedef typename NoDuplicates<Tail>::Result L1;
        typedef typename Erase<L1, Head>::Result    L2;

    public:
        typedef Typelist<Head, L2> Result;
    };

    template <>
    struct NoDuplicates<NullType>
    {
        typedef NullType Result;
    };
////////////////////////////////////////////////////////////////////////////////
// class template Replace
// Replaces the first occurence of a type in a typelist, with another type
// Invocation (TList is a typelist, T, U are types):
// Replace<TList, T, U>::Result
// returns a typelist in which the first occurence of T is replaced with U
////////////////////////////////////////////////////////////////////////////////
	template <class TList, class T, class U>
	struct Replace;
namespace Private
{
	// If TList is not NullType, check if T is equal to TList::Head
	template <class TList>
	struct ReplaceImpl
	{
		template <class T, class U>
		struct In
		{
			// If TList::Head is equal to T, the result is a typelist
			// with U as its head an TList::Tail as its tail.
			// If T is not equal to TList::Head, the result is a typelist
			// with TList::Head as its head and the result of applying
			// Replace to TList::Tail, T, U as its tail
			ASSERT_TYPELIST(TList);
			typedef typename TList::Head Head;
			typedef typename TList::Tail Tail;
			typedef typename Select
			<
				IsEqualType<T, Head>::value,	// Is T == Head?
				Typelist<U, Tail>,				// yes
				Typelist<Head, typename Replace<Tail, T, U>::Result>
			>::Result Result;
		};
	};

	// If TList is NullType the result is NullType
	template <>
	struct ReplaceImpl<NullType>
	{
		template <class T, class U>
		struct In
		{
			typedef NullType Result;
		};
	};
}	// end of namespace Private

	template <class TList, class T, class U>
	struct Replace
	{
		typedef typename
		Private::ReplaceImpl<TList>::template In<T, U>::Result Result;
	};

////////////////////////////////////////////////////////////////////////////////
// class template ReplaceAll
// Replaces all occurences of a type in a typelist, with another type
// Invocation (TList is a typelist, T, U are types):
// Replace<TList, T, U>::Result
// returns a typelist in which all occurences of T is replaced with U
////////////////////////////////////////////////////////////////////////////////
	template <class TList, class T, class U>
	struct ReplaceAll;

namespace Private
{
	// If TList is not NullType, check if T is equal to TList::Head
	template <class TList>
	struct ReplaceAllImpl
	{
		template <class T, class U>
		struct In
		{
			ASSERT_TYPELIST(TList);
			typedef typename TList::Head Head;
			typedef typename TList::Tail Tail;
			typedef typename Select
			<
				IsEqualType<T, Head>::value,							// Is T == Head?
				Typelist<U, typename ReplaceAll<Tail, T, U>::Result>,	// yes
				Typelist<Head, typename ReplaceAll<Tail, T, U>::Result>
			>::Result Result;
		};
	};

	// If TList is NullType the result is NullType
	template <>
	struct ReplaceAllImpl<NullType>
	{
		template <class T, class U>
		struct In
		{
			typedef NullType Result;
		};
	};
}
	template <class TList, class T, class U>
	struct ReplaceAll
	{
		typedef typename
		Private::ReplaceAllImpl<TList>::template In<T, U>::Result Result;
	};

////////////////////////////////////////////////////////////////////////////////
// class template Reverse
// Reverses a typelist
// Invocation (TList is a typelist):
// Reverse<TList>::Result
// returns a typelist that is TList reversed
////////////////////////////////////////////////////////////////////////////////
//	Reverse taken from Rani Sharoni's Loki VC7-Port.
	template <class TList> struct Reverse;

    template <>
    struct Reverse<NullType>
    {
        typedef NullType Result;
    };

    template <class TList>
    struct Reverse
    {
    private:
        typedef typename TList::Head Head;
        typedef typename TList::Tail Tail;

        ASSERT_TYPELIST(TList);

    public:
        typedef typename Append<
            typename Reverse<Tail>::Result, Head>::Result Result;
    };

////////////////////////////////////////////////////////////////////////////////
// class template MostDerived
// Finds the type in a typelist that is the most derived from a given type
// Invocation (TList is a typelist, T is a type):
// MostDerived<TList, T>::Result
// returns the type in TList that's the most derived from T
////////////////////////////////////////////////////////////////////////////////
	template <class TList, class T> struct MostDerived;
namespace Private
{
	template <class TList>
	struct MostDerivedImpl
	{
		template <class T>
		struct In
		{
			private:
				ASSERT_TYPELIST(TList);
				typedef typename TList::Head Head;
				typedef typename TList::Tail Tail;
				typedef typename MostDerived<Tail, T>::Result Candidate;
			public:
				typedef typename Select
				<
					SUPERSUBCLASS(Candidate, Head),
					Head,
					Candidate
				>::Result Result;
		};
	};
	template <>
	struct MostDerivedImpl<NullType>
	{
		template <class T>
		struct In {typedef T Result;};
	};
}	// end of namespace Private

	template <class TList, class T>
	struct MostDerived
	{
		typedef typename
		Private::MostDerivedImpl<TList>::template In<T>::Result Result;
	};
////////////////////////////////////////////////////////////////////////////////
// class template DerivedToFront
// Arranges the types in a typelist so that the most derived types appear first
// Invocation (TList is a typelist):
// DerivedToFront<TList>::Result
// returns the reordered TList
////////////////////////////////////////////////////////////////////////////////
	template <class TList>
    struct DerivedToFront
    {
    private:
        ASSERT_TYPELIST(TList);

        typedef typename TList::Head Head;
        typedef typename TList::Tail Tail;

        typedef typename MostDerived<Tail, Head>::Result TheMostDerived;
        typedef typename Replace<Tail, TheMostDerived, Head>::Result Temp;
        typedef typename DerivedToFront<Temp>::Result L;

    public:
        typedef Typelist<TheMostDerived, L> Result;
    };

	template <>
	struct DerivedToFront<NullType>
	{
		typedef NullType Result;
	};

////////////////////////////////////////////////////////////////////////////////
// class template DerivedToFrontAll
// Arranges all the types in a typelist so that the most derived types appear first
// Invocation (TList is a typelist):
// DerivedToFront<TList>::Result
// returns the reordered TList
////////////////////////////////////////////////////////////////////////////////
// DerivedToFrontAll taken from Rani Sharoni's Loki VC7-Port.
	template <class TList>
    struct DerivedToFrontAll
    {
		private:
			ASSERT_TYPELIST(TList);

			typedef typename TList::Head Head;
			typedef typename TList::Tail Tail;

			typedef typename MostDerived<Tail, Head>::Result TheMostDerived;
			typedef typename Replace<Tail, TheMostDerived, Head>::Result L;

			typedef typename DerivedToFrontAll<L>::Result TailResult;

		public:
			typedef Typelist<TheMostDerived, TailResult> Result;
    };

    template <>
    struct DerivedToFrontAll<NullType>
    {
        typedef NullType Result;
    };


	}	// end of namespace TL
}	// end of namespace Loki
////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 09, 2001: Fix bug in parameter list of macros TYPELIST_23 to TYPELIST_27
//      (credit due to Dave Taylor)
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// May  10, 2002: ported by Rani Sharoni to VC7 (RTM - 9466)
// Sept 29, 2002: ported by Benjamin Kaufmann to MSVC 6.0
// Feb	24, 2003: renamed MakeTypeList to MakeTypelist. Fixed a bug in
//					DerivedToFront.
////////////////////////////////////////////////////////////////////////////////
#endif // TYPELIST_INC_
