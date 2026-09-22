#ifndef ROUTING_KIT_TIMESTAMP_FLAG_H
#define ROUTING_KIT_TIMESTAMP_FLAG_H

#include <vector>

namespace RoutingKit{

class TimestampFlags{
public:
	TimestampFlags(){}

	explicit TimestampFlags(unsigned id_count):last_seen(id_count, 0), current_timestamp(1){}

	bool is_set(unsigned id)const{
		return last_seen[id] == current_timestamp;
	}
	
	void set(unsigned id){
		last_seen[id] = current_timestamp;
	}

	void reset_one(unsigned id){
		last_seen[id] = current_timestamp-1;
	}

	void reset_all(){
		++current_timestamp;
		if(current_timestamp == 0){
			std::fill(last_seen.begin(), last_seen.end(), 0);
			current_timestamp = 1;
		}
	}

private:
	std::vector<unsigned short>last_seen;
	unsigned short current_timestamp;
};

} // RoutingKit

#endif

