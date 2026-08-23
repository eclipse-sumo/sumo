#ifndef BUFFERED_ASYNCHRONOUS_READER_H
#define BUFFERED_ASYNCHRONOUS_READER_H

#include <functional>
#include <memory>

namespace RoutingKit{

class BufferedAsynchronousReader{
public:
	BufferedAsynchronousReader();
	template<class Source>
	BufferedAsynchronousReader(Source&source):
		BufferedAsynchronousReader(source.get_read_function_object(), source.minimum_read_size()){}
	BufferedAsynchronousReader(std::function<unsigned long long(char*, unsigned long long)>byte_source, unsigned block_size);

	BufferedAsynchronousReader(const BufferedAsynchronousReader&)=delete;
	BufferedAsynchronousReader&operator=(const BufferedAsynchronousReader&)=delete;

	BufferedAsynchronousReader(BufferedAsynchronousReader&&);
	BufferedAsynchronousReader&operator=(BufferedAsynchronousReader&&);

	~BufferedAsynchronousReader();

	char* read(unsigned size);
	char* read_or_throw(unsigned size);

private:
	struct Impl;
	std::unique_ptr<Impl>impl;
};

} // namespace RoutingKit

#endif
