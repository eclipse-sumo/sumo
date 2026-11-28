cmake_minimum_required(VERSION 3.20)
project(LibsumoConsumer LANGUAGES CXX)

find_package(SUMO CONFIG REQUIRED NO_DEFAULT_PATH)
add_executable(libsumo_consumer main.cpp)
target_compile_features(libsumo_consumer PRIVATE cxx_std_17)
target_link_libraries(libsumo_consumer PRIVATE SUMO::libsumocpp)
# Keep the Windows executable next to its installed DLLs in every configuration.
set_target_properties(libsumo_consumer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<1:${SUMO_INSTALL_BIN}>")

enable_testing()
add_test(NAME libsumo_consumer COMMAND libsumo_consumer)
