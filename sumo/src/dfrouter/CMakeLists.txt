set(dfrouter_SRCS
   dfrouter_main.cpp
   RODFDetFlowLoader.cpp
   RODFDetFlowLoader.h
   RODFDetector.cpp
   RODFDetector.h
   RODFDetectorFlow.cpp
   RODFDetectorFlow.h
   RODFDetectorHandler.cpp
   RODFDetectorHandler.h
   RODFNet.cpp
   RODFNet.h
   RODFRouteCont.cpp
   RODFRouteCont.h
   RODFRouteDesc.h
   RODFEdgeBuilder.cpp
   RODFEdgeBuilder.h
   RODFEdge.cpp
   RODFEdge.h
   RODFFrame.cpp
   RODFFrame.h
)

add_executable(dfrouter ${dfrouter_SRCS})

target_link_libraries(dfrouter router ${commonvehiclelibs})

install(TARGETS dfrouter DESTINATION bin)
