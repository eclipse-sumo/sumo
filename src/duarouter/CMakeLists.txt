set(duarouter_SRCS
   duarouter_main.cpp
   RODUAEdgeBuilder.cpp
   RODUAEdgeBuilder.h
   RODUAFrame.cpp
   RODUAFrame.h
)
if (HAVE_ROUTINGKIT)
    list(APPEND duarouter_SRCS
       RODUACCHGraph.cpp
       RODUACCHGraph.h
    )
endif()

add_executable(duarouter ${duarouter_SRCS})
set_target_properties(duarouter PROPERTIES OUTPUT_NAME duarouter${BINARY_SUFFIX})
set_target_properties(duarouter PROPERTIES OUTPUT_NAME_DEBUG duarouter${BINARY_SUFFIX}D)
target_link_libraries(duarouter router ${commonvehiclelibs} ${TCMALLOC_LIBRARY})
if (HAVE_ROUTINGKIT)
    target_link_libraries(duarouter ${ROUTINGKIT_LIB})
    if (TARGET OpenMP::OpenMP_CXX)
        target_link_libraries(duarouter OpenMP::OpenMP_CXX)
    elseif (OMP_LIB)
        target_link_libraries(duarouter ${OMP_LIB})
    endif ()
endif()
add_dependencies(duarouter generate-version-h install_dll)

install(TARGETS duarouter RUNTIME DESTINATION bin COMPONENT runtime)
