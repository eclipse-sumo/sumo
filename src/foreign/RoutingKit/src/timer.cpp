#include <routingkit/timer.h>

// #include <sys/time.h>
#include <chrono>

namespace RoutingKit{

//long long get_micro_time(){
//	timeval t;
//	gettimeofday(&t, 0);
//	return t.tv_sec*1000000ll+t.tv_usec;
//}

long long get_micro_time(){
	return std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
}

} // RoutingKit
