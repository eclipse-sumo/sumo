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

// Last update: Feb 16, 2003
// Added isFunctionPointer to TypeTraits.
//
// This VC 6 port of TypeTraits is based on Rani Sharoni's Loki VC 7 port.
// Reference, pointer, array, const and volatile detection is based on
// boost's type traits.
// see http://www.boost.org/libs/type_traits/
//
// The code for Enum- and pointer-to-member-function detection is based on
// ideas from M. Yamada - many thanks :)
//
// AdjReference has moved to namespace-scope. Explicit specialization is
// only allowed there.
//
// known bugs:
// assert(TypeTraits<const void>::isConst == 1) fails.
// assert(TypeTraits<volatile void>::isVolatile == 1) fails.
// assert(TypeTraits<const volatile void>::isConst == 1) fails.
// assert(TypeTraits<const volatile void>::isVolatile == 1) fails.
// This is because the VC 6 does not differentiate
// between void, const void, volatile void and const volatile void.

#ifndef TYPETRAITS_INC_
#define TYPETRAITS_INC_

//
// Ignore forcing value to bool 'true' or 'false' (performance warning)
//
#ifdef _MSC_VER
#include <cctype>				// for wchar_t
#pragma warning (disable: 4800)
#endif

#include "Typelist.h"

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template IsCustomUnsignedInt
// Offers a means to integrate nonstandard built-in unsigned integral types
// (such as unsigned __int64 or unsigned long long int) with the TypeTraits
//     class template defined below.
// Invocation: IsCustomUnsignedInt<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in unsigned
//     integral type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomUnsignedInt
    {
        enum { value = 0 };
    };

////////////////////////////////////////////////////////////////////////////////
// class template IsCustomSignedInt
// Offers a means to integrate nonstandard built-in unsigned integral types
// (such as unsigned __int64 or unsigned long long int) with the TypeTraits
//     class template defined below.
// Invocation: IsCustomSignedInt<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in signed
//     integral type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomSignedInt
    {
        enum { value = 0 };
    };

////////////////////////////////////////////////////////////////////////////////
// class template IsCustomFloat
// Offers a means to integrate nonstandard floating point types with the
//     TypeTraits class template defined below.
// Invocation: IsCustomFloat<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in
//     floating point type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomFloat
    {
        enum { value = 0 };
    };

////////////////////////////////////////////////////////////////////////////////
// Helper types for class template TypeTraits defined below
////////////////////////////////////////////////////////////////////////////////
	namespace Private
    {
        typedef TYPELIST_4(unsigned char, unsigned short int,
           unsigned int, unsigned long int) StdUnsignedInts;
        typedef TYPELIST_4(signed char, short int,
           int, long int) StdSignedInts;
        typedef TYPELIST_3(bool, char, wchar_t) StdOtherInts;
        typedef TYPELIST_3(float, double, long double) StdFloats;
    }

////////////////////////////////////////////////////////////////////////////////
// class template TypeTraits
// Figures out various properties of any given type
// Invocations (T is a type):
// a) TypeTraits<T>::isPointer
// returns (at compile time) true if T is a pointer type
// b) TypeTraits<T>::PointeeType
// returns the type to which T points is T is a pointer type, NullType otherwise
// a) TypeTraits<T>::isReference
// returns (at compile time) true if T is a reference type
// b) TypeTraits<T>::ReferredType
// returns the type to which T refers is T is a reference type, NullType
// otherwise
// c) TypeTraits<T>::isMemberPointer
// returns (at compile time) true if T is a pointer to member type
// d) TypeTraits<T>::isStdUnsignedInt
// returns (at compile time) true if T is a standard unsigned integral type
// e) TypeTraits<T>::isStdSignedInt
// returns (at compile time) true if T is a standard signed integral type
// f) TypeTraits<T>::isStdIntegral
// returns (at compile time) true if T is a standard integral type
// g) TypeTraits<T>::isStdFloat
// returns (at compile time) true if T is a standard floating-point type
// h) TypeTraits<T>::isStdArith
// returns (at compile time) true if T is a standard arithmetic type
// i) TypeTraits<T>::isStdFundamental
// returns (at compile time) true if T is a standard fundamental type
// j) TypeTraits<T>::isUnsignedInt
// returns (at compile time) true if T is a unsigned integral type
// k) TypeTraits<T>::isSignedInt
// returns (at compile time) true if T is a signed integral type
// l) TypeTraits<T>::isIntegral
// returns (at compile time) true if T is a integral type
// m) TypeTraits<T>::isFloat
// returns (at compile time) true if T is a floating-point type
// n) TypeTraits<T>::isArith
// returns (at compile time) true if T is a arithmetic type
// o) TypeTraits<T>::isFundamental
// returns (at compile time) true if T is a fundamental type
// p) TypeTraits<T>::ParameterType
// returns the optimal type to be used as a parameter for functions that take Ts
// q) TypeTraits<T>::isConst
// returns (at compile time) true if T is a const-qualified type
// r) TypeTraits<T>::NonConstType
// removes the 'const' qualifier from T, if any
// s) TypeTraits<T>::isVolatile
// returns (at compile time) true if T is a volatile-qualified type
// t) TypeTraits<T>::NonVolatileType
// removes the 'volatile' qualifier from T, if any
// u) TypeTraits<T>::UnqualifiedType
// removes both the 'const' and 'volatile' qualifiers from T, if any
////////////////////////////////////////////////////////////////////////////////
	namespace Private
	{
		// const-detection based on boost's
		// Type-Traits. See: boost\type_traits\is_const.hpp
		YES IsConstTester(const volatile void*);
		NO IsConstTester(volatile void *);

		template <bool is_ref, bool array>
		struct IsConstImpl
		{
			template <class T> struct In {enum {value=0};};
		};
		template <>
		struct IsConstImpl<false,false>
		{
			template <typename T> struct In
			{
				static T* t;
				enum {value = sizeof(YES) == sizeof(IsConstTester(t))};
			};
		};
		template <>
		struct IsConstImpl<false,true>
		{
			template <typename T> struct In
			{
				static T t;
				enum { value = sizeof(YES) == sizeof(IsConstTester(&t)) };
			};
		};

		// this volatile-detection approach is based on boost's
		// Type-Traits. See: boost\type_traits\is_volatile.hpp
		YES IsVolatileTester(void const volatile*);
		NO IsVolatileTester(void const*);

		template <bool is_ref, bool array>
		struct IsVolatileImpl
		{
			template <typename T> struct In
			{
				enum {value = 0};
			};
		};

		template <>
		struct IsVolatileImpl<false,false>
		{
			template <typename T> struct In
			{
				static T* t;
				enum {value = sizeof(YES) == sizeof(IsVolatileTester(t))};
			};
		};

		template <>
		struct IsVolatileImpl<false,true>
		{
			template <typename T> struct In
			{
				static T t;
				enum {value = sizeof(YES) == sizeof(IsVolatileTester(&t))};
			};
		};

		template<bool IsRef>
        struct AdjReference
        {
            template<typename U>
            struct In { typedef U const & Result; };
        };

        template<>
        struct AdjReference<true>
        {
            template<typename U>
            struct In { typedef U Result; };
        };
		struct PointerHelper
		{
			PointerHelper(const volatile void*);
		};


		template <class T>
		NO EnumDetection(T);

		template <class T>
		YES  EnumDetection(...);

		YES IsPointer(PointerHelper);
		NO  IsPointer(...);

		// With the VC 6. Rani Sharoni's approach to detect references unfortunately
		// results in an error C1001: INTERNAL COMPILER-ERROR
		//
		// this reference-detection approach is based on boost's
		// Type-Traits. See: boost::composite_traits.h
		//
		// is_reference_helper1 is a function taking a Type2Type<T> returning
		// a pointer to a function taking a Type2Type<T> returning a T&.
		// This function can only be used if T is not a reference-Type.
		// If T is a reference Type the return type would be
		// a function taking a Type2Type<T> returning a reference to a T-reference.
		// That is illegal, therefore is_reference_helper1(...) is used for
		// references.
		// In order to detect a reference, use the return-type of is_reference_helper1
		// with is_reference_helper2.
		//
		template <class U>
		U&(* IsReferenceHelper1(::Loki::Type2Type<U>) )(::Loki::Type2Type<U>);
		NO	IsReferenceHelper1(...);

		template <class U>
		NO	IsReferenceHelper2(U&(*)(::Loki::Type2Type<U>));
		YES IsReferenceHelper2(...);

		template <class U, class Z>
		YES IsPointer2Member(Z U::*);
		NO	IsPointer2Member(...);

		// this array-detection approach is based on boost's
		// Type-Traits. See: boost::array_traits.hpp

		// This function can only be used for non-array-types, because
		// functions can't return arrays.
		template<class U>
		U(* IsArrayTester1(::Loki::Type2Type<U>) )(::Loki::Type2Type<U>);
		char IsArrayTester1(...);

		template<class U>
		NO IsArrayTester2(U(*)(::Loki::Type2Type<U>));
		YES IsArrayTester2(...);

		// Helper functions for function-pointer detection.
		// The code uses the fact, that arrays of functions are not allowed.
		// Of course TypeTraits first makes sure that U is neither void
		// nor a reference.type.
		// The idea for this code is from D Vandevoorde's & N. Josuttis'
		// book "C++ Templates".
		template<class U>
		NO IsFunctionPtrTester1(U*, U(*)[1] = 0);
		YES IsFunctionPtrTester1(...);
	}

	template <typename T>
    class TypeTraits
    {
	public:
		enum { isVoid = Private::IsVoid<T>::value};
		enum { isStdUnsignedInt =
            TL::IndexOf<Private::StdUnsignedInts, T>::value >= 0 };
        enum { isStdSignedInt =
            TL::IndexOf<Private::StdSignedInts, T>::value >= 0 };
        enum { isStdIntegral = isStdUnsignedInt || isStdSignedInt ||
            TL::IndexOf<Private::StdOtherInts, T>::value >= 0 };
        enum { isStdFloat = TL::IndexOf<Private::StdFloats, T>::value >= 0 };
        enum { isStdArith = isStdIntegral || isStdFloat };
        enum { isStdFundamental = isStdArith || isStdFloat || isVoid };

        enum { isUnsignedInt = isStdUnsignedInt || IsCustomUnsignedInt<T>::value };
        enum { isSignedInt = isStdSignedInt || IsCustomSignedInt<T>::value };
        enum { isIntegral = isStdIntegral || isUnsignedInt || isSignedInt };
        enum { isFloat = isStdFloat || IsCustomFloat<T>::value };
        enum { isArith = isIntegral || isFloat };
        enum { isFundamental = isStdFundamental || isArith || isFloat };

		enum { isArray	= sizeof(Private::YES)
						== sizeof(Private::IsArrayTester2(
									Private::IsArrayTester1(::Loki::Type2Type<T>()))
							)
		};

		enum { isReference	= sizeof(Private::YES)
							== sizeof(Private::IsReferenceHelper2(
										Private::IsReferenceHelper1(::Loki::Type2Type<T>()))
								) && !isVoid
		};
		enum { isConst	= Private::IsConstImpl
						<isReference, isArray>::template In<T>::value
        };

        enum { isVolatile = Private::IsVolatileImpl
							<isReference, isArray>::template In<T>::value
        };
	private:
        typedef typename Private::AdjReference<isReference || isVoid>::
		template In<T>::Result AdjType;

		struct is_scalar
        {
        private:
            struct BoolConvert { BoolConvert(bool); };

            static Private::YES check(BoolConvert);
            static Private::NO check(...);

            struct NotScalar {};

            typedef typename Select
            <
                isVoid || isReference || isArray,
                NotScalar, T
            >
            ::Result RetType;

            // changed to RetType& to allow testing of abstract classes
			static RetType& get();

        public:
			enum { value = sizeof(check(get())) == sizeof(Private::YES) };


        }; // is_scalar

    public:
		enum { isScalar = is_scalar::value};
		typedef typename Select
        <
            isScalar || isArray, T, AdjType
        >
        ::Result ParameterType;
	private:

		typedef typename Loki::Select
		<
			isScalar,
			T,
			int
		>::Result TestType;
		static TestType MakeT();

		enum { isMemberPointerTemp = sizeof(Private::YES)
									== sizeof(Private::IsPointer2Member(MakeT()))
		};
	public:
		enum {isPointer = sizeof(Private::YES)
						== sizeof(Private::IsPointer(MakeT()))};
	private:
		typedef typename Loki::Select
		<
			isVoid || isReference || !isPointer,
			int*,
			T
		>::Result MayBeFuncPtr;
	public:
		// enum types are the only scalar types that can't be initialized
		// with 0.
		// Because we replace all non scalars with int,
		// template <class T>
		// YES EnumDetection(...);
		// will only be selected for enums.
		enum { isEnum = sizeof(Private::YES)
						== sizeof (Private::EnumDetection<TestType>(0))
         };

		enum { isMemberFunctionPointer =	isScalar && !isArith && !isPointer &&
										!isMemberPointerTemp && !isEnum
		};
		enum { isMemberPointer = isMemberPointerTemp || isMemberFunctionPointer};

		enum { isFunctionPointer = sizeof(Private::YES)
								== sizeof(
								Private::IsFunctionPtrTester1(MayBeFuncPtr(0))
								) && !isMemberPointer
		};
		//
        // We get is_class for free
        // BUG - fails with functions types (ICE) and unknown size array
        // (but works for other incomplete types)
        // (the boost one (Paul Mensonides) is better)
        //
        enum { isClass =
                !isScalar    &&
                !isArray     &&
                !isReference &&
                !isVoid		 &&
				!isEnum
        };
    };

}
#ifdef _MSC_VER
#pragma warning (default: 4800)
#endif

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// May  10, 2002: ported by Rani Sharoni to VC7 (RTM - 9466)
// Oct	05, 2002: ported by Benjamin Kaufmann to MSVC 6
// Jan	31, 2003: fixed bugs in scalar and array detection.
//					Added isMemberFuncPointer and isEnum. B.K.
//
// Feb	16,	2003: fixed bug in reference-Detection. Renamed isMemberFuncPointer
//					to isMemberFunctionPointer. Added isFunctionPointer, replaced
//					all occurrences of Private::Wrap with Loki::Type2Type and
//					cleaned up the TypeTraits-class.	B.K.
////////////////////////////////////////////////////////////////////////////////

#endif // TYPETRAITS_INC_
