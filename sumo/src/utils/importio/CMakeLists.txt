set(utils_importio_STAT_SRCS
   LineReader.cpp
   NamedColumnsParser.cpp
   LineReader.h
   NamedColumnsParser.h
)

add_library(utils_importio STATIC ${utils_importio_STAT_SRCS})
