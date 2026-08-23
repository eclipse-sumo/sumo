#include "buffered_asynchronous_reader.h"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <string.h>
#include <assert.h>
#include <exception>
#include <stdexcept>
#include <string>

namespace RoutingKit{

const int blocks_per_buffer = 5;

struct BufferedAsynchronousReader::Impl{
	unsigned block_size;
	char*buffer;

	unsigned data_begin, data_end;

	bool was_end_of_file_reached;
	bool was_termination_requested;

	std::thread worker;

	std::exception_ptr read_exception;

	std::mutex lock;
	std::condition_variable main_thread_has_done_something;
	std::condition_variable worker_thread_has_done_something;

	~Impl(){
		{
			std::unique_lock<std::mutex>guard(lock);
			was_termination_requested = true;
		}
		main_thread_has_done_something.notify_one();
		worker.join();

		delete[]buffer;
	}

	unsigned how_many_bytes_are_in_the_buffer() const {
		if(data_begin <= data_end){
			return data_end - data_begin;
		}else{
			return blocks_per_buffer*block_size - (data_begin - data_end);
		}
	}
};

BufferedAsynchronousReader::BufferedAsynchronousReader(){}

BufferedAsynchronousReader::BufferedAsynchronousReader(BufferedAsynchronousReader&&o){
	impl = std::move(o.impl);
}

BufferedAsynchronousReader&BufferedAsynchronousReader::operator=(BufferedAsynchronousReader&&o){
	impl = std::move(o.impl);
	return *this;
}

BufferedAsynchronousReader::~BufferedAsynchronousReader(){
}

BufferedAsynchronousReader::BufferedAsynchronousReader(
	std::function<unsigned long long(char*, unsigned long long)>byte_source, unsigned block_size
):
	impl(std::unique_ptr<Impl>(new Impl))
{
	assert(byte_source && "byte_source must not be 0");

	impl->block_size = block_size;
	impl->buffer = new char[(blocks_per_buffer+1)*block_size];

	std::unique_lock<std::mutex>guard(impl->lock);
	impl->was_termination_requested = false;
	impl->was_end_of_file_reached = false;

	impl->data_begin = 0;
	impl->data_end = 0;

	Impl*ptr = impl.get();

	impl->worker = std::thread(
		[ptr,byte_source,block_size]{
			std::unique_lock<std::mutex>guard(ptr->lock);
			try{
				for(;;){
					ptr->main_thread_has_done_something.wait(
						guard,
						[&]{
							return (blocks_per_buffer * block_size - ptr->how_many_bytes_are_in_the_buffer()) >= 2*block_size || ptr->was_termination_requested;
						}
					);

					if(ptr->was_termination_requested){
						return;
					}

					guard.unlock();
					unsigned long long bytes_read = byte_source(ptr->buffer + ptr->data_end, block_size);
					guard.lock();


					assert(bytes_read <= block_size);

					if(ptr->was_termination_requested){
						return;
					}

					if(bytes_read == 0){
						ptr->was_end_of_file_reached = true;
						ptr->worker_thread_has_done_something.notify_one();
						return;
					}

					unsigned new_data_end = ptr->data_end + bytes_read;
					if(new_data_end >= blocks_per_buffer*block_size){
						new_data_end -= blocks_per_buffer*block_size;
						memcpy(ptr->buffer, ptr->buffer+blocks_per_buffer*block_size, new_data_end);
					}
					if(ptr->data_end < block_size){
						if(new_data_end < block_size)
							memcpy(ptr->buffer+blocks_per_buffer*block_size+ptr->data_end, ptr->buffer+ptr->data_end, bytes_read);
						else
							memcpy(ptr->buffer+blocks_per_buffer*block_size+ptr->data_end, ptr->buffer+ptr->data_end, block_size-ptr->data_end);
					}
					ptr->data_end = new_data_end;

					ptr->worker_thread_has_done_something.notify_one();
				}
			}catch(...){
				ptr->read_exception = std::current_exception();
				ptr->worker_thread_has_done_something.notify_one();
			}
		}
	);
}

char* BufferedAsynchronousReader::read(unsigned size) {
	if(size > impl->block_size)
		throw std::runtime_error("Requested to read " +std::to_string(size)+" bytes, which is more than the block size of "+std::to_string(impl->block_size));

	std::unique_lock<std::mutex>guard(impl->lock);

	impl->worker_thread_has_done_something.wait(
		guard,
		[&]{
			return impl->was_end_of_file_reached || impl->how_many_bytes_are_in_the_buffer() >= size || impl->read_exception;
		}
	);

	if(impl->read_exception){
		impl->was_end_of_file_reached = true;
		std::rethrow_exception(impl->read_exception);
	}

	if(impl->how_many_bytes_are_in_the_buffer() >= size){
		unsigned data_begin = impl->data_begin, block_size = impl->block_size;
		char* buffer = impl->buffer;

		char*ret = buffer + data_begin;
		data_begin += size;
		if(data_begin >= blocks_per_buffer*block_size)
			data_begin -= blocks_per_buffer*block_size;

		impl->data_begin = data_begin;
		impl->main_thread_has_done_something.notify_one();
		return ret;
	} else {
		assert(impl->was_end_of_file_reached);
		return 0;
	}
}

char* BufferedAsynchronousReader::read_or_throw(unsigned size){
	char*x = read(size);
	if(x == 0)
		throw std::runtime_error("Wanted to read "+std::to_string(size)+" bytes but only "+std::to_string(impl->how_many_bytes_are_in_the_buffer())+" are available in the data source.");
	return x;
}

} // namespace RoutingKit


