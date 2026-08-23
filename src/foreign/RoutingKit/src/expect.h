#ifndef EXPECT_H
#define EXPECT_H

#include <iostream>

namespace RoutingKit{

extern bool expect_failed;

namespace detail{
	template<class T>
	const T&handle_null_ptr(const T&t){return t;}
	inline const char* handle_null_ptr(const char*p) { if(p) return p; else return "nullptr";}
	inline const char* handle_null_ptr(const decltype(nullptr)) {return "nullptr";}

}


#define EXPECT2(c, c_msg, line) \
	((c) ? std::cerr : (expect_failed = true, std::cerr << c_msg <<" failed in line "<<line << std::endl))

#define EXPECT(c) \
	EXPECT2(c, #c, __LINE__)



#define EXPECT_CMP2(x, op, y, x_msg, op_msg, y_msg, line) \
	(((x) op (y)) ? std::cerr : (expect_failed = true, std::cerr << x_msg << ' ' <<op_msg << ' ' << y_msg << " failed in line "<<line <<"\n"<<x_msg<<" is "<<::RoutingKit::detail::handle_null_ptr(x)<<"\n"<<y_msg<<" is "<<::RoutingKit::detail::handle_null_ptr(y) << std::endl))

#define EXPECT_CMP(x, op, y) \
	EXPECT_CMP2(x, op, y, #x, #op, #y, __LINE__)




}

#endif
