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

// Last update: Oct 17, 2002

#include "static_check.h"

#ifndef TYPEMANIP_INC_
#define TYPEMANIP_INC_

namespace Loki
{
	namespace Private
	{
		typedef char YES;
		struct NO {char dummy[3];};
	}
////////////////////////////////////////////////////////////////////////////////
// class template Int2Type
// Converts each integral constant into a unique type
// Invocation: Int2Type<v> where v is a compile-time constant integral
// Defines 'value', an enum that evaluates to v
////////////////////////////////////////////////////////////////////////////////

    template <int v>
    struct Int2Type
    {
        Int2Type() {}
		enum { value = v };
    };

////////////////////////////////////////////////////////////////////////////////
// class template Type2Type
// Converts each type into a unique, insipid type
// Invocation Type2Type<T> where T is a type
// Defines the type OriginalType which maps back to T
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct Type2Type
    {
        typedef T OriginalType;
        Type2Type() {};
    };

////////////////////////////////////////////////////////////////////////////////
// class template Select
// Selects one of two types based upon a boolean constant
// Invocation: Select<flag, T, U>::Result
// where:
// flag is a compile-time boolean constant
// T and U are types
// Result evaluates to T if flag is true, and to U otherwise.
////////////////////////////////////////////////////////////////////////////////
// VC6 compatible version
namespace Private
{
	template <bool>
	struct SelectImpl
	{
		template <class T, class U>
		struct In
		{
			typedef T Result;
		};
	};

	template <>
	struct SelectImpl<false>
	{
		template <class T, class U>
		struct In
		{
			typedef U Result;
		};
	};

}	// end of namespace private
    template <bool flag, typename T, typename U>
    struct Select
    {
		typedef typename Private::SelectImpl<flag>::template In<T, U>::Result Result;
	};


////////////////////////////////////////////////////////////////////////////////
// class template IsEqualType
// Returns true if the two given types are equal
// Invocation: IsEqualType<T, U>::value
// where:
// T and U are types
// Result evaluates to true if U == T (types equal)
////////////////////////////////////////////////////////////////////////////////
//	This template is not in the original Loki-Library
	template <class T, class U>
	struct IsEqualType
	{
		private:
			static Private::YES check(Type2Type<T>);
			static Private::NO check(...);
		public:
			enum {value = sizeof(check(Type2Type<U>())) == sizeof(Private::YES)};
	};
////////////////////////////////////////////////////////////////////////////////
// Helper types Small and Big - guarantee that sizeof(Small) < sizeof(Big)
////////////////////////////////////////////////////////////////////////////////
    namespace Private
    {
        typedef char Small;
        class Big { char dummy[2]; };

// IsVoid from Rani Sharoni's VC 7 port
        template<typename T>
        struct IsVoid
        {
            enum { value =
                IsEqualType<T, void>::value          ||
                IsEqualType<T, const void>::value    ||
                IsEqualType<T, volatile void>::value ||
                IsEqualType<T, const volatile void>::value
            };
        };
    }

//
// is one type convertable to another?
//
// is_convertible from Rani Sharoni's VC 7 port.
    template <class T, class U>
    class is_convertible
    {
        struct VoidReplace {};

        typedef typename Select
        <
            Private::IsVoid<T>::value,
            VoidReplace, T
        >
        ::Result T1;

        typedef typename Select
        <
            Private::IsVoid<U>::value,
            VoidReplace, U
        >
        ::Result U1;

        static Private::Big   Test(...);
        static Private::Small Test(U1);
        static T1 MakeT();

    public:
        enum { exists = sizeof(Test(MakeT())) == sizeof(Private::Small) };
    };

////////////////////////////////////////////////////////////////////////////////
// class template Conversion
// Figures out the conversion relationships between two types
// Invocations (T and U are types):
// a) Conversion<T, U>::exists
// returns (at compile time) true if there is an implicit conversion from T
// to U (example: Derived to Base)
// b) Conversion<T, U>::exists2Way
// returns (at compile time) true if there are both conversions from T
// to U and from U to T (example: int to char and back)
// c) Conversion<T, U>::sameType
// returns (at compile time) true if T and U represent the same type
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////
//	Conversion-Template from Rani Sharoni's VC 7 port.
    template <class T, class U>
    struct Conversion
    {
        enum { exists = (is_convertible<T,U>::exists) };
        enum { exists2Way = (exists && is_convertible<U, T>::exists) };
        enum { sameType = (IsEqualType<T, U>::value) };
    };

////////////////////////////////////////////////////////////////////////////////
// class template SuperSubclass
// Invocation: SuperSubclass<B, D>::value where B and D are types.
// Returns true if B is a public base of D, or if B and D are aliases of the
// same type.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

template <class T, class U>
struct SuperSubclass
{
  enum { value = (::Loki::Conversion<const volatile U*, const volatile T*>::exists &&
                  !::Loki::Conversion<const volatile T*, const volatile void*>::sameType) };
};

////////////////////////////////////////////////////////////////////////////////
// class template SuperSubclassStrict
// Invocation: SuperSubclassStrict<B, D>::value where B and D are types.
// Returns true if B is a public base of D.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

template<class T,class U>
struct SuperSubclassStrict
{
  enum { value = (::Loki::Conversion<const volatile U*, const volatile T*>::exists &&
                 !::Loki::Conversion<const volatile T*, const volatile void*>::sameType &&
                 !::Loki::Conversion<const volatile T*, const volatile U*>::sameType) };
};

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS
// Invocation: SUPERSUBCLASS(B, D) where B and D are types.
// Returns true if B is a public base of D, or if B and D are aliases of the
// same type.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

#define SUPERSUBCLASS(T, U) \
    (::Loki::Conversion<const volatile U*, const volatile T*>::exists && \
    !::Loki::Conversion<const volatile T*, const volatile void*>::sameType)

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS
// Invocation: SUPERSUBCLASS(B, D) where B and D are types.
// Returns true if B is a public base of D.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

#define SUPERSUBCLASS_STRICT(T, U) \
    (SUPERSUBCLASS(T, U) && \
    !::Loki::Conversion<const volatile T *, const volatile U *>::sameType)



template <unsigned i>
struct TypeTag
{
	struct Inner {char c[i];};
	typedef Inner X;
	STATIC_SIZE_ASSERT(X, i);
};

} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// May  10, 2002: ported by Rani Sharoni to VC7 (RTM - 9466)
// Sept 29, 2002: ported by Benjamin Kaufmann to MSVC 6
////////////////////////////////////////////////////////////////////////////////

#endif // TYPEMANIP_INC_

