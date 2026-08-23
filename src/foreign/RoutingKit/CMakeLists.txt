set(foreign_routingkit_STAT_SRCS
    include/routingkit/bit_vector.h
    include/routingkit/constants.h
    include/routingkit/contraction_hierarchy.h
    include/routingkit/customizable_contraction_hierarchy.h
    include/routingkit/filter.h
    include/routingkit/graph_util.h
    include/routingkit/id_mapper.h
    include/routingkit/id_queue.h
    include/routingkit/id_set_queue.h
    include/routingkit/inverse_vector.h
    include/routingkit/min_max.h
    include/routingkit/nested_dissection.h
    include/routingkit/permutation.h
    include/routingkit/sort.h
    include/routingkit/timer.h
    include/routingkit/timestamp_flag.h
    include/routingkit/vector_io.h
    src/bit_select.h
    src/bit_select.cpp
    src/bit_vector.cpp
    src/contraction_hierarchy.cpp
    src/customizable_contraction_hierarchy.cpp
    src/emulate_gcc_builtin.h
    src/expect.h
    src/expect.cpp
    src/graph_util.cpp
    src/id_mapper.cpp
    src/nested_dissection.cpp
    src/timer.cpp
)

add_library(foreign_routingkit STATIC ${foreign_routingkit_STAT_SRCS})
target_include_directories(foreign_routingkit PUBLIC include)
if (MSVC)
    target_compile_definitions(foreign_routingkit PRIVATE ROUTING_KIT_NO_GCC_EXTENSIONS)
endif ()
set_property(TARGET foreign_routingkit PROPERTY PROJECT_LABEL "z_foreign_routingkit")
if (SUMO_UTILS)
    install(TARGETS foreign_routingkit DESTINATION lib COMPONENT runtime)
endif ()
